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

  \file mesh.hh

  \brief Set of points in a region, mostly for diagnostics.

*/

#ifndef MESH_HH
#define MESH_HH

#include <vector>

#include "siku.hh"

//! \brief stores set of 3D points of extrinsic normalized coordinates
//! (ENC) for points to use in diagnostics
class Mesh
{
 public:
  std::vector<vec3d> data;

};

#endif      /* MESH_HH */
