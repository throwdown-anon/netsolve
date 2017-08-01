#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <random>

#include "parking_lot.hh"
#include "optimizer.cc"

using namespace std;

class CCScheme
{
private:
  double send_rate_;

public:
  CCScheme( const double send_rate )
    : send_rate_( send_rate )
  {}

  double send_rate() const { return send_rate_; }

  void feedback( const double throughput, const double random )
  {
    if ( throughput > send_rate() ) {
      throw logic_error( "throughput > send rate" );
    } else if ( throughput < send_rate() ) {
      //      const double loss_rate = send_rate() - throughput;
      send_rate_ *= random;
    } else {
      send_rate_ += .01;
    }
  }
};

int main()
{
  default_random_engine rng { random_device{}() };
  uniform_real_distribution<> random_backoff { .99, 1 };

  ParkingLot network;

  cout << setprecision( 20 );

  for ( double Ar = 0; Ar < 11; Ar += .25 ) {
    for ( double Br = 0; Br < 11; Br += .25 ) {
      for ( double Cr = 0; Cr < 11; Cr += .25 ) {
	CCScheme A { Ar }, B { Br }, C { Cr };

	ParkingLot::Rates total_throughputs { 0, 0, 0 };
	const unsigned int iteration_count = 100000;

	for ( unsigned int i = 0; i < iteration_count; i++ ) {
	  const ParkingLot::Rates send_rates { A.send_rate(), B.send_rate(), C.send_rate() };
	  const ParkingLot::Rates throughputs = network.throughputs_fast( send_rates );

	  A.feedback( get<0>( throughputs ), random_backoff( rng ) );
	  B.feedback( get<1>( throughputs ), random_backoff( rng ) );
	  C.feedback( get<2>( throughputs ), random_backoff( rng ) );

	  /* accumulate throughput */
	  get<0>( total_throughputs ) += get<0>( throughputs );
	  get<1>( total_throughputs ) += get<1>( throughputs );
	  get<2>( total_throughputs ) += get<2>( throughputs );
	}

	get<0>( total_throughputs ) /= double( iteration_count );
	get<1>( total_throughputs ) /= double( iteration_count );
	get<2>( total_throughputs ) /= double( iteration_count );

	cout << Ar << " " << Br << " " << Cr << " -> " << to_string( total_throughputs ) << "\n";
      }
    }
  }

  return EXIT_SUCCESS;
}
