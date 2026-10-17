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

# ── Virtual environment ───────────────────────────────────────────────────────
# Create the venv and install Sphinx on first use, or when requirements change.
$(STAMP): $(DOCS)/requirements.txt
	$(PYTHON) -m venv $(VENV)
	$(VENV)/bin/pip install -q -r $<
	touch $@

.PHONY: venv
venv: $(STAMP)

# ── HTML ──────────────────────────────────────────────────────────────────────
# -W turns warnings into errors so broken references or diagrams fail the build.
.PHONY: html
html: $(STAMP)
	$(SPHINX) -b html -W --keep-going $(SOURCE) $(OUTPUT)/html
	@echo
	@echo "Open $(OUTPUT)/html/index.html"

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
