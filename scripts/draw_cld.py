"""Causal loop diagram of the coupled AgriPoliS x ALMaSS-emulator ABPM (docs/figures/cld_emulator.png)."""
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch, FancyBboxPatch, Rectangle

OUT = Path(__file__).resolve().parents[1] / "docs" / "figures"

N = {  # key: (x, y, label, kind)
    "price":  (1.6, 9.2, "Maize price\n(scenario S4)", "exo"),
    "pay":    (5.4, 9.2, "Policy payments\n(PFES, Decree 58/2024)", "exo"),
    "wage":   (9.2, 9.2, "Wage and off-farm\njob availability", "exo"),
    "gap":    (1.6, 6.9, "Profit gap:\nintensive vs low-input maize", "agp"),
    "stew":   (6.4, 7.0, "Net benefit of\nstewardship activities", "agp"),
    "int":    (3.6, 4.6, "Share of intensive\nland use (MAIZE_INT)", "key"),
    "value":  (8.6, 5.0, "Conservation value\nin the MIP: a × 150 €/ha", "agp"),
    "aw":     (12.6, 5.0, "Land-manager\nawareness a", "key"),
    "assets": (13.8, 9.2, "Assets and\nland-use certificate", "exo"),
    "perenn": (13.8, 7.2, "Scope to switch to perennials\n(orchard, native timber; yearly limits)", "agp"),
    "ext":    (16.4, 6.4, "Extension\ncampaign (S5)", "exo"),
    "nb":     (16.4, 4.4, "Neighbours'\nawareness", "agp"),
    "decay":  (10.6, 3.0, "Decay\n(towards survey a₀)", "agp"),
    "dist":   (3.6, 1.9, "Management disturbance\n(slashing, burning, herbicide)", "eco"),
    "veq":    (7.6, 0.5, "Equilibrium richness V*\n(emulator = f(shares))", "eco"),
    "vobs":   (11.6, 0.5, "Observed richness V", "eco"),
    "loss":   (15.8, 1.6, "Perceived loss\nL = (V₂₀₂₅ − V)/V₂₀₂₅", "eco"),
}
STYLE = {"exo": ("#f2f2f2", "#888888"), "agp": ("#e3eefc", "#3b6fb6"),
         "key": ("#cfe0fa", "#1f4e99"), "eco": ("#e2f4e4", "#2e8b57")}

# (from, to, polarity, curvature, delay label)
E = [
    ("price", "gap", "+", 0.0, None), ("gap", "int", "+", 0.15, None),
    ("pay", "stew", "+", 0.0, None), ("wage", "stew", "−", 0.0, None),
    ("stew", "int", "−", 0.1, None), ("value", "int", "−", -0.15, "1-year lag"),
    ("aw", "value", "+", 0.0, None), ("assets", "perenn", "+", 0.0, None),
    ("perenn", "stew", "+", 0.12, None), ("ext", "aw", "+", 0.0, None),
    ("nb", "aw", "+", 0.15, None), ("aw", "nb", "+", 0.15, None),
    ("aw", "decay", "+", 0.3, None), ("decay", "aw", "−", 0.3, None),
    ("int", "dist", "+", 0.0, None), ("dist", "veq", "−", 0.1, None),
    ("veq", "vobs", "+", 0.0, "τ ≈ 5 to 6 years"), ("vobs", "loss", "−", 0.1, None),
    ("loss", "aw", "+", 0.25, "feedback F = 3\n(cut in one-way)"),
]


LABEL_AT = {("value", "int"): 0.3, ("loss", "aw"): 0.6, ("stew", "int"): 0.4, ("vobs", "loss"): 0.55}


def box(ax, key):
    x, y, text, kind = N[key]
    face, edge = STYLE[kind]
    ax.text(x, y, text, ha="center", va="center", fontsize=9.5, zorder=3,
            bbox=dict(boxstyle="round,pad=0.45", fc=face, ec=edge, lw=1.6 if kind == "key" else 1.1))


