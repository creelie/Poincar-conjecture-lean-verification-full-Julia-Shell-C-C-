"""Sanity checks with Regina (regina-normal.github.io) of the objects in the Poincare conjecture.

- Regina's triangulated 3-sphere and the boundary of the 4-simplex are recognised as S^3.
- The Poincare homology sphere has H1 = 0 but is not S^3: its fundamental group is not
  trivial. So the hypothesis pi_1 = 1 cannot be weakened to H_1 = 0.
- Lens spaces L(p, q) with p > 1 have H1 = Z/p, so they are not simply connected.
"""
import sys

import regina

fails = 0


def check(name, cond):
    global fails
    print(("ok   " if cond else "FAIL ") + name)
    if not cond:
        fails += 1


s3 = regina.Example3.threeSphere()
check("Example3.threeSphere() is recognised as S^3", s3.isSphere())

# boundary of the 4-simplex, built facet by facet
four = regina.Triangulation4()
four.newPentachoron()
bdry = four.boundaryComponent(0).build()
check("boundary of the 4-simplex has 5 tetrahedra", bdry.size() == 5)
check("boundary of the 4-simplex is recognised as S^3", bdry.isSphere())

phs = regina.Example3.poincare()
h1 = phs.homology()
check("Poincare homology sphere: H1 = 0", h1.isTrivial())
check("Poincare homology sphere is closed and orientable", phs.isClosed() and phs.isOrientable())
check("Poincare homology sphere is NOT recognised as S^3", not phs.isSphere())
g = phs.group()
g.simplify()
check("its fundamental group has a nontrivial presentation", g.countGenerators() > 0)

for p, q in ((2, 1), (3, 1), (5, 2), (7, 3)):
    L = regina.Example3.lens(p, q)
    h = L.homology()
    check(f"L({p},{q}): H1 = Z/{p}", h.rank() == 0 and h.countInvariantFactors() == 1
          and int(str(h.invariantFactor(0))) == p)
    check(f"L({p},{q}) is not S^3", not L.isSphere())

print("OK" if fails == 0 else "FAILED")
sys.exit(1 if fails else 0)
