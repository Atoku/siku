/*!

  \file modeltime.cc

  \brief Model time methods implementation

*/

#include "siku.hh"

#include "modeltime.hh"
#include "errors.hh"

using namespace boost::posix_time;
using namespace boost::gregorian;


//---------------------------------------------------------------------
void ModelTime::set_marker( const unsigned int marker,
                            const int year,
                            const int month,
                            const int day,
                            const int hh,
                            const int mm,
                            const int ss,
                            const int ms )
{
  ptime *pmarker;

  // choosing what we set
  switch( marker )
    {
    case MARKER_START:
      pmarker = &start;             break;
    case MARKER_FINISH:
      pmarker = &finish;            break;
    case MARKER_CURRENT:
      pmarker = &current;           break;
    case MARKER_SAVE:
      pmarker = &save;              break;
    default:
      fatal( 2, "Wrong marker type" );
    }

  date start_date( year, month, day );
  *pmarker = ptime( start_date, hours(hh) + 
                    minutes(mm) + seconds(ss) + millisec(ms) );
}

//---------------------------------------------------------------------
void ModelTime::set_marker( const unsigned int marker,
                            const boost::posix_time::ptime& t_new )
{
  ptime *pmarker;

  // choosing what we set
  switch( marker )
    {
    case MARKER_START:
      pmarker = &start;             break;
    case MARKER_FINISH:
      pmarker = &finish;            break;
    case MARKER_CURRENT:
      pmarker = &current;           break;
    case MARKER_SAVE:
      pmarker = &save;              break;
    default:
      fatal( 2, "Wrong marker type" );
    }

  *pmarker = t_new;
}

//---------------------------------------------------------------------
void ModelTime::set_dt( const unsigned int dtflag, 
                        const int dd,
                        const int ss, 
                        const int ms )
{
  time_duration *pdt;

  switch( dtflag )
    {
    case DT_DT:
      pdt = &dt;            break;
    case DT_DTS:
      pdt = &dts;           break;
    default:
      fatal( 2, "Wrong dtflag type" );
    }

  *pdt = hours( dd*24 ) + seconds( ss ) + millisec( ms );
}

//---------------------------------------------------------------------
void ModelTime::set_dt( const unsigned int dtflag, 
                        const boost::posix_time::time_duration& dt_new )
{
  time_duration *pdt;

  switch( dtflag )
    {
    case DT_DT:
      pdt = &dt;            break;
    case DT_DTS:
      pdt = &dts;           break;
    default:
      fatal( 2, "Wrong dtflag type" );
    }

  *pdt = dt_new;
}

//---------------------------------------------------------------------
boost::posix_time::ptime ModelTime::get_current() const
{
  boost::posix_time::ptime current_copy (current);
  return current_copy;
}

//---------------------------------------------------------------------
void 
ModelTime::get_current_as_timestamp( ModelTimeTypes::timestamp* pmts ) const
{
  get_as_timestamp( current, pmts );
}

//---------------------------------------------------------------------
void ModelTime::get_start_as_timestamp( ModelTimeTypes::timestamp* pmts ) const
{
  get_as_timestamp( start, pmts );
}

//---------------------------------------------------------------------
void ModelTime::get_finish_as_timestamp( ModelTimeTypes::timestamp* pmts ) const
{
  get_as_timestamp( finish, pmts );
}

//---------------------------------------------------------------------
void ModelTime::get_save_as_timestamp( ModelTimeTypes::timestamp* pmts ) const
{
  get_as_timestamp( save, pmts );
}

//---------------------------------------------------------------------
void ModelTime::get_dt_as_dtstamp( ModelTimeTypes::dtstamp* pmts ) const
{
  get_as_dtstamp( dt, pmts );
}

//---------------------------------------------------------------------
void ModelTime::get_dts_as_dtstamp( ModelTimeTypes::dtstamp* pmts ) const
{
  get_as_dtstamp( dts, pmts );
}

//---------------------------------------------------------------------

void ModelTime::get_as_timestamp( const boost::posix_time::ptime & t,
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

void ModelTime::get_as_dtstamp( const boost::posix_time::time_duration & dt,
                     ModelTimeTypes::dtstamp* pmts ) const
{
  pmts->hours   = dt.hours();
  pmts->minutes = dt.minutes();
  pmts->seconds = dt.seconds();
  pmts->microseconds = 
    dt.total_microseconds() - 
    1000000 * dt.total_seconds();
}

//---------------------------------------------------------------------
void ModelTime::print() const
{
  std::cout << "Model time info pretty print:" << std::endl;
  std::cout << "Start:   " << to_simple_string( start )   << std::endl;
  std::cout << "Finish:  " << to_simple_string( finish )  << std::endl;
  std::cout << "Current: " << to_simple_string( current ) << std::endl;
  std::cout << "Save:    " << to_simple_string( save )    << std::endl;
  std::cout << std::endl;
  std::cout << "dt:      " << to_simple_string( dt )  << std::endl;
  std::cout << "dts:     " << to_simple_string( dts ) << std::endl;
}
