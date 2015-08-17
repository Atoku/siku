/*!

  \file scheduler.cc

  \brief Implementation of scheduler

*/

#include "scheduler.hh"

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
