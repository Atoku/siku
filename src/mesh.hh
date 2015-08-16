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
