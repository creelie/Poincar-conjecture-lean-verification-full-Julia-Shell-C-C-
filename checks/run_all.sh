#!/usr/bin/env bash
# Every check that does not need the Lean build: the static scan of the proof sources, the
# statement comparison, and the independent C, C++, Julia and Python checks.
# Usage: checks/run_all.sh   (JULIA=/path/to/julia to choose the Julia binary)
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
build="$(mktemp -d)"
trap 'rm -rf "$build"' EXIT
fails=0
run() {
  local name="$1"; shift
  echo "== $name"
  if "$@"; then echo "-- $name: passed"; else echo "-- $name: FAILED"; fails=$((fails + 1)); fi
}
run "static scan of the Lean sources" "$root/verification/scan.sh" "$root"
run "statement comparison" python3 -I "$root/verification/compare_statements.py" "$root"
gcc -O2 -Wall -Werror -o "$build/sphere_homology" "$root/checks/c/sphere_homology.c"
run "C: homology of the boundary of the 4-simplex" "$build/sphere_homology"
g++ -O2 -std=c++17 -Wall -Werror -o "$build/binary_icosahedral" "$root/checks/cpp/binary_icosahedral.cpp"
run "C++: binary icosahedral group" "$build/binary_icosahedral"
run "Julia: Ricci flow on S^3 and the neck" "${JULIA:-julia}" "$root/checks/julia/ricci_sphere.jl"
run "Python: Regina recognition" python3 -I "$root/checks/python/regina_checks.py"
if [ "$fails" -eq 0 ]; then echo "ALL CHECKS PASSED"; else echo "$fails CHECK(S) FAILED"; exit 1; fi
