// The Poincare homology sphere: its fundamental group, the binary icosahedral group
// <s, t | (st)^2 = s^3 = t^5>, has order 120 (Todd-Coxeter coset enumeration over the
// trivial subgroup) and trivial abelianization (H1 = 0). So a homology 3-sphere need not be
// simply connected, which is why the Poincare conjecture assumes pi_1 = 1, not H_1 = 0.
// Control: <s, t | s^2, t^3, (st)^5> is the icosahedral group A5 of order 60.
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

struct TC {
  int ngen;                      // generators 0..ngen-1, inverses ngen..2ngen-1
  std::vector<std::vector<int>> tab;
  std::vector<int> parent;
  int inv(int g) const { return g < ngen ? g + ngen : g - ngen; }
  int find(int c) { while (parent[c] != c) c = parent[c] = parent[parent[c]]; return c; }
  int newcoset() { tab.push_back(std::vector<int>(2 * ngen, -1)); parent.push_back((int)parent.size()); return (int)tab.size() - 1; }
  std::vector<std::pair<int,int>> queue;
  void coincidence(int a, int b) {
    queue.clear(); queue.push_back({a, b});
    while (!queue.empty()) {
      auto [x, y] = queue.back(); queue.pop_back();
      x = find(x); y = find(y);
      if (x == y) continue;
      if (x > y) std::swap(x, y);
      parent[y] = x;
      for (int g = 0; g < 2 * ngen; g++) {
        int z = tab[y][g];
        if (z < 0) continue;
        tab[y][g] = -1;
        if (tab[z][inv(g)] == y) tab[z][inv(g)] = -1;
        int xx = find(x), zz = find(z);
        if (tab[xx][g] >= 0) queue.push_back({tab[xx][g], zz});
        else tab[xx][g] = zz;
        if (tab[zz][inv(g)] >= 0) queue.push_back({tab[zz][inv(g)], xx});
        else tab[zz][inv(g)] = xx;
      }
    }
  }
  void define(int c, int g) { int d = newcoset(); tab[c][g] = d; tab[d][inv(g)] = c; }
  // scan relator r from coset c, filling in (HLT)
  void scanfill(int c, const std::vector<int>& r) {
    for (;;) {
      if (find(c) != c) return;
      int f = c, i = 0, b = c, j = (int)r.size() - 1;
      while (i <= j && tab[f][r[i]] >= 0) { f = tab[f][r[i]]; i++; }
      if (i > j) { if (f != c) coincidence(f, c); return; }
      while (j >= i && tab[b][inv(r[j])] >= 0) { b = tab[b][inv(r[j])]; j--; }
      if (j < i) { coincidence(f, b); return; }
      if (i == j) { tab[f][r[i]] = b; tab[b][inv(r[i])] = f; return; }
      define(f, r[i]);
    }
  }
  int run(int ng, const std::vector<std::vector<int>>& rels, int limit) {
    ngen = ng; tab.clear(); parent.clear(); newcoset();
    for (size_t c = 0; c < tab.size(); c++) {
      if ((int)tab.size() > limit) return -1;
      for (auto& r : rels) { if (find((int)c) != (int)c) break; scanfill((int)c, r); }
      if (find((int)c) != (int)c) continue;
      for (int g = 0; g < 2 * ngen; g++) if (tab[c][g] < 0 && find((int)c) == (int)c) define((int)c, g);
    }
    int n = 0;
    for (size_t c = 0; c < tab.size(); c++) if (find((int)c) == (int)c) n++;
    return n;
  }
};

// words over s=0, t=1, S=2 (s^-1), T=3 (t^-1)
static std::vector<int> w(const std::string& s) {
  std::vector<int> v;
  for (char ch : s) v.push_back(ch == 's' ? 0 : ch == 't' ? 1 : ch == 'S' ? 2 : 3);
  return v;
}

int main() {
  int fails = 0;
  TC tc;
  // (st)^2 = s^3 = t^5 : relators stst S^3 and s^3 T^5
  int n1 = tc.run(2, {w("ststSSS"), w("sssTTTTT")}, 200000);
  std::printf("binary icosahedral <s,t | (st)^2 = s^3 = t^5>: order %d\n", n1);
  if (n1 != 120) fails++;
  int n2 = tc.run(2, {w("ss"), w("ttt"), w("ststststst")}, 200000);
  std::printf("icosahedral A5 <s,t | s^2, t^3, (st)^5>: order %d\n", n2);
  if (n2 != 60) fails++;
  // abelianization of the first presentation: rows (2-3, 2) and (3, -5), determinant -1
  long a = 2 - 3, b = 2, c = 3, d = -5, det = a * d - b * c;
  std::printf("abelianization relation matrix [[%ld,%ld],[%ld,%ld]], det %ld, so H1 = 0\n", a, b, c, d, det);
  if (std::labs(det) != 1) fails++;
  std::printf(fails ? "FAILED\n" : "OK\n");
  return fails != 0;
}
