/*
 * polyhedron.cc
 *
 *  Created on: 22 марта 2016
 *      Author: gleb
 */

#include "polyhedron3d.hh"

namespace Geometry
{

  Polyhedron3d::Polyhedron3d( const std::vector< vec3d >& vv,
                              const std::vector< face >& ff )
  {
    // local struct for generating normals in vertices
    struct _node
    {
      std::vector< size_t > inds;

      // adding unique edges in ordered array
      void add_edge( const size_t& _i )
      {
        for( size_t i = 0; i < inds.size(); i++ )
          if( inds[i] < _i &&
              ( i == inds.size()-1 ||
                  ( i < inds.size()-1 && inds[i+1] > _i  )
              )
            )
            {
              inds.insert( inds.begin() + i + 1, _i );
              break;
            }
      }

      // generating 'mean' normal
      vec3d gen_norm()
      {
        vec3d _t {};
        for( auto& a : inds )
          _t += a;
        _t /= (double) inds.size();
        return _t / abs( _t );
      }
    };


    // temporal nodes and normals of sides
    std::vector< _node > nodes;
    std::vector< vec3d > normals;

    nodes.resize( vv.size() );
    normals.resize( ff.size() );
    for( size_t i = 0; i < ff.size(); i++ )
      {
        auto& f = ff[i];

        // generating current _node
        nodes[f.v1].add_edge( f.v2 );
        nodes[f.v1].add_edge( f.v3 );
        nodes[f.v2].add_edge( f.v1 );
        nodes[f.v2].add_edge( f.v3 );
        nodes[f.v3].add_edge( f.v1 );
        nodes[f.v3].add_edge( f.v2 );

        //generating current normal
        vec3d v1 = vv[f.v2] - vv[f.v1],
              v2 = vv[f.v3] - vv[f.v1];

        normals[i] = cross( v1, v2 );
      }

    // finding origin = mean value of vertices
    for( auto& v : vv )
      pos += v;
    pos /= (double) vv.size();

    // generating vertices - related positions and normals
    verts.resize( vv.size() );
    sbb_r = 0.;
    for( size_t i = 0; i < vv.size(); i++ )
      {
        vec3d tv = vv[i] - pos;
        double td = abs( tv );

        verts[i] = { tv, nodes[i].gen_norm() };

        if( td > sbb_r )  sbb_r = td;  // searching for the most distant vertex
      }
    sbb_r = sqrt( sbb_r );  // bounding sphere radius

  }

}


