"""Read the output of `#print axioms` and fail unless every theorem uses only the three standard
axioms of Lean and Mathlib (propext, Classical.choice, Quot.sound).
Usage: python3 -I check_axioms_log.py LOG EXPECTED_COUNT
"""
import re
import sys

ALLOWED = {"propext", "Classical.choice", "Quot.sound"}
text = open(sys.argv[1]).read()
expected = int(sys.argv[2])
seen = 0
bad = []
for m in re.finditer(r"'([^']+)' (depends on axioms: \[([^\]]*)\]|does not depend on any axioms)", text):
    seen += 1
    axioms = {a.strip() for a in (m.group(3) or "").split(",") if a.strip()}
    extra = axioms - ALLOWED
    print(f"{m.group(1)}: {sorted(axioms) if axioms else 'no axioms'}" + (f"  EXTRA {sorted(extra)}" if extra else ""))
    if extra:
        bad.append(m.group(1))
if "error" in text.lower() or "sorry" in text.lower():
    print("the log contains an error or sorry")
    bad.append("log")
if seen != expected:
    print(f"expected {expected} axiom reports, found {seen}")
    bad.append("count")
print("axioms: OK" if not bad else f"axioms: FAILED {bad}")
sys.exit(1 if bad else 0)
