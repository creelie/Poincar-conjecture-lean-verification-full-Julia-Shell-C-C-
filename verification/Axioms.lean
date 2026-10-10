import Solution

/-! The axioms behind the two endpoint theorems, both in the form the Comparator challenge
states them (`PoincareConjecture.ComparatorTargets`) and in the form `PoincareLib` proves them
(`PoincareMT`).  Each line must report only `propext`, `Classical.choice` and `Quot.sound`;
`sorryAx` would reveal a `sorry` and `Lean.ofReduceBool` a `native_decide`. -/

#print axioms PoincareConjecture.ComparatorTargets.smoothPoincareSkeleton
#print axioms PoincareConjecture.ComparatorTargets.topologicalPoincareSkeleton
#print axioms PoincareMT.smoothPoincareSkeleton
#print axioms PoincareMT.topologicalPoincareSkeleton
