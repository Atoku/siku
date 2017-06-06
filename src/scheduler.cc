/*
 * Siku: Discrete element method sea-ice model: description.hh
 *
 * Copyright (C) UAF
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

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
