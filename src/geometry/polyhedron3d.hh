/*
 * polyhedron3d.hh
 *
 *  Created on: 22 марта 2016
 *      Author: gleb
 */

#ifndef POLYHEDRON3D_HH_
#define POLYHEDRON3D_HH_

#include <vector>

#include "segment3d.hh"
// #include chain:
// gtypes -- vec3d - pnt3d - mat3d - segment2d - polyhedron3d -- geometry
//        \- vec2d - pnt2d - segment2d - polygon2d --         -/


namespace Geometry
{

  class Polyhedron3d // rename to 'poly3d'
  {
  public:// should it be?
    struct vertex
    {
      vec3d pos;    // relative (to polyhedron origin) position
      vec3d norm;   // relative (to polyhedron) orientation (for graphics, etc)
      //~vertex() = default;
    };

    struct face
    {
      size_t v1, v2, v3;    // indexes of vertices
    };

  public:// should it be?
    pnt3d pos {};
    quat rot {};
    double sbb_r{};

    std::vector< vertex > verts;
    std::vector< face > hedra;

  public:
    //Polyhedron3d() = default;
    //~Polyhedron3d() = default;

    Polyhedron3d( const pnt3d& p, const quat& q, const double& r,
                  const std::vector< vertex >& vv,
                  const std::vector< face >& ff ) :
                    pos( p ), rot( q ), sbb_r( r )
    {
      verts = vv;
      hedra = ff;
    }
    Polyhedron3d( const std::vector< vec3d >& vv,
                  const std::vector< face >& ff );

  };

}



#endif /* POLYHEDRON3D_HH_ */
