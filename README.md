# The Poincaré conjecture in Lean 4: the frenzymath formalization, built and checked

This repository holds a complete Lean 4 and Mathlib formalization of the smooth and topological
Poincaré theorems, copied unchanged from
[frenzymath/PoincareConjecture](https://github.com/frenzymath/PoincareConjecture) at commit
[`62db01a`](https://github.com/frenzymath/PoincareConjecture/tree/62db01af9388dc1d9b56b66d6674f4c09b2318a2)
(5 October 2026), together with an independent check of it. The formalization follows Morgan and
Tian's *Ricci Flow and the Poincaré Conjecture* (Hamilton–Perelman: Ricci flow with surgery and
finite-time extinction). It is the work of its authors, not of this repository:

> Zhiyuan Zhang, Axel Delaval, Leheng Chen, Jinxuan Chen, Jie Xu, Yuxuan Liao, Jiedong Jiang,
> Chunlei Liu and Bin Dong, *An AI-Assisted Formalization of the Poincaré Conjecture*,
> [arXiv:2610.08329](https://arxiv.org/abs/2610.08329) (6 October 2026).

The Lean files keep their copyright and licence notices (Apache-2.0, `LICENSES/Apache-2.0.txt`,
`NOTICE`, `MODIFICATIONS.md`); the upstream README is kept as `UPSTREAM_README.md`. The checks
added here (`verification/`, `checks/`, the workflows and this README) are under the MIT licence
in `LICENSE`. A second, independent formalization is described by Qin, Liao, Khaitan and Chow,
[arXiv:2609.33842](https://arxiv.org/abs/2609.33842); no public repository for it was found.

## What is proved

`PoincareConjecture/Statement.lean` imports only Mathlib and states:

```lean
abbrev ThreeSphere := Metric.sphere (0 : EuclideanSpace ℝ (Fin 4)) 1

def SmoothPoincare : Prop :=
  ∀ (M : Type u) [TopologicalSpace M] [T2Space M] [SecondCountableTopology M]
    [ChartedSpace (EuclideanSpace ℝ (Fin 3)) M] [IsManifold (𝓡 3) ∞ M]
    [CompactSpace M] [SimplyConnectedSpace M],
    Nonempty (Diffeomorph (𝓡 3) (𝓡 3) M ThreeSphere ∞)

def TopologicalPoincare : Prop :=
  ∀ (M : Type u) [TopologicalSpace M] [T2Space M] [SecondCountableTopology M]
    [ChartedSpace (EuclideanSpace ℝ (Fin 3)) M]
    [CompactSpace M] [SimplyConnectedSpace M], Nonempty (M ≃ₜ ThreeSphere)
```

So every compact, simply connected, Hausdorff, second-countable 3-manifold without boundary is
homeomorphic to the unit sphere in ℝ⁴, and diffeomorphic to it when it is smooth. Mathlib's
`SimplyConnectedSpace` includes nonempty and path-connected, and charts into all of ℝ³ (the model
`𝓡 3`) mean there is no boundary, so these are the classical statements. The theorems are
`PoincareConjecture.ComparatorTargets.smoothPoincareSkeleton` and `topologicalPoincareSkeleton` in
`Comparator/Solution.lean`, which apply `PoincareMT.smoothPoincareSkeleton` and
`PoincareMT.topologicalPoincareSkeleton` from `PoincareLib`.

## The checks

| Check | Result |
|---|---|
| Static scan (`verification/scan.sh`) of the 23,559 proof files, 3,061,741 lines | no `sorry`, `admit`, `axiom`, `native_decide`, `ofReduceBool`, `implemented_by`, `extern`, `unsafe`, `opaque`, kernel debug option or environment command |
| Statements (`verification/compare_statements.py`) | the three definitions are identical in `Statement.lean` and the Comparator challenge, `Solution.lean` proves exactly the two challenge statements, and the Comparator config allows only `propext`, `Classical.choice`, `Quot.sound` |
| Lean build of `Solution` and `PoincareConjecture` (Lean v4.33.1, Mathlib `0df444a`) | in CI (`lean` workflow, chained stages); see `VERIFICATION.md` |
| `#print axioms` of the four endpoint theorems (`verification/Axioms.lean`) | in CI, after the build |
| Kernel replay with `leanchecker --fresh Solution` | in CI, after the build |
| C (`checks/c`): homology of the boundary of the 4-simplex by Smith normal form | Z, 0, 0, Z, no torsion; χ = 0; H₁(L(5,1)) = Z/5 |
| C++ (`checks/cpp`): Todd–Coxeter for ⟨s,t ∣ (st)² = s³ = t⁵⟩ | order 120 with trivial abelianization: the Poincaré homology sphere is not simply connected |
| Julia (`checks/julia`): Ricci flow on the round S³ and the neck S²×ℝ | extinction at r₀²/4 and pinching at r₀²/2; R(T − t) = 3/2 |
| Python (`checks/python`, Regina 7.4.1) | S³ and ∂Δ⁴ recognised as S³; the Poincaré homology sphere and the lens spaces are not |

`checks/run_all.sh` runs everything except the Lean build and passed locally on 10 October 2026.
The C, C++, Julia and Python checks test the objects the theorem is about, not the Lean proof.

## Rebuilding

```sh
lake exe cache get
lake build Solution PoincareConjecture
lake env lean verification/Axioms.lean
checks/run_all.sh
```

The Comparator replay described in `UPSTREAM_README.md` needs `landrun`, `lean4export` and
`nanoda` and was not run here.
