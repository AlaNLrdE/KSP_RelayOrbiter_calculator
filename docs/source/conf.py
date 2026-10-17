# Sphinx configuration for the KSP Relay Constellation Calculator docs.
from pathlib import Path

DOCS_SOURCE = Path(__file__).parent.resolve()

# ── Project ───────────────────────────────────────────────────────────────────
project   = "KSP Relay Constellation Calculator"
author    = "Alan Ocegueda"
copyright = "2026, Alan Ocegueda"

# ── General ───────────────────────────────────────────────────────────────────
extensions = [
    "sphinx.ext.mathjax",
    "sphinxcontrib.plantuml",
]

primary_domain     = "cpp"
highlight_language = "none"  # plain `::` blocks are program output; code blocks name their language
exclude_patterns   = ["_build", ".venv"]

# Every :cpp:any: / :cpp:class: reference must resolve.
nitpicky = True
nitpick_ignore = [
    ("cpp:identifier", "std"),
    ("cpp:identifier", "std::string"),
    ("cpp:identifier", "std::string_view"),
    ("cpp:identifier", "std::optional"),
    ("cpp:identifier", "std::vector"),
    ("cpp:identifier", "std::shared_ptr"),
    ("cpp:identifier", "size_t"),
]

# ── PlantUML ──────────────────────────────────────────────────────────────────
# Requires the `plantuml` command on PATH (e.g. `brew install plantuml`).
# The shared style is applied to every diagram through -config.
plantuml = f"plantuml -config {DOCS_SOURCE / 'diagrams' / 'style.cfg'}"
plantuml_output_format = "svg_img"
plantuml_latex_output_format = "pdf"

# ── HTML ──────────────────────────────────────────────────────────────────────
html_theme = "sphinx_rtd_theme"
html_title = "KSP Relay Constellation Calculator"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
html_theme_options = {
    "navigation_depth": 3,
    "collapse_navigation": False,
}
