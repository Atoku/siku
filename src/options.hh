/*
 * Siku: Discrete element method sea-ice model: options.hh
 *       Options reading, settings initializer
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
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

/*! \file options.hh
 *  \brief class Options to read command line
 */

#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <string>
using namespace std;

#include "siku.hh"

//! \brief Basic settings for the program as a whole
class Options
{
private:
  bool verbose {false};                 //!< Output flag
  string pythonfname {SIKU_DEFAULT_PYTHON};   //!< Python file name
  void usage();                         //!< Usage message
  void info();                          //!< Start info
  
public:
  //! \param argc Number of options (as it comes from main)
  //! \param argv Option sting (as it comes from main)
  Options( int argc, char* argv[] );
  
  //! \return Python config file name
  const string& get_pythonfname() { return pythonfname; };
  //! \return verbose flag
  bool is_verbose() { return verbose; };
};


#endif
