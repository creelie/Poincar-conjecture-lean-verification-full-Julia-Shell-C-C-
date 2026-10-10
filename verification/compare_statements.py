"""Check that the statements proved are the statements posed.

Comparator/Challenge.lean poses the two theorems (with `sorry`); PoincareConjecture/Statement.lean
holds the definitions the proof uses; Comparator/Solution.lean proves the two theorems.  This
script checks, after removing comments and normalising whitespace, that
  1. the definitions ThreeSphere, SmoothPoincare and TopologicalPoincare are identical in
     Challenge.lean and Statement.lean, and
  2. Solution.lean proves theorems with exactly the names and types that Challenge.lean states.
Usage: python3 -I verification/compare_statements.py [repo-root]
"""
import re
import sys
from pathlib import Path

root = Path(sys.argv[1] if len(sys.argv) > 1 else Path(__file__).resolve().parent.parent)


def clean(text):
    text = re.sub(r"/-.*?-/", " ", text, flags=re.S)
    text = re.sub(r"--[^\n]*", " ", text)
    return text


def norm(s):
    return " ".join(s.split())


def decls(text, kind):
    """Map name -> normalised text of each `kind` declaration (up to the next declaration)."""
    out = {}
    pat = re.compile(rf"^(?:abbrev|def|theorem|end|namespace)\b", re.M)
    starts = [m.start() for m in pat.finditer(text)] + [len(text)]
    for a, b in zip(starts, starts[1:]):
        chunk = text[a:b]
        m = re.match(rf"({kind})\s+([\w.']+)", chunk)
        if m:
            out[m.group(2)] = norm(chunk)
    return out


challenge = clean((root / "Comparator/Challenge.lean").read_text())
statement = clean((root / "PoincareConjecture/Statement.lean").read_text())
solution = clean((root / "Comparator/Solution.lean").read_text())

fails = 0
cd = decls(challenge, "abbrev|def")
sd = decls(statement, "abbrev|def")
for name in ("ThreeSphere", "SmoothPoincare", "TopologicalPoincare"):
    same = name in cd and cd.get(name) == sd.get(name)
    print(("same " if same else "DIFF ") + name)
    fails += not same

ct = decls(challenge, "theorem")
st = decls(solution, "theorem")
for name in ("smoothPoincareSkeleton", "topologicalPoincareSkeleton"):
    c = ct.get(name, "").split(":=")[0]
    s = st.get(name, "").split(":=")[0]
    same = bool(c) and c == s
    print(("same " if same else "DIFF ") + f"statement of {name}: {c}")
    fails += not same
    if "sorry" not in ct.get(name, ""):
        print(f"  (the challenge proof of {name} is expected to be `sorry`)")
        fails += 1
    if "sorry" in st.get(name, ""):
        print(f"  the solution proof of {name} uses sorry")
        fails += 1

# the Comparator configuration names exactly these two theorems and the three standard axioms
import json
cfg = json.loads((root / "Comparator/config.json").read_text())
ok = (sorted(cfg["theorem_names"]) == sorted(
        ["PoincareConjecture.ComparatorTargets.smoothPoincareSkeleton",
         "PoincareConjecture.ComparatorTargets.topologicalPoincareSkeleton"])
      and sorted(cfg["permitted_axioms"]) == sorted(["propext", "Classical.choice", "Quot.sound"]))
print(("same " if ok else "DIFF ") + "Comparator config: theorem names and permitted axioms")
fails += not ok
print("statements: OK" if fails == 0 else f"statements: {fails} FAILED")
sys.exit(1 if fails else 0)
