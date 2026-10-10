/* Integral homology of the boundary of the 4-simplex, a triangulated 3-sphere,
   computed by Smith normal form of the simplicial boundary matrices.
   Expected: H0 = Z, H1 = 0, H2 = 0, H3 = Z, no torsion, Euler characteristic 0.
   Also checks the lens space L(5,1) presentation: H1 = Z/5. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 64
typedef long long ll;

static int nsimp[5];
static int simp[5][MAXN][5];

static void enumerate(int n) { /* k-faces of the full simplex on n vertices */
  for (int k = 0; k <= 3; k++) {
    nsimp[k] = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
      if (__builtin_popcount(mask) != k + 1) continue;
      int c = 0;
      for (int v = 0; v < n; v++) if (mask >> v & 1) simp[k][nsimp[k]][c++] = v;
      nsimp[k]++;
    }
  }
}

static int find(int k, const int *s) {
  for (int i = 0; i < nsimp[k]; i++) {
    int ok = 1;
    for (int j = 0; j <= k; j++) if (simp[k][i][j] != s[j]) { ok = 0; break; }
    if (ok) return i;
  }
  return -1;
}

/* Smith normal form: returns rank, writes the nonzero invariant factors */
static int snf(ll a[MAXN][MAXN], int r, int c, ll *d) {
  int rank = 0;
  for (int t = 0; t < r && t < c; t++) {
    for (;;) {
      int pi = -1, pj = -1; ll best = 0;
      for (int i = t; i < r; i++)
        for (int j = t; j < c; j++)
          if (a[i][j] && (best == 0 || llabs(a[i][j]) < best)) { best = llabs(a[i][j]); pi = i; pj = j; }
      if (pi < 0) return rank;
      for (int j = 0; j < c; j++) { ll x = a[t][j]; a[t][j] = a[pi][j]; a[pi][j] = x; }
      for (int i = 0; i < r; i++) { ll x = a[i][t]; a[i][t] = a[i][pj]; a[i][pj] = x; }
      int done = 1;
      for (int i = t + 1; i < r; i++) {
        ll q = a[i][t] / a[t][t];
        for (int j = t; j < c; j++) a[i][j] -= q * a[t][j];
        if (a[i][t]) done = 0;
      }
      for (int j = t + 1; j < c; j++) {
        ll q = a[t][j] / a[t][t];
        for (int i = t; i < r; i++) a[i][j] -= q * a[i][t];
        if (a[t][j]) done = 0;
      }
      if (!done) continue;
      int divides = 1;
      for (int i = t + 1; i < r && divides; i++)
        for (int j = t + 1; j < c; j++)
          if (a[i][j] % a[t][t]) { for (int jj = t; jj < c; jj++) a[t][jj] += a[i][jj]; divides = 0; break; }
      if (divides) break;
    }
    d[rank++] = llabs(a[t][t]);
  }
  return rank;
}

static ll M[MAXN][MAXN];

int main(void) {
  int fails = 0;
  enumerate(5);
  int rank[5] = {0}; int ntors[5] = {0};
  for (int k = 1; k <= 3; k++) { /* boundary d_k : C_k -> C_{k-1} */
    memset(M, 0, sizeof M);
    for (int j = 0; j < nsimp[k]; j++)
      for (int i = 0; i <= k; i++) {
        int f[5], c = 0;
        for (int m = 0; m <= k; m++) if (m != i) f[c++] = simp[k][j][m];
        M[find(k - 1, f)][j] += (i % 2 ? -1 : 1);
      }
    ll d[MAXN];
    rank[k] = snf(M, nsimp[k - 1], nsimp[k], d);
    for (int i = 0; i < rank[k]; i++) if (d[i] > 1) ntors[k - 1]++;
  }
  int betti[4];
  for (int k = 0; k <= 3; k++) {
    int z = nsimp[k] - (k ? rank[k] : 0);
    int b = (k < 3) ? rank[k + 1] : 0;
    betti[k] = z - b;
  }
  int chi = nsimp[0] - nsimp[1] + nsimp[2] - nsimp[3];
  printf("boundary of the 4-simplex: f-vector (%d,%d,%d,%d), chi = %d\n", nsimp[0], nsimp[1], nsimp[2], nsimp[3], chi);
  printf("Betti numbers %d %d %d %d, torsion in H0..H2: %d %d %d\n", betti[0], betti[1], betti[2], betti[3], ntors[0], ntors[1], ntors[2]);
  if (!(betti[0] == 1 && betti[1] == 0 && betti[2] == 0 && betti[3] == 1)) fails++;
  if (ntors[0] + ntors[1] + ntors[2]) fails++;
  if (chi != 0) fails++;

  /* L(5,1): one generator a, one relator a^5, so H1 = Z/5 (a homology computation, not a sphere) */
  memset(M, 0, sizeof M); M[0][0] = 5; ll d[MAXN];
  int r = snf(M, 1, 1, d);
  printf("L(5,1): H1 invariant factor %lld\n", r ? d[0] : 0);
  if (!(r == 1 && d[0] == 5)) fails++;

  printf(fails ? "FAILED\n" : "OK\n");
  return fails != 0;
}
