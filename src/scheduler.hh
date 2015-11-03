/*!

  \file scheduler.hh

  \brief Scheduling events class such as calls of diagnostics
  functions. It relates its time with model time and when model time
  exceeds scheduled time, it flags for the event.

*/

#ifndef SCHEDULER_HH
#define SCHEDULER_HH

#include "modeltime.hh"

using namespace boost::posix_time;
using namespace boost::gregorian;

//! \brief Additional class for scheduling events like calls of
//! diagnostics functions
class Scheduler
{
 public:

  //! \brief sets next model time to trigger the event
  void set_event_time( const int year,
                       const int month,
                       const int day,
                       const int hh,
                       const int mm,
                       const int ss,
                       const int ms );

  //! \brief sets next model time to trigger the event using posix
  //! time
  void set_event_time( boost::posix_time::ptime new_tevent )
  { tevent = new_tevent; };

  //! \brief sets time interval for event frequency
  void set_dt( const int dd,
               const int ss,
               const int ms )
  { dt = hours( dd*24 ) + seconds( ss ) + millisec( ms ); };

  //! \brief sets time interval for event frequency
  void set_dt( boost::posix_time::time_duration dt_new )
  { dt = dt_new; };

  //! \brief true if event 
  bool is_event( boost::posix_time::ptime model_current )
  { return model_current >= tevent; };

  //! \brief increments the next event time by dt 
  void increment_event_time()
  { tevent += dt; };

  //! \brief time access for saving
  void get_dt_as_dtstamp( ModelTimeTypes::dtstamp* ) const;
  void get_tevent_as_timestamp( ModelTimeTypes::timestamp* ) const;

 private:

  boost::posix_time::time_duration dt; //!< time step
  boost::posix_time::ptime tevent;     //!< time for next event

  // ---------------- private methods ---------------

  // cloned from 'ModelTime'
  void get_as_timestamp( const boost::posix_time::ptime & t,
                       ModelTimeTypes::timestamp* pmts ) const;

  void get_as_dtstamp( const boost::posix_time::time_duration & dt,
                       ModelTimeTypes::dtstamp* pmts ) const;

};

#endif      /* SCHEDULER_HH */

