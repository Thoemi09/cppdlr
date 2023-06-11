#include <nda/nda.hpp>
#include <cppdlr/cppdlr.hpp>

using namespace cppdlr;
using namespace nda;

/**
* @brief Green's function which is a random sum of exponentials
*
* G_ij(t) = sum_l c_ijl K(t,om_ijl) with random c_ijl, om_ijl
*
* @param norb Number of orbital indices
* @param beta Inverse temperature
* @param t    Imaginary time evaluation point
*
* @return Green's function evaluated at t
*/
nda::matrix<double> gfun(int norb, double beta, double t) {

  int npeak = 5;

  auto g    = nda::matrix<double>(norb, norb);
  g         = 0;
  auto c    = nda::vector<double>(npeak);
  double om = 0;
  for (int i = 0; i < norb; ++i) {
    for (int j = 0; j < norb; ++j) {

      // Get random weights that sum to 1
      for (int l = 0; l < npeak; ++l) {
        c(l) = (sin(1000.0 * (i + 2 * j + 3 * l + 7)) + 1) / 2; // Quick and dirty rand # gen on [0,1]
      }
      c = c / sum(c);

      // Evaluate Green's function
      for (int l = 0; l < npeak; ++l) {
        om = sin(2000.0 * (3 * i + 2 * j + l + 6)); // Rand # on [-1,1]
        g(i, j) += c(l) * k_it(t, beta * om);
      }
    }
  }

  return g;
}

/**
* @brief Compare symmetrized and unsymmetrized DLR for interpolation and
* evaluation of matrix-valued Green's function in imaginary time
*/
int main() {

  double lambda = 1000;  // DLR cutoff
  double eps    = 1e-10; // DLR tolerance

  double beta = 1000;  // Inverse temperature
  int ntst    = 10000; // # imag time test points

  int norb = 2; // Orbital dimensions

  // Get DLR frequencies
  auto dlr_rf     = build_dlr_rf(lambda, eps);
  auto dlr_rf_sym = build_dlr_rf(lambda, eps, SYM);

  int r    = dlr_rf.size();
  int rsym = dlr_rf_sym.size();

  // Get DLR imaginary time object
  auto itops     = imtime_ops(lambda, dlr_rf);
  auto itops_sym = imtime_ops(lambda, dlr_rf_sym, SYM);

  // Sample Green's function G at DLR imaginary time nodes
  auto const &dlr_it     = itops.get_itnodes();
  auto const &dlr_it_sym = itops_sym.get_itnodes();

  auto g     = nda::array<double, 3>(r, norb, norb);
  auto g_sym = nda::array<double, 3>(rsym, norb, norb);
  for (int i = 0; i < r; ++i) { g(i, _, _) = gfun(norb, beta, dlr_it(i)); }
  for (int i = 0; i < rsym; ++i) { g_sym(i, _, _) = gfun(norb, beta, dlr_it_sym(i)); }

  // DLR coefficients of G
  auto gc     = itops.vals2coefs(g);
  auto gc_sym = itops_sym.vals2coefs(g_sym);

  // Get test points in relative format
  auto ttst = eqptsrel(ntst);

  // Compute L infinity error
  auto gtru     = nda::matrix<double>(norb, norb);
  auto gtst     = nda::matrix<double>(norb, norb);
  auto gtst_sym = nda::matrix<double>(norb, norb);
  double err = 0, err_sym = 0;
  for (int i = 0; i < ntst; ++i) {
    gtru = gfun(norb, beta, ttst(i));

    gtst     = itops.coefs2eval(gc, ttst(i));
    gtst_sym = itops_sym.coefs2eval(gc_sym, ttst(i));

    err     = std::max(err, max_element(abs(gtru - gtst)));
    err_sym = std::max(err_sym, max_element(abs(gtru - gtst_sym)));
  }

  // Print results
  std::cout << "Unsymmetrized DLR rank = " << r << std::endl;
  std::cout << "Symmetrized DLR rank = " << rsym << std::endl;
  std::cout << "L infinity error for unsymmetrized DLR = " << err << std::endl;
  std::cout << "L infinity error for symmetrized DLR = " << err_sym << std::endl;
}