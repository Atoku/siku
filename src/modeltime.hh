/*!

  \file modeltime.hh

  \brief Model time and all other aux info for tracing time in the
  model, converstions, updates and so on.

  This class uses boost/date_time to keep trac of the current date and
  be able to convert to UTC (GMT).

*/

#ifndef MODELTIME_HH
#define MODELTIME_HH

#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/date_time/posix_time/posix_time_types.hpp"

//! \brief Types suitable to store model time are defined
namespace ModelTimeTypes
{

  // Structure to save the data from model time
  struct timestamp {
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
    long microseconds;
  };

  // Structure to save time step
  struct dtstamp {
    long hours;
    long minutes;
    long seconds;
    long microseconds;
  };
}

//! \brief Model time and methods to update it Model time. 
//!
//! The time accuracy is just milliseconds. There is no need for
//! better accuarcy as we increment time independently of all variables.
class ModelTime
{
public:

  // constants

  static const unsigned int MARKER_START   = 0x0; //!< flag for setting start
  static const unsigned int MARKER_FINISH  = 0x1; //!< flag for setting finish 
  static const unsigned int MARKER_CURRENT = 0x2; //!< flag for setting current
  static const unsigned int MARKER_SAVE    = 0x3; //!< flag for setting tsave

  static const unsigned int DT_DT    = 0x0;       //!< flag for setting dt
  static const unsigned int DT_DTS   = 0x1;       //!< flag for setting dts

  //!\brief set a specific model time marker
  //!\param[in] marker: one of MODELTIME_MARKER_* constants
  void set_marker( const unsigned int marker,
                   const int year, const int month, const int day, 
                   const int hour, const int minutes, const int seconds, 
                   const int ms );

  //!\brief set a specific model time marker
  //!\param[in] marker one of MODELTIME_MARKER_* constants
  //!\param[in] t_new new marker time
  void set_marker( const unsigned int marker,
                   const boost::posix_time::ptime& t_new );

  //!\brief set dt
  //!\param[in] dtflag one of MODELTIME_DT_* constant
  void set_dt( const unsigned int dtflag,
               const int days,
               const int seconds, 
               const int ms );

  //! \brief set dt
  //! \param[in] dtflag one of MODELTIME_DT_* constant
  //! \param[in] dt_new value to set
  void set_dt( const unsigned int dtflag,
               const boost::posix_time::time_duration& dt_new );

  //!\brief set current to beginning
  void restart_current()  { current = start; };
  
  //!\brief return timestep as a double in seconds (for calculations)
  double get_dt()              //!< timestep in seconds
    const { return 0.001 * dt.total_milliseconds(); };

  size_t get_n()  const { return n;  };
  size_t get_ns() const { return ns; };

  int get_year()  const { return current.date().year(); };
  int get_month() const { return current.date().month(); };
  int get_day()   const { return current.date().day(); };

  long get_hours()   const { return current.time_of_day().hours(); };
  long get_minutes() const { return current.time_of_day().minutes(); };
  long get_seconds() const { return current.time_of_day().seconds(); };
 
  long get_total_seconds() const
  { return current.time_of_day().total_seconds(); };
  long get_total_milliseconds() const
  { return current.time_of_day().total_milliseconds(); };
  long get_total_microseconds() const
  { return current.time_of_day().total_microseconds(); };

  //! Current time as an actual structure ptime
  boost::posix_time::ptime get_current_as_is() const
  { return current; };

  void get_current_as_timestamp( ModelTimeTypes::timestamp* ) const;
  void get_start_as_timestamp( ModelTimeTypes::timestamp* ) const;
  void get_finish_as_timestamp( ModelTimeTypes::timestamp* ) const;
  void get_save_as_timestamp( ModelTimeTypes::timestamp* ) const;

  void get_dt_as_dtstamp( ModelTimeTypes::dtstamp* ) const;
  void get_dts_as_dtstamp( ModelTimeTypes::dtstamp* ) const;

  //! \brief copying current time
  boost::posix_time::ptime get_current () const;

  //!\brief increase current by dt
  void increment() { current += dt; n++; };

  //!\brief check if we are still between start and fi
  bool is_done() const { return current >= finish; }

  //!\brief save increment
  void save_increment() { save += dts; ns++; };

  //!\brief save increment
  bool is_savetime() const { return current >= save; };

  void print() const;        //!< pretty print all fields 

private:

  boost::posix_time::ptime start;      //!< start time
  boost::posix_time::ptime finish;     //!< end time
  boost::posix_time::ptime current;    //!< current time
  boost::posix_time::ptime save;       //!< time for next save
  boost::posix_time::time_duration dt; //!< time step
  boost::posix_time::time_duration dts;//!< saving time step

  size_t n {0};                     //!< number of steps
  size_t ns {0};                    //!< number of savings

  // ---------------- private methods ---------------

  void get_as_timestamp( const boost::posix_time::ptime & t,
                       ModelTimeTypes::timestamp* pmts ) const;

  void get_as_dtstamp( const boost::posix_time::time_duration & dt,
                       ModelTimeTypes::dtstamp* pmts ) const;

};

#endif      /* MODELTIME_HH */
