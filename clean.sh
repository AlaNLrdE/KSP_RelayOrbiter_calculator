#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

total=0

# Find matching paths (excluding .git internals and the thirdparty submodule),
# delete them, and report count.
# Uses -exec rm -rf {} + so rm handles missing paths gracefully.
remove() {
    local label="$1"; shift
    local count
    count=$(find . -not -path "./.git/*" -not -path "./thirdparty/*" -not -path "./docs/.venv/*" "$@" -print 2>/dev/null | wc -l)
    if [ "$count" -gt 0 ]; then
        find . -not -path "./.git/*" -not -path "./thirdparty/*" -not -path "./docs/.venv/*" "$@" -exec rm -rf {} + 2>/dev/null || true
        printf '  %-6s  %s\n' "$count" "$label"
        total=$(( total + count ))
    fi
}

echo "Cleaning $ROOT ..."
echo

# Build outputs
remove "build/obj dirs"      -type d -path "./build/obj"
remove "object files"        -type f -name "*.o"
remove "*.dSYM dirs"         -type d -name "*.dSYM"

# Documentation
remove "docs/_build dirs"    -type d -path "./docs/_build"

# Claude
remove ".claude dirs"        -type d -name ".claude"

echo
if [ "$total" -eq 0 ]; then
    echo "Nothing to clean."
else
    echo "$total item(s) removed."
fi
