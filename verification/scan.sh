#!/usr/bin/env bash
# Static scan of the proof sources (PoincareLib/, PoincareConjecture/, the two root modules and
# Comparator/Solution.lean) for anything that could let a proof bypass the kernel or add
# assumptions: sorry, admit, new axioms, native_decide and Lean.ofReduceBool,
# implemented_by / extern, unsafe and opaque declarations, kernel debugging options and
# environment-modifying commands.  Lines that start a comment (-- or /-) are skipped; other
# matches inside block comments are reported and read by hand.  Comparator/Challenge.lean is
# not scanned: it states the two theorems with `sorry` on purpose, as the challenge for the
# Comparator tool.
# Usage: verification/scan.sh [repo-root]
set -u
root="${1:-$(dirname "$0")/..}"
list=$(mktemp)
{ find "$root/PoincareLib" "$root/PoincareConjecture" -name '*.lean'
  echo "$root/PoincareLib.lean"; echo "$root/PoincareConjecture.lean"; echo "$root/Comparator/Solution.lean"; } > "$list"
nfiles=$(wc -l < "$list")
nlines=$(xargs cat < "$list" | wc -l)
echo "scanning $nfiles Lean files, $nlines lines, under $root"
status=0
check() {
  local label="$1" pattern="$2" hits n=0
  hits=$(xargs grep -EHn "$pattern" < "$list" | grep -Ev ':[0-9]+:\s*(--|/-)' || true)
  [ -n "$hits" ] && n=$(echo "$hits" | wc -l)
  printf '%-34s %s\n' "$label" "$n"
  if [ "$n" -gt 0 ]; then echo "$hits" | head -5; status=1; fi
}
check "sorry"                     '(^|[^A-Za-z_.])sorry([^A-Za-z_]|$)'
check "admit (tactic)"            '(^|[;(]|by)\s*admit\s*($|[;)])'
check "axiom declarations"        '^\s*(@\[[^]]*\]\s*)?(private |public |protected |noncomputable )*axiom\s'
check "native_decide"             'native_decide'
check "Lean.ofReduceBool"         'ofReduceBool|ofReduceNat'
check "implemented_by / extern"   'implemented_by|@\[extern'
check "unsafe"                    '(^|\s)unsafe\s'
check "opaque"                    '^\s*(private |public |protected )*opaque\s'
check "debug / skipKernelTC"      'set_option\s+debug|skipKernelTC'
check "run_cmd / run_elab / addDecl" 'run_cmd|run_elab|run_meta|addDecl|modifyEnv'
check "bv_decide (trusts LRAT checker)" 'bv_decide'
n=$(xargs grep -Eoh 'decide\s*\+kernel' < "$list" | wc -l)
printf '%-34s %s (allowed: checked by the kernel)\n' "decide +kernel" "$n"
rm -f "$list"
if [ $status -eq 0 ]; then echo "scan: clean"; else echo "scan: FOUND ITEMS ABOVE"; fi
exit $status
