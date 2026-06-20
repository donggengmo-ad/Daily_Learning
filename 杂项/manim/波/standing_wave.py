"""
驻波曲面演示 — 两列行波 y(x,t) 曲面叠加形成驻波
================================================
y₁(x,t) = A·sin(kx - ωt)         右行波曲面 (青蓝)
y₂(x,t) = A·sin(kx + ωt)         左行波曲面 (品红)
y(x,t)  = 2A·sin(kx)·cos(ωt)     驻波曲面   (金色)

最终俯视图 → X-T 棋盘：白=y>0, 黒=y<0, 网格线标波节+零时间点
"""

from manim import *
import numpy as np

# ── xelatex 中文模板 ──────────────────────────────────────────
_cjk_template = TexTemplate(
    tex_compiler="xelatex",
    output_format=".xdv",
    documentclass=r"\documentclass[preview]{standalone}",
    preamble=r"""
\usepackage{amsmath}
\usepackage{amssymb}
\usepackage{fontspec}
\setmainfont{PingFang SC}
\setsansfont{PingFang SC}
\setmonofont{Menlo}
""",
)
config.tex_template = _cjk_template
# ──────────────────────────────────────────────────────────────


class StandingWaveSurface(ThreeDScene):
    """三维曲面：两列行波 → 驻波 → 俯视棋盘"""

    def construct(self):
        # ============================================================
        # 1. 参数
        # ============================================================
        A = 1.0
        k = 2 * PI
        omega = 2 * PI
        x_max = 2.0
        t_max = 2.0
        SURF_RES = 22       # 降分辨率加速

        # ============================================================
        # 2. 相机
        # ============================================================
        self.set_camera_orientation(
            phi=60 * DEGREES,
            theta=-50 * DEGREES,
            zoom=0.7,
        )

        # ============================================================
        # 3. 三维坐标轴 — X=位置(右) Y=时间(上) Z=振幅(深)
        # ============================================================
        axes = ThreeDAxes(
            x_range=[0, x_max, 0.5],
            y_range=[0, t_max, 0.5],
            z_range=[-2.5, 2.5, 0.5],
            x_length=6,
            y_length=5,
            z_length=5,
            axis_config={"include_numbers": True, "font_size": 14},
        )

        x_lbl = Tex("$x$ 位置", font_size=28)
        y_lbl = Tex("$t$ 时间", font_size=28)
        z_lbl = Tex("$y$ 振幅", font_size=28)

        self.play(Create(axes), run_time=1.2)
        self.play(
            Write(x_lbl.next_to(axes.c2p(2.3, 0, 0), RIGHT)),
            Write(y_lbl.next_to(axes.c2p(0, 2.3, 0), OUT)),
            Write(z_lbl.next_to(axes.c2p(0, 0, 2.7), UP)),
            run_time=0.8,
        )
        self.wait(0.2)

        # ============================================================
        # 4. 标题 + 公式（固定屏幕）
        # ============================================================
        title = Tex("驻波曲面：两列行波的叠加", font_size=36).to_corner(UL)

        eq1 = MathTex(
            r"y_1 = A\sin(kx - \omega t)", font_size=26, color=TEAL_D
        )
        eq2 = MathTex(
            r"y_2 = A\sin(kx + \omega t)", font_size=26, color=MAROON_D
        )
        eq3 = MathTex(
            r"y = 2A\sin(kx)\cos(\omega t)", font_size=26, color=GOLD,
        )
        formulas = VGroup(eq1, eq2, eq3).arrange(
            DOWN, aligned_edge=LEFT, buff=0.15
        )
        formulas.to_corner(UR).shift(DOWN * 0.6)

        self.add_fixed_in_frame_mobjects(title, formulas)
        self.play(Write(title), Write(formulas), run_time=1.5)
        self.wait(0.2)

        # ============================================================
        # 5. 图例
        # ============================================================
        def legend_entry(color, text):
            box = Square(side_length=0.3, color=color,
                         fill_color=color, fill_opacity=0.7, stroke_width=1)
            label = Tex(text, font_size=22, color=color)
            return VGroup(box, label).arrange(RIGHT, buff=0.12)

        legend = VGroup(
            legend_entry(TEAL_D, r"$y_1$ 右行波"),
            legend_entry(MAROON_D, r"$y_2$ 左行波"),
            legend_entry(GOLD, r"$y_1+y_2$ 驻波"),
        ).arrange(DOWN, aligned_edge=LEFT, buff=0.15)
        legend.to_corner(DL).shift(UP * 0.25)

        self.add_fixed_in_frame_mobjects(legend)
        self.play(FadeIn(legend), run_time=0.6)

        # ============================================================
        # 6. 曲面 — c2p(x, t, y) → X=位置 Y=时间 Z=振幅
        # ============================================================
        offset = ValueTracker(1.5)

        # 右行波 — 青蓝
        c_blue = interpolate_color(TEAL, BLUE, 0.6)
        surface_right = always_redraw(lambda: Surface(
            lambda u, v: axes.c2p(
                u, v, offset.get_value() + A * np.sin(k * u - omega * v)
            ),
            u_range=[0, x_max],
            v_range=[0, t_max],
            resolution=(SURF_RES, SURF_RES),
            fill_color=c_blue,
            fill_opacity=0.80,
            stroke_width=0,
        ))

        # 左行波 — 品红
        c_red = interpolate_color(MAROON, PURE_RED, 0.4)
        surface_left = always_redraw(lambda: Surface(
            lambda u, v: axes.c2p(
                u, v, -offset.get_value() + A * np.sin(k * u + omega * v)
            ),
            u_range=[0, x_max],
            v_range=[0, t_max],
            resolution=(SURF_RES, SURF_RES),
            fill_color=c_red,
            fill_opacity=0.80,
            stroke_width=0,
        ))

        # 驻波 — 金
        surface_standing = always_redraw(lambda: Surface(
            lambda u, v: axes.c2p(
                u, v, 2 * A * np.sin(k * u) * np.cos(omega * v)
            ),
            u_range=[0, x_max],
            v_range=[0, t_max],
            resolution=(SURF_RES, SURF_RES),
            fill_color=GOLD,
            fill_opacity=0.75,
            stroke_width=0,
        ))

        # ============================================================
        # 7. 波节线
        # ============================================================
        node_x = [0, 0.5, 1.0, 1.5, 2.0]
        node_lines = VGroup(*[
            ParametricFunction(
                lambda v, nx=nx: axes.c2p(nx, v, 0),
                t_range=[0, t_max],
                color=PURPLE,
                stroke_width=4,
            )
            for nx in node_x
        ])

        node_text = Tex("波节线", font_size=20, color=PURPLE)
        self.add_fixed_in_frame_mobjects(node_text)
        node_text.next_to(legend, DOWN, aligned_edge=LEFT, buff=0.2)

        # ============================================================
        # 8. 提示文字
        # ============================================================
        hint = always_redraw(
            lambda: Tex("", font_size=26).to_edge(DOWN, buff=0.35)
        )
        self.add_fixed_in_frame_mobjects(hint)

        # ============================================================
        # 9. 动画流程
        # ============================================================

        # ── 阶段 1：两个分离曲面 ──
        hint.become(
            Tex("两列反向传播的行波曲面", font_size=26, color=WHITE)
            .to_edge(DOWN, buff=0.35)
        )
        self.begin_ambient_camera_rotation(rate=0.08)
        self.play(
            FadeIn(surface_right),
            FadeIn(surface_left),
            run_time=2,
        )
        self.wait(2)
        self.stop_ambient_camera_rotation()
        self.wait(0.3)

        # ── 阶段 2：靠拢叠加 ──
        hint.become(
            Tex("两列波向中间靠拢、叠加...", font_size=26, color=WHITE)
            .to_edge(DOWN, buff=0.35)
        )
        self.play(
            offset.animate.set_value(0),
            run_time=2.5,
            rate_func=smooth,
        )
        self.wait(0.3)

        # ── 阶段 3：驻波浮现 ──
        hint.become(
            Tex(
                r"驻波：$y = 2A\sin(kx)\cos(\omega t)$",
                font_size=26, color=GOLD,
            ).to_edge(DOWN, buff=0.35)
        )
        self.play(FadeIn(surface_standing), run_time=1.5)
        self.wait(0.3)

        # ── 阶段 4：隐藏行波，显示波节 ──
        self.play(
            FadeOut(surface_right),
            FadeOut(surface_left),
        )
        self.wait(0.2)

        hint.become(
            Tex("波节线在空间中固定不动", font_size=24, color=PURPLE)
            .to_edge(DOWN, buff=0.35)
        )
        self.play(
            Create(node_lines, run_time=1.5),
            Write(node_text),
            run_time=2,
        )
        self.wait(0.3)

        # ── 阶段 5：自由旋转 ──
        self.begin_ambient_camera_rotation(rate=0.06)
        self.wait(3)
        self.stop_ambient_camera_rotation()

        # ============================================================
        # 10. 过渡到俯视棋盘
        # ============================================================
        hint.become(
            Tex("俯视 X-T 平面：驻波的正负区域", font_size=26, color=WHITE)
            .to_edge(DOWN, buff=0.35)
        )

        # 隐藏 3D 对象
        self.play(
            FadeOut(surface_standing),
            FadeOut(node_lines),
            FadeOut(node_text),
            run_time=1,
        )

        # ── 构造棋盘 ──
        # 波节线 x = 0, 0.5, 1.0, 1.5, 2.0
        # 零时间线 t = 0.25, 0.75, 1.25, 1.75 (cos(ωt)=0)
        x_edges = np.arange(0, 2.01, 0.5)
        t_edges = np.arange(0, 2.01, 0.25)

        cells = VGroup()
        for i in range(len(x_edges) - 1):
            for j in range(len(t_edges) - 1):
                xm = (x_edges[i] + x_edges[i + 1]) / 2
                tm = (t_edges[j] + t_edges[j + 1]) / 2
                sgn = np.sin(k * xm) * np.cos(omega * tm)

                p_bl = axes.c2p(x_edges[i],     t_edges[j],     0)
                p_br = axes.c2p(x_edges[i + 1], t_edges[j],     0)
                p_tr = axes.c2p(x_edges[i + 1], t_edges[j + 1], 0)
                p_tl = axes.c2p(x_edges[i],     t_edges[j + 1], 0)

                cell = Polygon(
                    p_bl, p_br, p_tr, p_tl,
                    fill_color=WHITE if sgn > 0 else DARK_GRAY,
                    fill_opacity=0.95,
                    stroke_width=0,
                )
                cells.add(cell)

        # 网格线 — 紫色竖线 = 波节, 灰色横线 = 零时刻
        grid_vertical = VGroup(*[
            ParametricFunction(
                lambda v, nx=nx: axes.c2p(nx, v, 0),
                t_range=[0, t_max],
                color=PURPLE,
                stroke_width=3,
            )
            for nx in x_edges
        ])
        grid_horizontal = VGroup(*[
            ParametricFunction(
                lambda u, nt=nt: axes.c2p(u, nt, 0),
                t_range=[0, x_max],
                color=GRAY,
                stroke_width=1.5,
            )
            for nt in t_edges
        ])

        # 图例 — 固定屏幕
        lbl_node = Tex("— 波节线 ($x=n\\lambda/2$)", font_size=20, color=PURPLE)
        lbl_zero = Tex("— 零时刻线 ($\\cos(\\omega t)=0$)", font_size=20, color=GRAY)
        lbl_white = Tex("$\\blacksquare$ $y>0$", font_size=20, color=WHITE)
        lbl_black = Tex("$\\blacksquare$ $y<0$", font_size=20, color=DARK_GRAY)
        board_legend = VGroup(lbl_node, lbl_zero, lbl_white, lbl_black).arrange(
            DOWN, aligned_edge=LEFT, buff=0.1
        )
        board_legend.to_corner(DL).shift(UP * 0.2)

        self.add_fixed_in_frame_mobjects(board_legend)

        self.play(
            FadeIn(cells),
            Create(grid_vertical),
            Create(grid_horizontal),
            Write(board_legend),
            run_time=2,
        )

        # ── 拉到正俯视 ──
        self.move_camera(
            phi=5 * DEGREES,
            theta=0 * DEGREES,
            zoom=1.1,
            run_time=3,
        )
        self.wait(3)


if __name__ == "__main__":
    import subprocess
    from pathlib import Path

    script_dir = Path(__file__).resolve().parent
    venv_bin = (
        Path.home() / "programme" / "Daily_Learning"
        / ".venv" / "bin" / "manim"
    )

    subprocess.run(
        [
            str(venv_bin),
            str(Path(__file__).resolve().name),
            "-ql",
            "--disable_caching",
        ],
        cwd=str(script_dir),
    )
