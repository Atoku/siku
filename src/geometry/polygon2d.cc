/*!

  \file polygon2d.cc

  \brief polygon2d  library implementation

*/

#include "polygon2d.hh"

#include <algorithm>

// ~~~~~~~~~~~~~~~~~~~~~~~~~ local utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool operator< (const Geometry::vec2d& v1, const Geometry::vec2d& v2 )
{
  return v1.x < v2.x || ( v1.x == v2.x && v1.y < v2.y );
}

namespace Geometry
{

// ~~~~~~~~~~~~~~~~~~~~~~~~~ external functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  int intersect( const cvpoly2d& poly1, const cvpoly2d& poly2,
                  std::vector<pnt2d>& verts,
                  std::vector<PointStatus>* pflags,
                  pnt2d* pcen, double* psize )
  {
    vector<pnt2d> tempVerts;
    vector<PointStatus> tempFlags;
    pnt2d a1, a2, b1, b2, tp;
    vec2d tv1, tv2, tv3 = vec2d();// nullvec2d;
    size_t n, s;
    double* prods;

    // search for inner points
    for( size_t i = 0; i < poly1.verts.size(); ++i )
      if( poly2.contains( poly1.verts[i] ) )
        {
          add_point( poly1.verts[i], tempVerts );
          tempFlags.push_back( PointStatus::VERTEX );
        }

    for( size_t i = 0; i < poly2.verts.size(); ++i )
      if( poly1.contains( poly2.verts[i] ) )
        {
          add_point( poly2.verts[i], tempVerts );
          tempFlags.push_back( PointStatus::VERTEX );
        }

    // edges` intersections
    for( size_t i = 0; i < poly1.verts.size(); ++i )
      {
        a1 = poly1.verts[ i ];
        a2 = poly1.verts[ (i+1) % poly1.verts.size() ];
        for( size_t j = 0; j < poly2.verts.size(); ++j )
          {
            b1 = poly2.verts[ j ];
            b2 = poly2.verts[ (j+1) % poly2.verts.size() ];

            if( segment2d_intersect( a1, a2, b1, b2, tp ) )// vec=pnt
              {
                add_point( tp, tempVerts );
                tempFlags.push_back( PointStatus::EDGE );
              }
          }
      }

    s = tempVerts.size();

    //----------------------------------

    // no intersections
    if( s == 0 ) return 0;

    // single point touch
    if( s == 1 )
      {
        if( psize )  *psize = 0.;
        if( pcen )  *pcen = tempVerts[ 0 ];
        return 1;
      }

    // line intersection
    if( s == 2 )
      {
        if( psize )  *psize = (tempVerts[ 1 ] - tempVerts[ 0 ]).abs();
        if( pcen )  *pcen =  (tempVerts[ 1 ] + tempVerts[ 0 ]) / 2.;// vec=pnt
        return 2;
      }

    if( s > 2 )
      {
        tv1 = vec2d();//nullvec2d;

        for ( size_t i = 0; i < s; ++i )
          tv1 += tempVerts[i];// vec=pnt
        tp = tv1 / (double)tempVerts.size();

        // calculating cross-products <~> phi = atan( y/x )
        prods = new double[s];
        //tv1 = tempVerts[0] - tp;
        for ( size_t i = 0; i < s; ++i )
          {
            tv2 = tempVerts[i] - tp;
            prods[i] = atan2( tv2.gety(), tv2.getx() );
          }

        // forming the 'result' vector of points
        verts.clear();
        if( pflags )  (*pflags).clear();
        while ( s )
          {
            n = 0;
            for ( size_t i = 0; i < s; ++i )  // search for min prod
              if ( prods[i] < prods[n] )
                n = i;

            // moving matching vertex into 'verts'
            verts.push_back ( tempVerts[n] );
            tempVerts[n] = tempVerts.back ();
            tempVerts.pop_back ();
            if( pflags )
              {
                (*pflags).push_back( tempFlags[n] );
                tempFlags[n] = tempFlags.back ();
                tempFlags.pop_back ();
              }
            prods[n] = prods[s - 1];
            --s;
          }

        delete[] prods;

        tp = (verts)[0];
        tv2 = (verts)[1] - tp;

        // area calculation (triangulation method)
        if( psize )
          {
            *psize = 0.;
            for(size_t i = 2; i < verts.size(); ++i )
              {
                tv1 = tv2;
                tv2 = (verts)[i] - tp;

                *psize += cross( tv1, tv2 );
              }
            *psize /= 2.;
          }
        tv2 = (verts)[1] - tp;

        // centroid
        // triangulation method [source: http://e-maxx.ru/algo/gravity_center]
        if( psize && pcen )
          {
            for(size_t i = 2; i < verts.size(); ++i )
              {
                tv1 = tv2;
                tv2 = (verts)[i] - tp;

                // (tv1 + tv2 + 0)/3  *  [tv1 x tv2]/2
                tv3 += ( tv1 + tv2 ) * cross( tv1 , tv2 ) / 6. ;
              }
            *pcen = tp + tv3 / *psize;
          }

        return verts.size();
      }

    return 0;
  }

