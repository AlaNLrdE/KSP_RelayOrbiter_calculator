#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

total=0

# Find matching paths (excluding .git internals), delete them, and report count.
# Uses -exec rm -rf {} + so rm handles missing paths gracefully.
remove() {
    local label="$1"; shift
    local count
    count=$(find . -not -path "./.git/*" "$@" -print 2>/dev/null | wc -l)
    if [ "$count" -gt 0 ]; then
        find . -not -path "./.git/*" "$@" -exec rm -rf {} + 2>/dev/null || true
        printf '  %-6s  %s\n' "$count" "$label"
        total=$(( total + count ))
    fi
}

echo "Cleaning $ROOT ..."
echo

# Python cache
remove "__pycache__ dirs"    -type d -name "__pycache__"
remove "compiled bytecode"   -type f \( -name "*.pyc" -o -name "*.pyo" -o -name "*.pyd" \)

# pytest
remove ".pytest_cache dirs"  -type d -name ".pytest_cache"
remove ".cache dirs"         -type d -name ".cache"

# Coverage
remove ".coverage files"     -type f \( -name ".coverage" -o -name ".coverage.*" \)
remove "htmlcov dirs"        -type d -name "htmlcov"

# Type checkers
remove ".mypy_cache dirs"    -type d -name ".mypy_cache"
remove ".pytype dirs"        -type d -name ".pytype"
remove ".pyre dirs"          -type d -name ".pyre"

# Build / packaging
remove "build dirs"          -type d -name "build"
remove "dist dirs"           -type d -name "dist"
remove "*.egg-info dirs"     -type d -name "*.egg-info"
remove "*.egg files"         -type f -name "*.egg"
remove ".eggs dirs"          -type d -name ".eggs"

# Virtual environments
remove "venv dirs"           -type d \( -name ".venv" -o -name "venv" -o -name "env" \)

# Claude
remove ".claude dirs"        -type d -name ".claude"

echo
if [ "$total" -eq 0 ]; then
    echo "Nothing to clean."
else
    echo "$total item(s) removed."
fi
