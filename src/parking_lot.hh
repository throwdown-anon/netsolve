#pragma once

#include <tuple>
#include <string>

#include "node.hh"
#include "flow.hh"

class ParkingLot
{
public:
  typedef std::tuple<double, double, double> Rates;

  static constexpr double max_reasonable_rate() { return 50; }

private:
  static const constexpr unsigned int AUDIT_INTERVAL = 73;

  Node x { "x", 10 };
  Node y { "y", 10 };
  Node z { "z", 0 };

  Flow A { "A", "y" };
  Flow B { "B", "z" };
  Flow C { "C", "z" };

  Rates throughputs_shortcut( const Rates & rates ) const;

  unsigned int calculation_count_ {};
  unsigned int audit_count_ {};

public:
  Rates throughputs( const Rates & rates );

  Rates throughputs_fast( const Rates & rates );

  unsigned int audit_count() const { return audit_count_; }

  ~ParkingLot();
};

extern std::string to_string( const ParkingLot::Rates & rates );
