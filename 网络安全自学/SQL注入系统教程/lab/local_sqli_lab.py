#!/usr/bin/env python3
"""Intentionally vulnerable localhost-only SQL injection training application."""

from __future__ import annotations

import html
import sqlite3
import threading
import time
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse


HOST = "127.0.0.1"
PORT = 8765


def create_database() -> sqlite3.Connection:
    connection = sqlite3.connect(":memory:", check_same_thread=False)
    connection.create_function("lab_sleep", 1, lambda seconds: time.sleep(float(seconds)) or 0)
    connection.executescript(
        """
        CREATE TABLE lab_users (
            id INTEGER PRIMARY KEY,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL,
            role TEXT NOT NULL
        );

        CREATE TABLE products (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            price REAL NOT NULL,
            training_flag TEXT NOT NULL
        );

        INSERT INTO lab_users (id, username, password, role) VALUES
            (1, 'alice', 'lab-alice-password', 'admin'),
            (2, 'bob', 'lab-bob-password', 'user');

        INSERT INTO products (id, name, category, price, training_flag) VALUES
            (1, 'SQL 入门书', 'book', 59.90, 'LAB{parameterize-values}'),
            (2, '机械键盘', 'device', 399.00, 'LAB{allowlist-identifiers}'),
            (3, '显示器支架', 'device', 199.00, 'LAB{least-privilege}');
        """
    )
    return connection


DATABASE = create_database()
DATABASE_LOCK = threading.Lock()


def page(title: str, body: str) -> bytes:
    document = f"""<!doctype html>
<html lang="zh-CN">
<head><meta charset="utf-8"><title>{html.escape(title)}</title></head>
<body><h1>{html.escape(title)}</h1>{body}</body>
</html>"""
    return document.encode("utf-8")


def render_rows(rows: list[sqlite3.Row] | list[tuple]) -> str:
    if not rows:
        return "<p>No rows</p>"
    items = []
    for row in rows:
        values = " | ".join(html.escape(str(value)) for value in row)
        items.append(f"<li>{values}</li>")
    return "<ul>" + "".join(items) + "</ul>"


class LabHandler(BaseHTTPRequestHandler):
    server_version = "LocalSQLiLab/1.0"

    def log_message(self, format_string: str, *args: object) -> None:
        print(f"[http] {self.address_string()} {format_string % args}")

    def send_page(
        self,
        title: str,
        body: str,
        status: HTTPStatus = HTTPStatus.OK,
    ) -> None:
        payload = page(title, body)
        self.send_response(status)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(payload)))
        self.end_headers()
        self.wfile.write(payload)

    def execute_vulnerable(self, sql: str) -> list[tuple]:
        print(f"[vulnerable SQL] {sql}")
        with DATABASE_LOCK:
            return DATABASE.execute(sql).fetchall()

    def do_GET(self) -> None:  # noqa: N802 - required by BaseHTTPRequestHandler
        parsed = urlparse(self.path)
        params = parse_qs(parsed.query, keep_blank_values=True)
        value = lambda name, default="": params.get(name, [default])[0]

        try:
            if parsed.path == "/":
                self.send_page(
                    "Local SQL Injection Lab",
                    """
                    <ul>
                      <li><a href="/product?id=1">/product?id=1</a></li>
                      <li><a href="/search?q=SQL">/search?q=SQL</a></li>
                      <li><a href="/login?username=alice&password=wrong">/login</a></li>
                      <li><a href="/blind?id=1">/blind?id=1</a></li>
                      <li><a href="/time?id=1">/time?id=1</a></li>
                      <li><a href="/safe/product?id=1">/safe/product?id=1</a></li>
                    </ul>
                    """,
                )
                return

            if parsed.path == "/product":
                product_id = value("id", "1")
                sql = (
                    "SELECT id, name, price FROM products WHERE id = "
                    + product_id
                )
                self.send_page("Vulnerable product", render_rows(self.execute_vulnerable(sql)))
                return

            if parsed.path == "/search":
                query = value("q")
                sql = (
                    "SELECT id, name, price FROM products "
                    f"WHERE name LIKE '%{query}%'"
                )
                self.send_page("Vulnerable search", render_rows(self.execute_vulnerable(sql)))
                return

            if parsed.path == "/login":
                username = value("username")
                password = value("password")
                sql = (
                    "SELECT id, username, role FROM lab_users "
                    f"WHERE username = '{username}' AND password = '{password}'"
                )
                rows = self.execute_vulnerable(sql)
                if rows:
                    self.send_page("Vulnerable login", f"<p>Welcome {html.escape(str(rows[0][1]))}</p>")
                else:
                    self.send_page("Vulnerable login", "<p>Login failed</p>")
                return

            if parsed.path in {"/blind", "/time"}:
                product_id = value("id", "1")
                sql = "SELECT id FROM products WHERE id = " + product_id
                rows = self.execute_vulnerable(sql)
                body = "<p>Found</p>" if rows else "<p>Not found</p>"
                self.send_page("Blind result", body)
                return

            if parsed.path == "/safe/product":
                product_id = value("id", "1")
                sql = "SELECT id, name, price FROM products WHERE id = ?"
                print(f"[safe SQL template] {sql} | parameter={product_id!r}")
                with DATABASE_LOCK:
                    rows = DATABASE.execute(sql, (product_id,)).fetchall()
                self.send_page("Safe product", render_rows(rows))
                return

            self.send_page("Not found", "<p>Unknown route</p>", HTTPStatus.NOT_FOUND)
        except sqlite3.Error as error:
            # Deliberately verbose for the vulnerable local lab.
            self.send_page(
                "Database error",
                f"<pre>{html.escape(str(error))}</pre>",
                HTTPStatus.INTERNAL_SERVER_ERROR,
            )


def main() -> None:
    server = ThreadingHTTPServer((HOST, PORT), LabHandler)
    print(f"Local SQL injection lab: http://{HOST}:{PORT}/")
    print("Press Ctrl+C to stop. Do not expose this intentionally vulnerable app.")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping lab.")
    finally:
        server.server_close()
        DATABASE.close()


if __name__ == "__main__":
    main()
