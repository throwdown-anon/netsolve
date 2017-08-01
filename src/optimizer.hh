#pragma once

#include <limits>

template <unsigned int component>
class Optimizer
{
private:
  /* smallest amount to attempt to increas or decrease a rate */
  static const constexpr double INITIAL_DELTA = 1024 * std::numeric_limits<double>::epsilon();

  template <class RateType>
  static RateType adjust_rate( RateType rates,
			       const double delta );

  template <class NetworkType>
  static double evaluate( NetworkType & network, const typename NetworkType::Rates & rates );

public:
  template <class NetworkType>
  static typename NetworkType::Rates search_one( NetworkType & network,
						 typename NetworkType::Rates guess );
};