def main():
    fig, ax = plt.subplots(figsize=(16, 10.5))
    ax.set_xlim(-0.6, 18.2)
    ax.set_ylim(-2.2, 10.3)
    ax.axis("off")
    ax.add_patch(Rectangle((-0.4, 3.4), 18.4, 6.8, fc="#f6f9ff", ec="none", zorder=0))
    ax.add_patch(Rectangle((-0.4, -0.9), 18.4, 4.2, fc="#f3fbf4", ec="none", zorder=0))
    ax.text(-0.3, 10.0, "AgriPoliS + SesExtension (agents, yearly MIP)", fontsize=11, color="#1f4e99", weight="bold")
    ax.text(-0.3, -0.7, "EcoEmulator (stands in for ALMaSS at run time): ecology", fontsize=11, color="#2e8b57", weight="bold")
    for key in N:
        box(ax, key)
    for a, b, pol, rad, delay in E:
        (x1, y1, *_), (x2, y2, *_) = N[a], N[b]
        arrow = FancyArrowPatch((x1, y1), (x2, y2), connectionstyle=f"arc3,rad={rad}", arrowstyle="-|>",
                                mutation_scale=16, lw=1.8 if a in ("loss", "veq", "value") else 1.3,
                                color="#333333", shrinkA=34, shrinkB=34, zorder=2)
        ax.add_patch(arrow)
        t = LABEL_AT.get((a, b), 0.5)
        mx, my = x1 + t * (x2 - x1), y1 + t * (y2 - y1)
        dx, dy = x2 - x1, y2 - y1
        ox, oy = -dy * rad * 0.5, dx * rad * 0.5
        ax.text(mx + ox + 0.15, my + oy + 0.15, pol, fontsize=15, weight="bold",
                color="#c0392b" if pol == "−" else "#1e7b34", zorder=4)
        if delay:
            shift = {"loss": (0.7, -0.45), "value": (-0.6, -0.75)}.get(a, (0.0, -0.45))
            ax.text(mx + ox + shift[0], my + oy + shift[1], "‖  " + delay, fontsize=8.5, color="#6b3fa0", ha="center", zorder=4)
    ax.text(8.6, 2.7, "B1", fontsize=26, weight="bold", color="#6b3fa0", ha="center")
    ax.text(8.6, 2.2, "main balancing loop:\nintensification → diversity loss → awareness ↑ → less intensification",
            fontsize=8.5, color="#6b3fa0", ha="center", va="top")
    ax.text(10.4, 4.3, "B2", fontsize=16, weight="bold", color="#6b3fa0")
    ax.add_patch(FancyBboxPatch((-0.3, -1.85), 7.2, 0.8, boxstyle="round,pad=0.1", fc="white", ec="#2e8b57", ls="--"))
    ax.text(3.3, -1.45, "Offline: 150 ALMaSS landscapes (HB_PFG) → emulator coefficients + τ", fontsize=9, ha="center", color="#2e8b57")
    ax.add_patch(FancyArrowPatch((5.5, -1.1), (7.0, 0.1), arrowstyle="-|>", ls="--", color="#2e8b57", mutation_scale=12))
    ax.text(9.2, -1.45, "+ / −: same / opposite direction     ‖: delay     "
            "one-way: no L → a arrow     no-lag: τ ≈ 0", fontsize=9, color="#444444")
    fig.suptitle("Causal loop diagram (CLD) of the Hòa Bình ABPM: AgriPoliS coupled to the ALMaSS emulator",
                 fontsize=14, weight="bold", y=0.93)
    OUT.mkdir(parents=True, exist_ok=True)
    fig.savefig(OUT / "cld_emulator.png", dpi=160, bbox_inches="tight")
    fig.savefig(OUT / "cld_emulator.svg", bbox_inches="tight", metadata={"Date": None})


if __name__ == "__main__":
    main()
