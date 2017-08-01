#include <cmath>
#include <tuple>
#include <limits>
#include <stdexcept>

#include "optimizer.hh"

/* PCC utility function as described in NSDI paper */
/* (rewards throughput, penalizes loss) */

/* alpha constant from PCC source code */
/* https://github.com/modong/pcc/blob/1afc958f/README.md */
/* https://github.com/modong/pcc/blob/1afc958f/sender/app/cc.h#L302 */

static double utility_function( const double sending_rate,
				const double throughput )

{
  const double loss_rate = 1 - throughput / sending_rate;
  return throughput * ( 1.0f - 1.0f / ( 1.0f + exp( -100.0f * ( loss_rate - 0.05f ) ) ) ) - sending_rate * loss_rate;
}

/* perturb one component of the rates */
template <unsigned int component>
template <class RateType>
RateType Optimizer<component>::adjust_rate( RateType rates,
					    const double delta )
{
  std::get<component>( rates ) = std::max( 0.0, std::get<component>( rates ) + delta );
  return rates;
}

/* calculate individual sender's utility for this configuration of rates */
template <unsigned int component>
template <class NetworkType>
double Optimizer<component>::evaluate( NetworkType & network,
				       const typename NetworkType::Rates & rates )
{
  const auto throughputs = network.throughputs_fast( rates );
  const double score = utility_function( std::get<component>( rates ), std::get<component>( throughputs ) );
  return score;
}

/* improve one component */
template <unsigned int component>
template <class NetworkType>
typename NetworkType::Rates Optimizer<component>::search_one( NetworkType & network,
							      typename NetworkType::Rates guess )
{
  double delta { INITIAL_DELTA };

  while ( true ) {
    /* limit size of change in throughput that can be made in one individual flow's optimization */
    if ( delta >= 0.1 ) {
      return guess;
    }

    /* don't allow flooding */
    if ( std::get<component>( guess ) > network.max_reasonable_rate() ) {
      throw std::runtime_error( "flooding detected" );
    }

    /* evaluate whether reducing, increasing, or staying the same is best */
    const auto low_guess = adjust_rate( guess, -delta );
    const auto high_guess = adjust_rate( guess, delta );

    const double low_score = evaluate( network, low_guess );
    const double mid_score = evaluate( network, guess );
    const double high_score = evaluate( network, high_guess );

    if ( mid_score >= low_score and mid_score >= high_score ) {
      return guess;
    } else if ( low_score > mid_score ) {
      guess = low_guess;
    } else {
      guess = high_guess;
    }

    delta *= 1.25;
  }
}
