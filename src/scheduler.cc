/*!

  \file scheduler.cc

  \brief Implementation of scheduler

*/

#include "scheduler.hh"
#include "modeltime.hh"

using namespace boost::posix_time;
using namespace boost::gregorian;

//---------------------------------------------------------------------

void Scheduler::set_event_time( const int year,
                                const int month,
                                const int day,
                                const int hh,
                                const int mm,
                                const int ss,
                                const int ms )
{
  date start_date( year, month, day );
  tevent = ptime( start_date, hours(hh) + 
                  minutes(mm) + seconds(ss) + millisec(ms) );
}

//---------------------------------------------------------------------

void Scheduler::get_dt_as_dtstamp( ModelTimeTypes::dtstamp* pmts ) const
{
  get_as_dtstamp( dt, pmts );
}

//---------------------------------------------------------------------

void Scheduler::get_tevent_as_timestamp( ModelTimeTypes::timestamp* pmts ) const
{
  get_as_timestamp( tevent, pmts );
}

//---------------------------------------------------------------------

void Scheduler::get_as_timestamp( const boost::posix_time::ptime & t,
                                ModelTimeTypes::timestamp* pmts ) const
{
  pmts->year    = t.date().year();
  pmts->month   = t.date().month();
  pmts->day     = t.date().day();
  pmts->hours   = t.time_of_day().hours();
  pmts->minutes = t.time_of_day().minutes();
  pmts->seconds = t.time_of_day().seconds();
  pmts->microseconds =
    t.time_of_day().total_microseconds() -
    1000000 * t.time_of_day().total_seconds();
}

//---------------------------------------------------------------------

void Scheduler::get_as_dtstamp( const boost::posix_time::time_duration & dt,
                     ModelTimeTypes::dtstamp* pmts ) const
{
  pmts->hours   = dt.hours();
  pmts->minutes = dt.minutes();
  pmts->seconds = dt.seconds();
  pmts->microseconds =
    dt.total_microseconds() -
    1000000 * dt.total_seconds();
}