// --------------------------------------------------------------------------

  vector<vec2d> convex_hull( vector<vec2d> P )
  {
      int n = P.size(), k = 0;
      vector<vec2d> H( 2*n );

      // Sort points lexicographically
      sort( P.begin(), P.end() );

      // Build lower hull
      for ( int i = 0; i < n; ++i )
        {
          while ( k >= 2 && cross( H[k-1] - H[k-2], P[i] - H[k-2] ) <= 0 )
            k--;
          H[k++] = P[i];
        }

      // Build upper hull
      for ( int i = n-2, t = k+1; i >= 0; i-- )
        {
          while ( k >= t && cross( H[k-1] - H[k-2], P[i] - H[k-2] ) <= 0)
            k--;
          H[k++] = P[i];
        }

      H.resize(k);
      return H;
  }

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ConvexPoly2d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // returns polygon center. Optional parameter 'a' - area of polygon, gives
  // almost twice speed-up of calculation
  pnt2d cvpoly2d::center( double a ) const
  {
    // default check
    if( verts.size() < 3 ) return pnt2d( vec2d() );//nullvec2d

    if( ! a )  a = area();  // if no arg is passed

    pnt2d o = verts[0];
    vec2d tv1, tv2 = verts[1] - o, cen = vec2d();//nullvec2d;

    // triangulation method [source: http://e-maxx.ru/algo/gravity_center]
    for(size_t i = 2; i < verts.size(); ++i )
      {
        tv1 = tv2;
        tv2 = verts[i] - o;

        // (tv1 + tv2 + 0)/3  *  [tv1 x tv2]/2
        cen += ( tv1 + tv2 ) * cross( tv1 , tv2 ) / 6. ;
      }

    return o + cen / a;
  }

// --------------------------------------------------------------------------

  // returns area of polygon. Should also work for non-convex polygons.
  double cvpoly2d::area() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    double a = 0.;
    pnt2d o = verts[0];

    // triangulation method
    for(size_t i = 1; i < verts.size() - 1; ++i )
      a += cross( verts[i] - o, verts[i+1] - o );

    return a / 2.;
  }

// --------------------------------------------------------------------------

  // returns a moment of inertia of convex polygon
  double cvpoly2d::mom_of_ine() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    pnt2d o = center();
    vec2d tv1, tv2;
    double res = 0., S = 0., a = area();

    for(size_t i = 0; i < size; ++i )
      S += abs( cross( verts[i] - o, verts[ (i+1)%size ] - o ) );

    // source: wiki
    // TODO: check
    for(size_t i = 0; i < size; ++i )
      {
        tv1 = verts[i] - o;  // aka Pi
        tv2 = verts[ (i+1)%size ] - o;  // aka Pi+1

        // aka:     |Pi x Pi+1|  * ( Pi ^ 2  + Pi*Pi+1 + Pi+1 ^2 )
        res += cross( tv1, tv2 ) * ( tv1*tv1 + tv1*tv2 + tv2*tv2 );
      }

    return res * a / ( 6. * S );
  }

// --------------------------------------------------------------------------

  // check if polygon is convex
  bool cvpoly2d::is_convex() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    vec2d tv1 = verts[ 1 ] - verts[ 0 ];
    vec2d tv2 = verts[ 2 ] - verts[ 1 ];
    double td1 = cross( tv1, tv2 );
    double td2;

    for(size_t i = 1; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = verts[ (i+2) % size ] - verts[ (i+1) % size ];

        // angle between edges (only sign matters) (TOLERANCE?)
        td2 = cross( tv1, tv2 );

        // UNDONE: this section contains hardcoded tolerance. It should be
        // rewritten
        static const double tolerance = -1e-12;
        if( td2 * td1 < tolerance ) return false;  // if different signs - not convex

        td1 = td2;
      }

    return true;

//    //test // BUG
//    return verts.size() == convex_hull( verts ).size();
  }

// --------------------------------------------------------------------------

  // check if vertices are given in counter-clockwise order for convex polygons
  bool cvpoly2d::is_CCW_oriented() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    pnt2d o = center();
    vec2d tv1, tv2 = verts[0] - o;

    for(size_t i = 0; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = verts[ (i+1) % size ] - o;

        // if any next vertex is located CW ( + tolerance? )from current one
        if( cross( tv1, tv2 ) < 0.0 ) return false;
      }

    return true;
  }

// --------------------------------------------------------------------------

  // slower check if vertices are given in counter-clockwise order for any kind
  // of polygons
  bool cvpoly2d::is_CCW_oriented_NC() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    vec2d tv1;
    vec2d tv2 = ort( verts[1] - verts[0] );
    double res = 0.;

    for(size_t i = 0; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = ort( verts[ (i+2) % size ] - verts[ (i+1) % size ] );

        // accumulate angle between edges
        // BUG: should be replaced with something like atan
        res += atan( cross( tv1, tv2 ) / dot( tv1, tv2 ) );
      }

    return res > 0.;
  }

// --------------------------------------------------------------------------

  //! \brief Checks if the polygon contains a pnt2d. !! CCW polygons only
  bool cvpoly2d::contains( const pnt2d& point ) const
  {
    vec2d PP;
    vec2d PO;
    //Vector2d OP;
    double res;

    int size = verts.size ();
    for ( int i = 0; i < size; ++i )
      {
        PP = verts[( i + 1 ) % size] - verts[i];
        PO = point - verts[i];
        //OP = verts[( i + 1 ) % size] - point;
        res = cross ( PP, PO );

        // CCW rightwards check (inclusive)
        if ( res < 0 )
          return false;
      }
    return true;
  }

//---------------------------------------------------------------------
  
  bool cvpoly2d::intersect( const cvpoly2d& P, const cvpoly2d& Q )
  {
    // clear the content of the current polygon
    verts.clear();

    // we a


    return true;
  }

}