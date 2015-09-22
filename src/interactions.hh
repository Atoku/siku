/*
 * Siku: Discrete element method sea-ice model: element.hh
 *       Ice Element class: ice element representation and methods
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

#ifndef INTERACTION_HH
#define INTERACTION_HH

#include "siku.hh"
#include "globals.hh"
#include "element.hh"
#include "coordinates.hh"

class Interactor
{
  static const vec3d NORTH;

public:

  static void find_pairs( Globals& siku );

};

#endif
