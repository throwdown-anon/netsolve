#include <stdexcept>
#include <iostream>

#include "parking_lot.hh"

using namespace std;

ParkingLot::Rates ParkingLot::throughputs( const ParkingLot::Rates & rates )
{
  double A_rate, B_rate, C_rate;
  tie( A_rate, B_rate, C_rate ) = rates;
  /* in C++17: const auto [ A_rate, B_rate, C_rate ] = rates; */

  A.set_rate( A_rate );
  B.set_rate( B_rate );
  C.set_rate( C_rate );

  x.add_flow( A );
  y.add_flow( B );
  x.add_flow( C );

  x.connect( y );
  y.connect( z );

  return { y.terminal_flow( "A" ).rate(),
           z.terminal_flow( "B" ).rate(),
           z.terminal_flow( "C" ).rate() };
}

ParkingLot::Rates ParkingLot::throughputs_shortcut( const Rates & rates ) const
{
  double A_rate, B_rate, C_rate;
  tie( A_rate, B_rate, C_rate ) = rates;
  /* in C++17: const auto [ A_rate, B_rate, C_rate ] = rates; */

  const double x_transiting_input = A_rate + C_rate;
  const double x_output = min( x_transiting_input, x.output_capacity() );
  const double x_delivery_proportion = x_transiting_input ? x_output / x_transiting_input : 1;

  const double A_throughput = A_rate * x_delivery_proportion;

  const double C_rate_after_x = C_rate * x_delivery_proportion;

  const double y_transiting_input = C_rate_after_x + B_rate;
  const double y_output = min( y_transiting_input, y.output_capacity() );
  const double y_delivery_proportion = y_transiting_input ? y_output / y_transiting_input : 1;

  const double B_throughput = B_rate * y_delivery_proportion;
  const double C_throughput = C_rate_after_x * y_delivery_proportion;

  return { A_throughput, B_throughput, C_throughput };
}

ParkingLot::Rates ParkingLot::throughputs_fast( const ParkingLot::Rates & rates )
{
  const auto fast_answer = throughputs_shortcut( rates );

  if ( calculation_count_++ % AUDIT_INTERVAL == 0 ) {
    const auto slow_answer = throughputs( rates );
    audit_count_++;
    if ( fast_answer != slow_answer ) {
      throw logic_error( "fast and slow throughput calculations disagree" );
    }
  }

  return fast_answer;
}

string to_string( const ParkingLot::Rates & rates )
{
  return to_string( get<0>( rates ) )
    + " " + to_string( get<1>( rates ) )
    + " " + to_string( get<2>( rates ) );
}

ParkingLot::~ParkingLot()
{
  cerr << "Number of successful audits of fast path: " << audit_count() << "\n";
}
