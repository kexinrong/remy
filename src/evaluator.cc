#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "configrange.hh"
#include "evaluator.hh"
#include "network.cc"
#include "rat-templates.cc"
#include "aimd-templates.cc"

const unsigned int TICK_COUNT = 10000;

Evaluator::Evaluator( const ConfigRange & range )
  : _prng( global_PRNG()() ), /* freeze the PRNG seed for the life of this Evaluator */
    _configs()
{
//    /* Rat vs AIMD, 1 second on, 1 second off */
//    _configs.push_back( NetConfig().set_link_ppt( range.link_packets_per_ms.first )
//                                   .set_delay( range.rtt_ms.first )
//                                   .set_num_senders1( 1 )
//                                   .set_num_senders2( 1 )
//                                   .set_on_duration( 1000.0 )
//                                   .set_off_duration( 1000.0 ) );
//
//    /* Rat vs AIMD, always on */
//    _configs.push_back( NetConfig().set_link_ppt( range.link_packets_per_ms.first )
//                                   .set_delay( range.rtt_ms.first )
//                                   .set_num_senders1( 1 )
//                                   .set_num_senders2( 1 )
//                                   .set_on_duration( 1000000000.0 )
//                                   .set_off_duration( 0.0 ) );
//
//    /* Two rats, 1 second on, 1 second off */
//    _configs.push_back( NetConfig().set_link_ppt( range.link_packets_per_ms.first )
//                                   .set_delay( range.rtt_ms.first )
//                                   .set_num_senders1( 2 )
//                                   .set_num_senders2( 0 )
//                                   .set_on_duration( 1000.0 )
//                                   .set_off_duration( 1000.0 ) );

    /* Two rats, always on */
    _configs.push_back( NetConfig().set_link_ppt( range.link_packets_per_ms.first )
                                   .set_delay( range.rtt_ms.first )
                                   .set_num_senders1( 2 )
                                   .set_num_senders2( 0 )
                                   .set_on_duration( 1000000000.0 )
                                   .set_off_duration( 0.0 ) );

}

Evaluator::Outcome Evaluator::score( WhiskerTree & run_whiskers,
				     const bool trace, const unsigned int carefulness ) const
{
  PRNG run_prng( _prng );

  run_whiskers.reset_counts();

  /* run tests */
  Outcome the_outcome;
  for ( auto &x : _configs ) {
    /* run once */
    Network<Rat, Aimd> network1( Rat( run_whiskers, trace ), Aimd(), run_prng, x );
    network1.run_simulation( TICK_COUNT * carefulness );

    the_outcome.score += network1.senders().utility();
    the_outcome.throughputs_delays.emplace_back( x, network1.senders().throughputs_delays() );
  }

  the_outcome.used_whiskers = run_whiskers;

  return the_outcome;
}
