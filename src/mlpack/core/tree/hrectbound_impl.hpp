/**
 * @file hrectbound_impl.hpp
 *
 * Implementation of hyper-rectangle bound policy class.
 * Template parameter t_pow is the metric to use; use 2 for Euclidean (L2).
 *
 * @experimental
 */
#ifndef __MLPACK_CORE_TREE_HRECTBOUND_IMPL_HPP
#define __MLPACK_CORE_TREE_HRECTBOUND_IMPL_HPP

#include <math.h>

#include "../math/math_misc.hpp"

// In case it has not been included yet.
#include "hrectbound.hpp"

namespace mlpack {
namespace bound {

/**
 * Empty constructor
 */
template<int t_pow>
HRectBound<t_pow>::HRectBound() :
    dim(0),
    bounds(NULL)
{ /* nothing to do */ }

/**
 * Initializes to specified dimensionality with each dimension the empty
 * set.
 */
template<int t_pow>
HRectBound<t_pow>::HRectBound(const size_t dimension) :
    dim(dimension),
    bounds(new math::Range[dim])
{ /* nothing to do */ }

/***
 * Copy constructor necessary to prevent memory leaks.
 */
template<int t_pow>
HRectBound<t_pow>::HRectBound(const HRectBound& other) :
    dim(other.Dim()),
    bounds(new math::Range[dim])
{
  // Copy other bounds over.
  for (size_t i = 0; i < dim; i++)
    bounds[i] = other[i];
}

/***
 * Same as the copy constructor.
 */
template<int t_pow>
HRectBound<t_pow>& HRectBound<t_pow>::operator=(const HRectBound& other)
{
  if (bounds)
    delete[] bounds;

  // We can't just copy the bounds_ pointer like the default copy constructor
  // will!
  dim = other.Dim();
  bounds = new math::Range[dim];
  for (size_t i = 0; i < dim; i++)
    bounds[i] = other[i];

  return *this;
}

/**
 * Destructor: clean up memory
 */
template<int t_pow>
HRectBound<t_pow>::~HRectBound()
{
  if (bounds)
    delete[] bounds;
}

/**
 * Resets all dimensions to the empty set.
 */
template<int t_pow>
void HRectBound<t_pow>::Clear()
{
  for (size_t i = 0; i < dim; i++)
    bounds[i] = math::Range();
}

/**
 * Gets the range for a particular dimension.
 */
template<int t_pow>
inline const math::Range& HRectBound<t_pow>::operator[](const size_t i) const
{
  return bounds[i];
}

/**
 * Sets the range for the given dimension.
 */
template<int t_pow>
inline math::Range& HRectBound<t_pow>::operator[](const size_t i)
{
  return bounds[i];
}

/***
 * Calculates the centroid of the range, placing it into the given vector.
 *
 * @param centroid Vector which the centroid will be written to.
 */
template<int t_pow>
void HRectBound<t_pow>::Centroid(arma::vec& centroid) const
{
  // set size correctly if necessary
  if (!(centroid.n_elem == dim))
    centroid.set_size(dim);

  for (size_t i = 0; i < dim; i++)
    centroid(i) = bounds[i].Mid();
}

/**
 * Calculates minimum bound-to-point squared distance.
 */
template<int t_pow>
double HRectBound<t_pow>::MinDistance(const arma::vec& point) const
{
  Log::Assert(point.n_elem == dim);

  double sum = 0;

  double lower, higher;
  for (size_t d = 0; d < dim; d++)
  {
    lower = bounds[d].lo - point[d];
    higher = point[d] - bounds[d].hi;

    // Since only one of 'lower' or 'higher' is negative, if we add each's
    // absolute value to itself and then sum those two, our result is the
    // nonnegative half of the equation times two; then we raise to power t_pow.
    sum += pow((lower + fabs(lower)) + (higher + fabs(higher)), (double) t_pow);
  }

  // Now take the t_pow'th root (but make sure our result is squared); then
  // divide by four to cancel out the constant of 2 (which has been squared now)
  // that was introduced earlier.
  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Calculates minimum bound-to-point squared distance, filtered
 *   by indices.
 */
template<int t_pow>
double HRectBound<t_pow>::MinDistance(const arma::vec& point,
                                      const std::vector<size_t>& indices) const
{
  Log::Assert(point.n_elem == dim);

  double sum = 0.0;

  double lower, higher;
  for (size_t index = 0; index < indices.size(); index++)
  {
    size_t dimension = indices[index];
    lower = bounds[dimension].lo - point[dimension];
    higher = point[dimension] - bounds[dimension].hi;

    // Since at least one of 'lower' or 'higher' is negative, if we add each's
    // absolute value to itself and then sum those two, our result is the
    // nonnegative half of the equation times two; then we raise to power t_pow.
    sum += pow((lower + fabs(lower)) + (higher + fabs(higher)), (double) t_pow);
  }

  // Now take the t_pow'th root (but make sure our result is squared); then
  // divide by four to cancel out the constant of 2 (which has been squared now)
  // that was introduced earlier.
  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Calculates minimum bound-to-bound squared distance.
 *
 * Example: bound1.MinDistanceSq(other) for minimum squared distance.
 */
template<int t_pow>
double HRectBound<t_pow>::MinDistance(const HRectBound& other) const
{
  Log::Assert(dim == other.dim);

  double sum = 0;
  const math::Range* mbound = bounds;
  const math::Range* obound = other.bounds;

  double lower, higher;
  for (size_t d = 0; d < dim; d++)
  {
    lower = obound->lo - mbound->hi;
    higher = mbound->lo - obound->hi;
    // We invoke the following:
    //   x + fabs(x) = max(x * 2, 0)
    //   (x * 2)^2 / 4 = x^2
    sum += pow((lower + fabs(lower)) + (higher + fabs(higher)), (double) t_pow);

    // Move bound pointers.
    mbound++;
    obound++;
  }

  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Calculates minimum bound-to-bound squared distance, filtered by indices.
 *
 * Example: bound1.MinDistanceSq(other, indices) for minimum squared distance.
 */
template<int t_pow>
double HRectBound<t_pow>::MinDistance(const HRectBound& other,
                                      const std::vector<size_t>& indices) const
{
  Log::Assert(dim == other.dim);

  double sum = 0.0;
  double lower, higher;

  for (size_t index = 0; index < indices.size(); index++)
  {
    size_t dimension = indices[index];
    lower = bounds[dimension].lo - other.bounds[dimension].hi;
    higher = other.bounds[dimension].lo - bounds[dimension].hi;

    // Since only one of 'lower' or 'higher' is negative, if we add each's
    // absolute value to itself and then sum those two, our result is the
    // nonnegative half of the equation times two; then we raise to power t_pow.
    sum += pow((lower + fabs(lower)) + (higher + fabs(higher)), (double) t_pow);
  }

  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Calculates maximum bound-to-point squared distance.
 */
template<int t_pow>
double HRectBound<t_pow>::MaxDistance(const arma::vec& point) const
{
  double sum = 0;

  Log::Assert(point.n_elem == dim);

  for (size_t d = 0; d < dim; d++)
  {
    double v = fabs(std::max(point[d] - bounds[d].lo,
                             bounds[d].hi - point[d]));
    sum += pow(v, (double) t_pow);
  }

  return pow(sum, 2.0 / (double) t_pow);
}

/**
 * Calculates maximum bound-to-point squared distance, filtered by indices.
 */
template<int t_pow>
double HRectBound<t_pow>::MaxDistance(const arma::vec& point,
                                      const std::vector<size_t>& indices) const
{
  double sum = 0.0;
  double lower, higher;

  Log::Assert(point.n_elem == dim);

  for (size_t index = 0; index < indices.size(); index++)
  {
    size_t dimension = indices[index];
    lower = fabs(point[dimension] - bounds[dimension].lo);
    higher = fabs(point[dimension] - bounds[dimension].hi);

    sum += pow(fabs(higher - lower) + higher + lower, (double) t_pow);
  }

  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Computes maximum distance.
 */
template<int t_pow>
double HRectBound<t_pow>::MaxDistance(const HRectBound& other) const
{
  double sum = 0;

  Log::Assert(dim == other.dim);

  double v;
  for (size_t d = 0; d < dim; d++)
  {
    v = fabs(std::max(other.bounds[d].hi - bounds[d].lo,
                      bounds[d].hi - other.bounds[d].lo));
    sum += pow(v, (double) t_pow); // v is non-negative.
  }

  return pow(sum, 2.0 / (double) t_pow);
}

/**
 * Computes the maximum distance between blocks,
 *   filtered by indices.
 */
template<int t_pow>
double HRectBound<t_pow>::MaxDistance(const HRectBound& other,
                                      const std::vector<size_t>& indices) const
{
  double sum = 0.0;
  double lower, higher;

  Log::Assert(other.dim == dim);

  for (size_t index = 0; index < indices.size(); index++)
  {
    size_t dimension = indices[index];
    lower = fabs(other.bounds[dimension].hi - bounds[dimension].lo);
    higher = fabs(other.bounds[dimension].lo - bounds[dimension].hi);

    sum += pow(fabs(higher-lower) + higher + lower, (double) t_pow);
  }

  return pow(sum, 2.0 / (double) t_pow) / 4.0;
}

/**
 * Calculates minimum and maximum bound-to-bound squared distance.
 */
template<int t_pow>
math::Range HRectBound<t_pow>::RangeDistance(const HRectBound& other) const
{
  double loSum = 0;
  double hiSum = 0;

  Log::Assert(dim == other.dim);

  double v1, v2, vLo, vHi;
  for (size_t d = 0; d < dim; d++)
  {
    v1 = other.bounds[d].lo - bounds[d].hi;
    v2 = bounds[d].lo - other.bounds[d].hi;
    // One of v1 or v2 is negative.
    if (v1 >= v2)
    {
      vHi = -v2; // Make it nonnegative.
      vLo = (v1 > 0) ? v1 : 0; // Force to be 0 if negative.
    }
    else
    {
      vHi = -v1; // Make it nonnegative.
      vLo = (v2 > 0) ? v2 : 0; // Force to be 0 if negative.
    }

    loSum += pow(vLo, (double) t_pow);
    hiSum += pow(vHi, (double) t_pow);
  }

  return math::Range(pow(loSum, 2.0 / (double) t_pow),
                     pow(hiSum, 2.0 / (double) t_pow));
}

/**
 * Calculates minimum and maximum bound-to-point squared distance.
 */
template<int t_pow>
math::Range HRectBound<t_pow>::RangeDistance(const arma::vec& point) const
{
  double loSum = 0;
  double hiSum = 0;

  Log::Assert(point.n_elem == dim);

  double v1, v2, vLo, vHi;
  for (size_t d = 0; d < dim; d++)
  {
    v1 = bounds[d].lo - point[d]; // Negative if point[d] > lo.
    v2 = point[d] - bounds[d].hi; // Negative if point[d] < hi.
    // One of v1 or v2 (or both) is negative.
    if (v1 >= 0) // point[d] <= bounds_[d].lo.
    {
      vHi = -v2; // v2 will be larger but must be negated.
      vLo = v1;
    }
    else // point[d] is between lo and hi, or greater than hi.
    {
      if (v2 >= 0)
      {
        vHi = -v1; // v1 will be larger, but must be negated.
        vLo = v2;
      }
      else
      {
        vHi = -std::min(v1, v2); // Both are negative, but we need the larger.
        vLo = 0;
      }
    }

    loSum += pow(vLo, (double) t_pow);
    hiSum += pow(vHi, (double) t_pow);
  }

  return math::Range(pow(loSum, 2.0 / (double) t_pow),
                     pow(hiSum, 2.0 / (double) t_pow));
}

/**
 * Expands this region to include a new point.
 */
template<int t_pow>
HRectBound<t_pow>& HRectBound<t_pow>::operator|=(const arma::vec& vector)
{
  Log::Assert(vector.n_elem == dim);

  for (size_t i = 0; i < dim; i++)
    bounds[i] |= vector[i];

  return *this;
}

/**
 * Expands this region to encompass another bound.
 */
template<int t_pow>
HRectBound<t_pow>& HRectBound<t_pow>::operator|=(const HRectBound& other)
{
  assert(other.dim == dim);

  for (size_t i = 0; i < dim; i++)
    bounds[i] |= other.bounds[i];

  return *this;
}

/**
 * Determines if a point is within this bound.
 */
template<int t_pow>
bool HRectBound<t_pow>::Contains(const arma::vec& point) const
{
  for (size_t i = 0; i < point.n_elem; i++)
  {
    if (!bounds[i].Contains(point(i)))
      return false;
  }

  return true;
}

}; // namespace bound
}; // namespace mlpack

#endif // __MLPACK_CORE_TREE_HRECTBOUND_IMPL_HPP