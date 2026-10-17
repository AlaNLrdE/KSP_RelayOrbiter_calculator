# ── Paths ─────────────────────────────────────────────────────────────────────
DOCS    := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
SOURCE  := $(DOCS)/source
OUTPUT  := $(DOCS)/_build
VENV    := $(DOCS)/.venv
PYTHON  ?= python3

SPHINX  := $(VENV)/bin/sphinx-build
STAMP   := $(VENV)/.installed

# ── Default target ────────────────────────────────────────────────────────────
.PHONY: all
all: html

# ── Dependency checks ─────────────────────────────────────────────────────────
# Python 3.10+ is required to create the venv. PlantUML (which brings its own
# Java runtime and Graphviz via Homebrew) renders the diagrams; it is installed
# automatically with Homebrew when available.
.PHONY: check-deps check-python check-plantuml
check-deps: check-python check-plantuml

check-python:
	@command -v $(PYTHON) >/dev/null 2>&1 || \
	    { echo "error: $(PYTHON) not found. Install Python 3.10+ or pass PYTHON=<path>."; exit 1; }
	@$(PYTHON) -c 'import sys; sys.exit(sys.version_info < (3, 10))' || \
	    { echo "error: Python 3.10+ required, found $$($(PYTHON) --version 2>&1)."; exit 1; }

check-plantuml:
	@if ! command -v plantuml >/dev/null 2>&1; then \
	    if command -v brew >/dev/null 2>&1; then \
	        echo "PlantUML not found -- installing with Homebrew..."; \
	        brew install plantuml || exit 1; \
	    else \
	        echo "error: plantuml not found on PATH. Install it first:"; \
	        echo "  macOS:          brew install plantuml"; \
	        echo "  Debian/Ubuntu:  sudo apt install plantuml"; \
	        echo "  Fedora:         sudo dnf install plantuml"; \
	        exit 1; \
	    fi; \
	fi
	@plantuml -version >/dev/null 2>&1 || \
	    { echo "error: plantuml is installed but fails to run (is a Java runtime available?)."; exit 1; }

# ── Virtual environment ───────────────────────────────────────────────────────
# Create the venv and install Sphinx on first use, or when requirements change.
$(STAMP): $(DOCS)/requirements.txt | check-python
	$(PYTHON) -m venv $(VENV)
	$(VENV)/bin/pip install -q --upgrade pip
	$(VENV)/bin/pip install -q -r $<
	touch $@

.PHONY: venv
venv: $(STAMP)

# ── HTML ──────────────────────────────────────────────────────────────────────
# -W turns warnings into errors so broken references or diagrams fail the build.
.PHONY: html
html: check-deps $(STAMP)
	$(SPHINX) -b html -W --keep-going $(SOURCE) $(OUTPUT)/html
	@echo
	@echo "Open $(OUTPUT)/html/index.html"

# ── README images ─────────────────────────────────────────────────────────────
# SVGs referenced by README.md, committed under docs/images/. Orbit figures are
# drawn to scale by a stdlib-only script; the rest are PlantUML diagrams
# rendered on a white background so they stay readable in GitHub dark mode.
IMAGES        := $(DOCS)/images
README_PUML   := architecture_overview body_hierarchy act_custom_altitude

.PHONY: readme-images
readme-images: check-deps
	$(PYTHON) -I $(DOCS)/tools/orbit_figures.py $(IMAGES)
	plantuml -config $(SOURCE)/diagrams/style.cfg -SbackgroundColor=white -tsvg \
	    -o $(IMAGES) $(foreach d,$(README_PUML),$(SOURCE)/diagrams/$(d).puml)

# ── Link check ────────────────────────────────────────────────────────────────
.PHONY: linkcheck
linkcheck: $(STAMP)
	$(SPHINX) -b linkcheck $(SOURCE) $(OUTPUT)/linkcheck

# ── Clean ─────────────────────────────────────────────────────────────────────
.PHONY: clean
clean:
	rm -rf $(OUTPUT)

.PHONY: distclean
distclean: clean
	rm -rf $(VENV)
