# Verification record

Repository state: frenzymath/PoincareConjecture at `62db01af9388dc1d9b56b66d6674f4c09b2318a2`
(Axel Delaval, 5 October 2026), copied byte for byte apart from `README.md`, which is renamed
`UPSTREAM_README.md`. Toolchain `leanprover/lean4:v4.33.1`; Mathlib `0df444a360eaa60ab8c11dca51a86af692955474`.

## 1. Static scan (10 October 2026)

`verification/scan.sh` reads every `.lean` file of `PoincareLib/`, `PoincareConjecture/`, the two
root modules and `Comparator/Solution.lean`: 23,559 files and 3,061,741 lines. Lines that open a
comment are skipped. Result: no `sorry`, no `admit`, no `axiom` declaration, no `native_decide`,
`ofReduceBool`, `implemented_by`, `@[extern]`, `unsafe` or `opaque` declaration, no
`set_option debug…` or `skipKernelTC`, no `run_cmd`, `run_elab`, `addDecl` or `modifyEnv`, no
`bv_decide`. The only `sorry`s in the repository are the two in `Comparator/Challenge.lean`,
which poses the theorems for the Comparator tool and is not part of the proof. One prose
comment mentions an "axiom" of combinatorial topology (Cairns 1940, Hudson 1969) and three mention
"opaque" in the sense of unfolding; none is a declaration.

## 2. Statement audit

`verification/compare_statements.py` checks that `ThreeSphere`, `SmoothPoincare` and
`TopologicalPoincare` are textually identical (comments removed, spaces normalised) in
`PoincareConjecture/Statement.lean`, which the proof uses, and in `Comparator/Challenge.lean`,
which poses the problem; that `Solution.lean` proves theorems with the same names and types as
the challenge; and that `Comparator/config.json` names those two theorems and permits only the
three standard axioms.

Reading of the statements:

- The manifold `M` is any type with a Hausdorff, second-countable topology and an atlas of
  charts into ℝ³ (`ChartedSpace (EuclideanSpace ℝ (Fin 3)) M`). Charts onto open subsets of all
  of ℝ³ allow no boundary.
- `IsManifold (𝓡 3) ∞ M` makes the atlas smooth; it is assumed only in the smooth statement.
- `CompactSpace M` and `SimplyConnectedSpace M`. In Mathlib `SimplyConnectedSpace X` means that
  `X` is nonempty, path-connected and has trivial fundamental groupoid, so connectedness is not
  missing.
- The conclusion is a homeomorphism, or a `C^∞` diffeomorphism, with the unit sphere of ℝ⁴ carrying
  Mathlib's standard smooth structure.
- Both statements are universe-polymorphic.

These are the classical topological and smooth Poincaré conjectures in dimension three.
`verification/Statements.lean` prints the definitions as elaborated, for reading next to this
list.

## 3. Lean build, axioms and kernel replay

The `lean` workflow builds `Solution` and `PoincareConjecture` in a chain of up to six stages,
because 3.06 million lines may not compile within one six-hour GitHub job. Each stage restores
the previous stage's compiled files from the Actions cache. Afterwards the `axioms` job checks
that nothing is left to build and that `#print axioms` reports only `propext`,
`Classical.choice` and `Quot.sound` for

- `PoincareConjecture.ComparatorTargets.smoothPoincareSkeleton`,
- `PoincareConjecture.ComparatorTargets.topologicalPoincareSkeleton`,
- `PoincareMT.smoothPoincareSkeleton`,
- `PoincareMT.topologicalPoincareSkeleton`,

and the `replay` job re-checks every declaration of `Solution` and everything it imports,
Mathlib included, with `leanchecker --fresh`.

Results: recorded in section 5 when the workflow finishes.

## 4. Independent checks of the objects (not of the Lean proof)

- C: the boundary of the 4-simplex has f-vector (5, 10, 10, 5), Euler characteristic 0 and
  integral homology Z, 0, 0, Z with no torsion (Smith normal form); H₁ of L(5,1) is Z/5.
- C++: Todd–Coxeter coset enumeration gives order 120 for ⟨s, t ∣ (st)² = s³ = t⁵⟩, the
  fundamental group of the Poincaré homology sphere, and 60 for the control A₅; its
  abelianization is trivial (relation matrix of determinant −1). A homology sphere need not be a
  sphere, which is why the theorem assumes π₁ = 1.
- Julia: under Ricci flow the round S³ of radius r₀ shrinks with r² = r₀² − 4t and becomes extinct
  at T = r₀²/4, with scalar curvature R = 3/(2(T − t)); the neck S² × ℝ pinches at r₀²/2. RK4
  agrees with the exact solutions to 10⁻⁸.
- Python with Regina 7.4.1: Regina's 3-sphere and the boundary of the 4-simplex are recognised
  as S³; the Poincaré homology sphere has H₁ = 0 and is not S³; L(2,1), L(3,1), L(5,2), L(7,3)
  have H₁ = Z/p and are not S³.

## 5. CI results

Pending.
