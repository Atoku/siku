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

#ifndef CONTACTFORCE_HH
#define CONTACTFORCE_HH

#include "siku.hh"
#include "element.hh"
#include "globals.hh"
#include "contact_detect.hh"
#include "coordinates.hh"

// Coefficient of restitution
#define k_rest 0.0001
#define k_torq 0.0000001

// some polygon intersections: http://www.angusj.com/delphi/clipper.php
// http://geomalgorithms.com/
// http://geomalgorithms.com/a09-_intersect-3.html
// http://www.cs.man.ac.uk/~toby/alan/software/gpc.html

//! Currently using boost::geometry

//! Function for calculating two elements interaction. Changes both elements
//! so should be used once per pair of elements.
void contact_push( Element& e1, Element& e2, Globals& siku );

#endif  /*CONTACTFORCE_HH*/
