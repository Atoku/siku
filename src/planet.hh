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

  \file planet.hh

  \brief Planet (Earth) parameters. They are read from siku module

*/

#ifndef PLANET_HH
#define PLANET_HH

//! \brief Default is Earth'r radius in meters 
const double PLANET_R_DEAFAULT = 6371000.0;//<- wiki says, 6353000.0;

//! \brief Structure for constants related to Planet 
struct Planet
{
  //! \brief Radius
  double R { PLANET_R_DEAFAULT };
  //! \brief Reciprocal of radius
  double R_rec { 1.0 / PLANET_R_DEAFAULT };

  //! \brief Radius squared
  double R2 { PLANET_R_DEAFAULT * PLANET_R_DEAFAULT };
  //! \brief Reciprocal of  squared radius
  double R2_rec { 1.0 / ( PLANET_R_DEAFAULT * PLANET_R_DEAFAULT ) };
};

#endif      /* PLANET_HH */

