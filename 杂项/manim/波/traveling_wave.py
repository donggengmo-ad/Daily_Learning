"""
行波曲面降维演示 — 从 y(x,t) 曲面到 y(φ) 简谐曲线
==================================================
y(x,t) = A·sin(kx + ωt)         左行波曲面

c2p(x, t, amplitude) → X=位置(右) Y=时间(上) Z=振幅(深)
  X-Y 构成"面对镜头"的平面，Z 为纵深

流程：
 1. XOY 平面上画出简谐曲线 y=sin(kx)  (t=0)
 2. 沿 t 轴扩展 + 沿 x 轴平移 → y(x,t) 曲面
 3. 等相位线 kx+ωt=C（X-Y 平面上的斜线）
 4. 相位线（过原点 ⊥ 等相位线，即 ∇φ 方向）
 5. 镜头转到与等相位线平行 → 曲面视觉降维
 6. 镜头垂直 Y-φ 平面 → 显现简谐曲线 y(φ)
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


class TravelingWaveDemo(ThreeDScene):
    """行波曲面 → 降维 → 简谐曲线"""

    def construct(self):
        # ============================================================
        # 1. 参数
        # ============================================================
        A = 1.0
        k = 2 * PI
        omega = 2 * PI
        L = 2.0                # 空间范围
        T = 2.0                # 时间范围
        SURF_RES = 24

        # ============================================================
        # 2. 初始相机 — 斜上方视角
        # ============================================================
        self.set_camera_orientation(
            phi=60 * DEGREES,
            theta=-50 * DEGREES,
            zoom=0.7,
        )

        # ============================================================
        # 3. 三维坐标轴
        #    c2p(x, t, amplitude) → X=位置(右) Y=时间(上) Z=振幅(深)
        #    X-Y 平面 = x-t 平面，Z = 振幅
        # ============================================================
        axes = ThreeDAxes(
            x_range=[0, L, 0.5],
            y_range=[0, T, 0.5],
            z_range=[-1.5, 1.5, 0.5],
            x_length=6,
            y_length=5,
            z_length=4,
            axis_config={"include_numbers": True, "font_size": 14},
        )

        x_lbl = Tex("$x$ 位置", font_size=28)
        y_lbl = Tex("$t$ 时间", font_size=28)
        z_lbl = Tex("$y$ 振幅", font_size=28)

        self.play(Create(axes), run_time=1.2)
        self.play(
            Write(x_lbl.next_to(axes.c2p(2.3, 0, 0), RIGHT)),
            Write(y_lbl.next_to(axes.c2p(0, 2.3, 0), OUT)),
            Write(z_lbl.next_to(axes.c2p(0, 0, 1.7), UP)),
            run_time=0.8,
        )
        self.wait(0.3)

        # ============================================================
        # 4. 阶段 1：XOY 平面上画简谐曲线 y=A·sin(kx) at t=0
        # ============================================================
        title_1 = Tex(
            "Step 1: XOY 平面上的简谐曲线", font_size=30, color=WHITE
        ).to_corner(UL)
        formula_1 = MathTex(
            r"y = A\sin(kx) \qquad (t=0)",
            font_size=28, color=YELLOW,
        ).to_corner(UR).shift(DOWN * 0.3)

        self.add_fixed_in_frame_mobjects(title_1, formula_1)

        # 曲线在 t=0 → c2p(x, 0, amp)
        curve_2d = ParametricFunction(
            lambda u: axes.c2p(u, 0, A * np.sin(k * u)),
            t_range=[0, L],
            color=YELLOW,
            stroke_width=4,
        )
        origin_dot = Dot3D(axes.c2p(0, 0, 0), radius=0.08, color=WHITE)

        self.play(
            Write(title_1),
            Write(formula_1),
            Create(curve_2d),
            FadeIn(origin_dot),
            run_time=2,
        )
        self.wait(1)

        # ============================================================
        # 5. 阶段 2：沿 t 扩展 + 沿 x 移动 → 行波曲面
        # ============================================================
        title_2 = Tex(
            "Step 2: 沿 $t$ 扩展 + 沿 $x$ 平移 → 行波曲面",
            font_size=28, color=WHITE,
        ).to_corner(UL)
        formula_2 = MathTex(
            r"y(x,t) = A\sin(kx + \omega t)",
            font_size=28, color=YELLOW,
        ).to_corner(UR).shift(DOWN * 0.3)

        self.play(
            ReplacementTransform(title_1, title_2),
            ReplacementTransform(formula_1, formula_2),
            run_time=1,
        )

        # 逐层画出时间切片 → 展示曲面构建
        N_SLICES = 16
        t_values = np.linspace(0, T, N_SLICES)
        slice_curves = VGroup()
        for tv in t_values:
            sc = ParametricFunction(
                lambda u, t_val=tv: axes.c2p(
                    u, t_val, A * np.sin(k * u + omega * t_val)
                ),
                t_range=[0, L],
                color=interpolate_color(YELLOW, GOLD, tv / T),
                stroke_width=1.8,
                stroke_opacity=0.55,
            )
            slice_curves.add(sc)

        # 实心曲面
        wave_surface = Surface(
            lambda u, v: axes.c2p(
                u, v, A * np.sin(k * u + omega * v)
            ),
            u_range=[0, L],
            v_range=[0, T],
            resolution=(SURF_RES, SURF_RES),
            fill_color=GOLD,
            fill_opacity=0.4,
            stroke_width=0,
        )

        self.play(
            LaggedStart(
                *[Create(c, run_time=0.12) for c in slice_curves],
                lag_ratio=0.7,
            ),
            FadeIn(wave_surface),
            FadeOut(curve_2d),
            run_time=4.5,
        )
        self.wait(0.3)
        self.play(FadeOut(slice_curves), wave_surface.animate.set_fill(opacity=0.55))
        self.wait(0.5)

        # ============================================================
        # 6. 阶段 3：等相位线 kx+ωt=C
        #     k=ω → x+t = C/(2π) → 方向 (−1, 1) in (x,t)=XY平面
        # ============================================================
        title_3 = Tex(
            "Step 3: 等相位线 $kx+\\omega t = C$", font_size=28, color=WHITE
        ).to_corner(UL)

        self.play(ReplacementTransform(title_2, title_3), run_time=0.8)

        C_vals = np.arange(-2 * PI, 3 * PI, PI)  # -2π, -π, 0, π, 2π
        c_colors = [BLUE_C, BLUE_D, BLUE_E, TEAL_D, TEAL_E]
        phase_line_group = VGroup()

        for i, C in enumerate(C_vals):
            shift = C / omega  # = C/(2π)

            line = ParametricFunction(
                lambda u, sv=shift: axes.c2p(u, sv - u, 0),
                t_range=[max(0, shift - T), min(L, shift)],
                color=c_colors[i % len(c_colors)],
                stroke_width=2.5,
            )
            phase_line_group.add(line)

            # 标签
            if C in [-2 * PI, 0, 2 * PI]:
                mx = max(0.3, min(L - 0.3, (shift + L) / 2 if shift > 0 else shift / 2))
                mt = shift - mx
                if -0.1 <= mt <= T + 0.1:
                    lbl = MathTex(
                        f"C{'+' if C>=0 else ''}{int(C/PI)}\\pi"
                        if C else "C=0",
                        font_size=20, color=c_colors[i % len(c_colors)],
                    )
                    lbl.next_to(
                        axes.c2p(mx, max(0.05, min(T - 0.05, mt)), 0),
                        UP, buff=0.3,
                    )
                    self.add_fixed_in_frame_mobjects(lbl)
                    phase_line_group.add(lbl)

        self.play(Create(phase_line_group), run_time=3)
        self.wait(0.8)

        # ============================================================
        # 7. 阶段 4：相位线 — ⊥ 等相位线，过原点，方向 (1,1)
        #     ∇φ = (k, ω) in (x,t) → 方向 (1, 1) in XY平面
        # ============================================================
        title_4 = Tex(
            "Step 4: 相位线（$\\perp$ 等相位线，$\\nabla\\phi$ 方向）",
            font_size=26, color=WHITE,
        ).to_corner(UL)

        self.play(ReplacementTransform(title_3, title_4), run_time=0.8)

        # x = t 过原点
        phase_dir = ParametricFunction(
            lambda u: axes.c2p(u, u, 0),
            t_range=[0, L],
            color=PURE_RED,
            stroke_width=5,
        )
        phase_lbl = Tex("$\\nabla\\phi$ 相位方向", font_size=22, color=PURE_RED)
        phase_lbl.next_to(axes.c2p(0.8, 0.8, 0), UP, buff=0.3)
        self.add_fixed_in_frame_mobjects(phase_lbl)

        origin_marker = Dot3D(axes.c2p(0, 0, 0), radius=0.12, color=WHITE)

        self.play(
            Create(phase_dir),
            FadeIn(phase_lbl),
            FadeIn(origin_marker),
            run_time=1.5,
        )
        self.wait(0.8)

        # ============================================================
        # 8. 阶段 5：镜头转平行等相位线 → 视觉降维
        #     等相位线方向: (−1, 1) in XY, theta≈atan2(1,−1)=135°
        #     相机沿此方向看: theta=135°, phi≈75°(略俯视)
        # ============================================================
        title_5 = Tex(
            "Step 5: 视线 // 等相位线 → 曲面收缩到相位线上",
            font_size=24, color=WHITE,
        ).to_corner(UL)

        self.play(ReplacementTransform(title_4, title_5), run_time=0.8)

        # 相机转到等相位线方向
        self.move_camera(
            phi=75 * DEGREES,
            theta=135 * DEGREES,
            zoom=0.95,
            run_time=3,
        )
        self.wait(1)

        # 曲面收缩 — 沿垂直于相位线的方向压扁
        self.play(
            wave_surface.animate.stretch(0.08, dim=0),
            run_time=2,
            rate_func=smooth,
        )
        self.play(
            wave_surface.animate.stretch(0.05, dim=1),
            run_time=1.5,
            rate_func=smooth,
        )
        self.wait(0.5)

        # ============================================================
        # 9. 阶段 6：镜头垂直 Y-φ 平面 → y(φ) 简谐曲线
        #     Y-φ 平面的法向量 = 等相位线方向
        #     phi=90° 让相机在 XY 平面（视线 ⊥ Z=振幅方向）
        #     此时屏幕横轴=φ方向, 纵轴=振幅
        # ============================================================
        title_6 = Tex(
            "Step 6: 视线 $\\perp$ $Y$-$\\phi$ 平面 → $y(\\phi)$ 简谐曲线",
            font_size=24, color=WHITE,
        ).to_corner(UL)

        self.play(ReplacementTransform(title_5, title_6), run_time=0.8)

        # 镜头放平 — 垂直 Y-φ 平面
        self.move_camera(
            phi=90 * DEGREES,
            theta=135 * DEGREES,
            zoom=1.1,
            run_time=3,
        )

        # 清理辅助元素
        self.play(
            FadeOut(phase_line_group),
            FadeOut(phase_dir),
            FadeOut(phase_lbl),
            run_time=1,
        )
        self.wait(0.5)

        # 替换曲面为降维曲线
        final_title = Tex(
            "$y(\\phi) = A\\sin(\\phi)$ — 降维后的简谐曲线",
            font_size=28, color=YELLOW,
        ).to_corner(UL)
        final_formula = MathTex(
            r"\phi = kx + \omega t",
            font_size=24, color=GRAY,
        ).next_to(final_title, DOWN, aligned_edge=LEFT)

        self.add_fixed_in_frame_mobjects(final_title, final_formula)

        self.play(
            ReplacementTransform(title_6, final_title),
            Write(final_formula),
            run_time=1.5,
        )

        # 沿相位线方向 (x=t) 的简谐曲线
        # φ = kx + ωt, 沿 x=t: φ = 2k·s = 4π·s
        collapse_curve = ParametricFunction(
            lambda u: axes.c2p(
                u, u,
                A * np.sin(k * u + omega * u),  # = A·sin(4π·u)
            ),
            t_range=[0, 1],
            color=YELLOW,
            stroke_width=5,
        )

        self.play(
            FadeOut(wave_surface),
            Create(collapse_curve),
            run_time=2,
        )

        # 微旋转展示
        self.move_camera(
            phi=85 * DEGREES,
            theta=130 * DEGREES,
            zoom=1.3,
            run_time=2,
        )
        self.wait(2)


if __name__ == "__main__":
    import subprocess
    from pathlib import Path

    script_dir = Path(__file__).resolve().parent
    venv_bin = (
        Path.home() / "programme" / "Daily_Learning"
        / ".venv" / "bin" / "manim"
    )

    subprocess.run(
        [str(venv_bin), str(Path(__file__).resolve().name), "-ql"],
        cwd=str(script_dir),
    )
