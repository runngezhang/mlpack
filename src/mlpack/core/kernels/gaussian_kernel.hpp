/**
 * @file gaussian_kernel.hpp
 * @author Wei Guan
 * @author James Cline
 * @author Ryan Curtin
 *
 * Implementation of the Gaussian kernel (GaussianKernel).
 */
#ifndef __MLPACK_CORE_KERNELS_GAUSSIAN_KERNEL_HPP
#define __MLPACK_CORE_KERNELS_GAUSSIAN_KERNEL_HPP

#include <mlpack/core.hpp>

namespace mlpack {
namespace kernel {

/**
 * The standard Gaussian kernel.  Given two vectors @f$ x @f$, @f$ y @f$, and a
 * bandwidth @f$ \bandwidth @f$ (set in the constructor),
 *
 * @f[
 * K(x, y) = \exp(-\frac{|| x - y ||^2}{2 \bandwidth^2}).
 * @f]
 *
 * The implementation is all in the header file because it is so simple.
 */
class GaussianKernel
{
 public:
  /**
   * Default constructor; sets bandwidth to 1.0.
   */
  GaussianKernel() : bandwidth(1.0), normalizer(sqrt(2.0 * M_PI)), gamma(-0.5) { }

  /**
   * Construct the Gaussian kernel with a custom bandwidth.
   *
   * @param bandwidth The bandwidth of the kernel.
   */
  GaussianKernel(double bandwidth) :
    bandwidth(bandwidth),
    normalizer(bandwidth * sqrt(2.0 * M_PI)),
    gamma(-0.5 * pow(bandwidth, -2.0))
  { }

  /**
   * Evaluation of the Gaussian kernel.  This could be generalized to use any
   * distance metric, not the Euclidean distance, but for now, the Euclidean
   * distance is used.
   *
   * @param a First vector.
   * @param b Second vector.
   * @return K(a, b) using the bandwidth (@f$\bandwidth@f$) specified in the
   *   constructor.
   */
  double Evaluate(const arma::vec& a, const arma::vec& b) const
  {
    // The precalculation of gamma saves us some little computation time.
    arma::vec diff = b - a;
    return exp(gamma * arma::dot(diff, diff));
  }
  /**
   * Evaluation of the Gaussian kernel using a double precision argument
   *
   * @param t double value.
   * @return K(t) using the bandwidth (@f$\bandwidth@f$) specified in the
   *   constructor.
   */
  double Evaluate(double t) const {
    // The precalculation of gamma saves us some little computation time.
    return exp(gamma * t * t);
  }

  const double& Normalizer() { return normalizer; }
  const double& Bandwidth() { return bandwidth; }

 private:
  /**
   * kernel bandwidth
   * */
  double bandwidth;
  /**
   * normalizing constant
   */
  double normalizer;
  /**
   * Precalculated constant depending on the bandwidth; @f$ \gamma =
   * -\frac{1}{2 \bandwidth^2} @f$.
   */
  double gamma;
};

}; // namespace kernel
}; // namespace mlpack

#endif