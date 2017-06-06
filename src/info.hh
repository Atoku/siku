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

  \file info.hh

  \brief General information about the model (usually restored from
  siku.info dictionary of configuration file)

*/

#ifndef INFO_HH
#define INFO_HH

#include <string>
using namespace std;

//! \brief General information about the model and current run
struct Info
{
  string name;                  //!< program name
  string brief;                 //!< description
  string version;               //!< version string
  string date;                  //!< date when program created
  string rundate;               //!< date when program runs
};

#endif      /* INFO_HH */
