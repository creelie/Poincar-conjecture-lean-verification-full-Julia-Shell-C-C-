The Lean 4 formalization of the smooth and topological Poincaré theorems by Zhang, Delaval,
Chen, Chen, Xu, Liao, Jiang, Liu and Dong (frenzymath/PoincareConjecture at 62db01a,
arXiv:2610.08329), unchanged, with an independent check:

- static scan of 23,559 proof files (3,061,741 lines): no sorry, axiom, native_decide or other
  escape hatch;
- statement audit: the proved statements are exactly the Comparator challenge;
- CI: chained Lean build, `#print axioms` of the four endpoint theorems, `leanchecker --fresh`;
- C, C++, Julia and Python (Regina) checks of S³, the Poincaré homology sphere and Ricci flow.

See VERIFICATION.md for the results. The formalization is its authors' work.
