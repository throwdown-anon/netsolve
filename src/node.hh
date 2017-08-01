#pragma once

#include <unordered_map>

#include "flow.hh"

class Node
{
private:
  std::string name_;
  std::unordered_map<std::string, Flow> inputs_ {};
  double output_capacity_;

  bool flow_transits_node( const Flow & flow ) const;
  double total_transiting_inputs() const;
  double delivery_proportion() const;
  double output_rate() const;

public:
  Node( const std::string & name,
	  const double output_capacity )
    : name_( name ),
      output_capacity_( output_capacity )
  {}

  double output_capacity() const { return output_capacity_; }

  void add_flow( const Flow & flow );
  void connect( Node & next_hop ) const;
  const Flow & terminal_flow( const std::string & source ) const;
};
