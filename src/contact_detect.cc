/*
 * Siku: Discrete element method sea-ice model
 *
 * Copyright (C) UAF.
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

// we need some sorting!
#include <algorithm>

#include "globals.hh"
#include "contact_detect.hh"
#include "siku.hh"
using namespace Coordinates;
using namespace Geometry;

// ~~~~~~~~~~~~~~~~~~~~~ predeclarations  and inlines ~~~~~~~~~~~~~~~~~~~~~~~

inline bool el_pointers_x_compare( Element* pe1, Element* pe2 );

void add_cont( Globals& siku, const size_t& i1, const size_t& i2,
               const int& t );

// merge lists of old and new contacts
void merge_contacts( vector<ContactDetector::Contact>& olds,
                     const vector<ContactDetector::Contact>& news );

void _freeze( ContactDetector::Contact& c, Globals& siku, double tol );
void _share( ContactDetector::Contact& c, Globals& siku, double tol );
void _dist_freeze( ContactDetector::Contact& c, Globals& siku, double tol );

void _select_freeze( std::vector<ContactDetector::Contact>& cont,
                     Globals& siku, const double& tol );

inline double _sqr( const double& x ) { return x*x; }

inline double _dist2( const vec3d& v1, const vec3d& v2 )
{
  vec3d v = v2 - v1;
  return dot( v, v );
}

// =============================== Methods ==================================

// For sorting watch
// https://s-media-cache-ak0.pinimg.com/originals/5f/fc/42/5ffc4224b938d1fb0abee887e4add84b.jpg
// Yet simple 'sort' from <algorithm> is used.

double ContactDetector::Contact::find_edges( Globals& siku )
{
  if( !i1 && !i2 ) throw("bad element indexes in contact");
    //return 0.; // if no elements were set

  //search for mutual edge:

  Element& e1 = siku.es[i1], & e2 = siku.es[i2];  // shortcut

  // variables
  mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
  mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

  // polygons in local (e1) coords
  vector<vec2d> loc_P1;         // e1.P vertices in local 2D coords
  vector<vec2d> loc_P2;         // e2.P vertices in local 2D coords
  for( auto& p : e1.P ) loc_P1.push_back( vec3_to_vec2( p ) );
  for( auto& p : e2.P ) loc_P2.push_back( vec3_to_vec2( e2_to_e1 * p ) );

//  vector<vec3d> loc_P1;         // e1.P vertices in local 2D coords
//  vector<vec3d> loc_P2;         // e2.P vertices in local 2D coords
//  for( auto& p : e1.P ) loc_P1.push_back( p );
//  for( auto& p : e2.P ) loc_P2.push_back( e2_to_e1 * p );

  v11 = 0, v22 = 0, v12 = 0, v21 = 0;    // indexes in e1 and e2.
  size_t s1 = loc_P1.size(), s2 = loc_P2.size();    // sizes of arrays
  double d, mind = abs( loc_P1[0] - loc_P2[0] );    // minimal distance


  // search for two closest vertices
  for( size_t i = 0; i < s1; i++ )
    for( size_t j = 0; j < s2; j++ )
      if( ( d = abs( loc_P1[ i%s1 ] - loc_P2[ j%s2 ] ) ) < mind )
        {
          mind = d;
          v11 = i;
          v22 = j;
        }

  // test which of neighbor vertices are second closest pair
  size_t v1l = (v11-1+s1) %s1,
         v2l = (v22+1)    %s2,
         v1r = (v11+1)    %s1,
         v2r = (v22-1+s2) %s2;
  if( abs( loc_P1[ v1l ] - loc_P2[ v2l ] ) <
      abs( loc_P1[ v1r ] - loc_P2[ v2r ] ) )
    {
      v12 = v1l;
      v21 = v2l;
    }
  else
    {
      v12 = v1r;
      v21 = v2r;
    }

  // return - mean length of two edges of elements
  return 0.5 * ( abs( loc_P1[v11] - loc_P1[v12] )
               + abs( loc_P2[v22] - loc_P2[v21] ) );

}

////OLD
//double ContactDetector::Contact::find_edges_( Globals& siku )
//{
//  if( !i1 && !i2 ) throw("bad element indexes in contact");
//    //return 0.; // if no elements were set
//
//  //search for mutual edge
//  Element& e1 = siku.es[i1], & e2 = siku.es[i2];
//
//  bool first_found = false, second_found = false;
//  double l1 =0, l2 =0;
//
//  mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
//  mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );
//  double tol { 100. }; // 100 - heuristic tolerance
//  vec2d dump, r12 = vec3_to_vec2( e2_to_e1 * NORTH ) * tol * tol,
//              r21 = vec3_to_vec2( e1_to_e2 * NORTH ) * tol * tol;
//
//  vec2d tv1 = vec3_to_vec2( e1.P.back() ) * tol;
//  vec2d tv2 = {};
//
//  for( size_t i = 0; i< e1.P.size(); i++)
//    {
//      tv2 = tv1;
//      tv1 = vec3_to_vec2( e1.P[i] ) * tol;
//
//      if( segment2d_intersect( {0.,0.}, r12, tv1, tv2, dump ) && tv1 != tv2 )
//        {
//          l1 = (tv2 - tv1).abs();
//          v11 = ( i + e1.P.size() - 1 ) % e1.P.size(); // i-1`st
//          v12 = i;
//          first_found = true;
//          break;
//        }
//    }
//
//  tv1 = vec3_to_vec2( e2.P.back() ) * tol;
//  for( size_t i = 0; i< e2.P.size(); i++)
//    {
//      tv2 = tv1;
//      tv1 = vec3_to_vec2( e2.P[i] ) * tol;
//
//      if( segment2d_intersect( {0.,0.}, r21, tv1, tv2, dump ) && tv1 != tv2 )
//        {
//          l2 = (tv2 - tv1).abs();
//          v21 = ( i + e2.P.size() - 1 ) % e2.P.size(); // i-1`st
//          v22 = i;
//          second_found = true;
//          break;
//        }
//    }
//
//  if( first_found && second_found )
//    return ( l1 + l2 ) * 0.5;
//  else
//    {
//      cout<<r21<<"---\n";
//      for( auto& a : e2.P )
//        print( vec3_to_vec2( a ) );
//      cout<<e2.A<<endl;
//
//      cout<<"\n"<<r12<<"---\n";
//      for( auto& a : e1.P )
//        print( vec3_to_vec2( a ) );
//      cout<<e1.A<<endl;
//
//      if( !first_found ) throw("first element`s edge not found");
//      if( !second_found) throw("second element`s edge not found");
//    }
//
//
//  return 0.;
//}

//---------------------------------------------------------------------

// IMPROVE: remove code duplication in 'find_pairs'
void ContactDetector::sweep_n_prune( Globals& siku )
{
  // preparing additional vector:
  // setting pointers in supporting vector (for sortings, etc)
  siku.pes.clear();
  for( size_t i = 0; i < siku.es.size(); ++i )
    siku.pes.push_back( &siku.es[i] );

  // sorting (by left-most point of bounding sphere)
  std::sort( siku.pes.begin(), siku.pes.end(), el_pointers_x_compare );

  // IMPROVE: need to create empty vector of instantly large capacity
  static std::vector<ContactDetector::Contact> news;

  // contact search
  for ( size_t i = 0; i < siku.pes.size () - 1; ++i )
    {
      // IMPROVE: reconsider error checks!
//      if( siku.pes[i]->flag & Element::F_ERRORED ) continue;

      for ( size_t j = i + 1; j < siku.pes.size (); ++j )
        {
//          if( siku.pes[j]->flag & Element::F_ERRORED ) continue;

          if ( _dist2( siku.pes[i]->Glob, siku.pes[j]->Glob ) <
              _sqr( siku.pes[i]->sbb_rmin + siku.pes[j]->sbb_rmin ) )
            {
              news.push_back( ContactDetector::Contact( siku.pes[i]->id,
                    siku.pes[j]->id, siku.time.get_n(), NONE ) );
            }

          // should not this be placed in loop condition check?
          if ( (siku.pes[i]->Glob.x + siku.pes[i]->sbb_rmin) <
              (siku.pes[j]->Glob.x - siku.pes[j]->sbb_rmin) )

              break;

        }
    }

  std::sort( news.begin(), news.end() );

  merge_contacts( siku.ConDet.cont, news );

  news.clear();  // should be O(1) without deallocation
}

//---------------------------------------------------------------------

void ContactDetector::find_pairs( Globals& siku )
{
  // IMPROVE: need to create empty vector of instantly large capacity
  static std::vector<ContactDetector::Contact> news;
  news.clear();

  for ( size_t i = 0; i < siku.es.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.es.size (); ++j )
        {
          if ( _dist2( siku.es[i].Glob, siku.es[j].Glob ) <
              _sqr( siku.es[i].sbb_rmin + siku.es[j].sbb_rmin ) )
//            add_cont( siku, i, j, siku.time.get_n() );
            news.push_back( ContactDetector::Contact( siku.es[i].id,
                    siku.es[j].id, siku.time.get_n(), NONE ) );
        }
    }

  std::sort( news.begin(), news.end() );

  merge_contacts( siku.ConDet.cont, news );

  news.clear();  // should be O(1) without deallocation
}

// --------------------------------------------------------------------------

void ContactDetector::clear()
{

//  cont.clear();

  static std::vector<ContactDetector::Contact> temp;
  temp.clear();

  for( auto& a : cont )
    {
      if( a.type == JOINT && a.durability > 0.05 )
        temp.push_back( a );
    }

  temp.swap( cont );

//// OLD algorithm, designed for storing contact history
//  size_t size = cont.size();
//  for( size_t i = 0; i < size; ++i )
//    {
//      if( cont[i].type == ContType::JOINT )  // joints untouched
//        {
//          if( cont[i].durability < 0.05 )  // destruction
//            {
//
//
////              cont[i].type = ContType::COLLISION;
////              cont[i].generation = 0;
//              cont[i] = cont[--size];  // replacing current with last
//              cont.pop_back();         // and deleting last
//            }
//        }
//      else  // deleting contact
//        {
//          cont[i] = cont[--size];  // replacing current with last
//          cont.pop_back();         // and deleting last
////          if( cont[i].generation > 1 )  // IMPROVE
////            {
////              cont[i] = cont[--size];  // replacing current with last
////              cont.pop_back();         // and deleting last
////            }
////          else
////            cont[i].generation++;
//        }
//    }
//
//  // sorting of remains. Required by contact_exist check
//  std::sort( cont.begin(), cont.end() );
}

//---------------------------------------------------------------------

void ContactDetector::detect( Globals& siku )
{
  // elements amount check: less then 2 - contacts are impossible
  if( siku.es.size() < 2 || !is_detect_time( siku ) )
    return;

  // smart clearing: joints remain untouched
  clear();

  switch( det_meth )
  {
    case CONTACTS_N2:
      find_pairs( siku );
      break;
    case SWEEP_N_PRUNE:
      sweep_n_prune( siku );
      break;
  }

}

// --------------------------------------------------------------------------

void ContactDetector::freeze( Globals& siku, double tol )
{
  // everything must be cleared
  cont.clear();

  // preparatory detection
  detect( siku );

  // actual freezing
  _select_freeze( cont, siku, tol );

}

// --------------------------------------------------------------------------

void ContactDetector::freeze_links( Globals& siku )
{
  cont.clear();

  for( auto& a : links )
    cont.push_back( Contact( a.i1, a.i2, 0, JOINT ) );

  std::sort( cont.begin(), cont.end() );

//  for( auto& c : cont )
//    _freeze( c, siku, 0.1 );
  _select_freeze( cont, siku, 0.1 );

}

// --------------------------------------------------------------------------

bool ContactDetector::is_detect_time( Globals& siku )
{
  switch( det_freq_t )
  {
    case 0:  // always
      //return true; // same as default
      break;

    case 1:  // by ticks
      if( siku.time.get_n() > size_t( det_last + det_value ) )
        {
          det_last = siku.time.get_n();
          return true;
        }
      return false;
      break;

    case 2:  // by seconds
      if( (double)siku.time.get_total_microseconds() * 0.000001 >
          det_last + det_value )
        {
          det_last = (double)siku.time.get_total_microseconds() * 0.000001;
          return true;
        }
      return false;
      break;

    case 3:  // by speed (automatic)

      // searching max p speed
      double maxs = 0;
      for( auto& e : siku.es )
        if( abs2( e.V ) > maxs ) maxs = abs2( e.V );
      maxs = sqrt( maxs );

      det_last += siku.time.get_dt() * maxs;  // accumulate displacement

      if( det_last > det_value )
        {
          det_last = 0.;
          return true;
        }
      return false;
      break;
  }
  return true;
}

// ============================ Local utilities =============================

// comparator for sorting algorithm
inline bool elements_x_compare( const Element& e1, const Element& e2 )
{
  /*
   * TODO: i think we should calculate xmin and xmax on update phase and store
   * inside each element, rather then calculate several times in one check!
   */
  return (e1.Glob.x - e1.sbb_rmin) < (e2.Glob.x - e2.sbb_rmin);
}

// --------------------------------------------------------------------------

// pointer comparator for sorting algorithm
inline bool el_pointers_x_compare( Element* pe1, Element* pe2 )
{
  return elements_x_compare( *pe1, * pe2 );
  //return (pe1->Glob.x - pe1->sbb_rmin) < (pe2->Glob.x - pe2->sbb_rmin);
}

// --------------------------------------------------------------------------

void merge_contacts( vector<ContactDetector::Contact>& olds,
                     const vector<ContactDetector::Contact>& news )
{
  static std::vector<ContactDetector::Contact> temp;
  temp.clear();
  size_t oi = 0, ni = 0;

  while( oi < olds.size() && ni < news.size() )
    {
      if( news[ni] < olds[oi] )
        temp.push_back( news[ni++] );
      else if( olds[oi] < news[ni] )
        temp.push_back( olds[oi++] );
      else // <=> equal contacts
        {
          temp.push_back( olds[oi++] );
          ni++;
        }
    }

  while( oi < olds.size() )
    temp.push_back( olds[oi++] );
  while( ni < news.size() )
    temp.push_back( news[ni++] );

  temp.swap( olds );  // now siku.ConDet.conts must be refreshed list

  temp.clear();  // if all is correct - size will be 0, but no deallocation
                 // would be called
}

// --------------------------------------------------------------------------

// quick check if such contact already exists. Assume that contacts list is
// sorted with 'cont_compare'
bool contact_exists( const ContactDetector& CD,
                     const size_t& i1, const size_t& i2 )
{
  size_t i = 0;

  // search first contact with matching i1
  if( CD.cont.size() )  // if list exists at all
    while( CD.cont[i].i1 < i1 )
      if( ++i >= CD.cont.size() )  return false;

  // check if any contact matches by both indexes
  for(; i < CD.cont.size() && CD.cont[i].i1 == i1; ++i )
    if( CD.cont[i].i2 == i2 )  return true;

  return false;
}

// --------------------------------------------------------------------------

// add new contact to list with type checks (deprecated)
void add_cont( Globals& siku, const size_t& i1, const size_t& i2, const int& t )
{
  if( !contact_exists( siku.ConDet, i1, i2 ) )
    {
      siku.ConDet.cont.push_back ( ContactDetector::Contact ( i1, i2, t ) );
    }
}

// --------------------------------------------------------------------------

void _select_freeze( std::vector<ContactDetector::Contact>& cont,
                     Globals& siku, const double& tol )
{
  switch( siku.cont_force_model )
  {
    case CF_DEFAULT : // same as CF_TEST_SPRINGS
      for( auto& c : cont )
        _freeze( c, siku, tol );
      break;

    case CF_HOPKINS :
      for( auto& c : cont )
        _dist_freeze( c, siku, tol );
        //_freeze( c, siku, tol );
        //_share( c, siku, tol );   //outdated
      break;

    case CF_DIST_SPRINGS :
      for( auto& c : cont )
        _dist_freeze( c, siku, tol );
      break;
  }
}

// --------------------------------------------------------------------------

//using namespace BG;
// perform freezing on two elements by contact
// implementation copied from 'contact_force'
// TODO: move to 'geometric' module
void _freeze( ContactDetector::Contact& c, Globals& siku, double tol )
{
  Element &e1 = siku.es[c.i1], &e2 = siku.es[c.i2];
  int ires;  // !static? temporal variable to store geometry results
  vec2d center; // !static
  double size;  // !static? temporal dump
  std::vector<vec2d> loc_P1;  // e1.P vertices in local coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local coords
  std::vector<vec2d> dump;
  std::vector<PointStatus> ps;

  mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
      // !static
  mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );
      // !static

  //vec2d r2 = - vec3_to_vec2( dest_to_src * NORTH ); //TODO: select correct
  vec2d r12 = vec3_to_vec2( e2_to_e1 * NORTH );

  for( auto& p : e1.P )
    loc_P1.push_back( vec3_to_vec2( p ) * (1.+tol) );
  for( auto& p : e2.P )
    loc_P2.push_back( r12 +
         ( vec3_to_vec2( e2_to_e1 * p ) - r12 ) * (1.+tol) );

  if( Geometry::intersect( loc_P1, loc_P2 , dump, &ps, &center, &size ) )
      //&& count() ) // corner intersections are ignored
    {
      c.type = ContType::JOINT;

      c.p1 = c.p2 = center;
      c.p3 = c.p4 = vec3_to_vec2( e1_to_e2 * vec2_to_vec3( center ) );

      c.durability = 1.;

      //deprecated
      //if( dump.size() > 2 ) c.init_len = size;  // only if size is area

      //search for length of original mutual edge
      c.init_wid = c.find_edges( siku );

      // if no edges such edges detected: L = S / Re, Re - radius of equivalent
      // circle of geometric mean of areas
      if( !c.init_wid && dump.size() > 2 )
        c.init_wid = size * sqrt( M_PI / sqrt( e1.A * e2.A ) );

      // init size - sum of distances between contact end elements` centers
      c.init_len = abs( c.p1 ) + abs( c.p3 );
    }
}

// --------------------------------------------------------------------------

// Outdated
void _share( ContactDetector::Contact& c, Globals& siku, double tol )
{
  Element& e1 = siku.es[c.i1];
  Element& e2 = siku.es[c.i2];

  static const double tolerance = 0.01; // TODO: clean this

  for( size_t i = 0; i < e1.P.size(); ++i )
    for( size_t j = 0; j < e2.P.size(); ++j )
      if( abs2(e1.P[i]-e2.P[j]) < tolerance )
        {
          size_t jm1 = ( j + e2.P.size() - 1 ) % e2.P.size(); // j-1 mod size
          size_t ip1 = (i+1) % e1.P.size();

          //cout<<"prejoint "<<i<<"-"<<ip1<<":"<<e1.P.size()
          //    <<" | "<<j<<"-"<<jm1<<":"<<e2.P.size()<<endl;
          if( abs2(e1.P[ ip1 ] - e2.P[ jm1 ]) < tolerance )
            {
              //cout<<"joint"<<endl;
              c.v11 = i;
              c.v12 = ip1;
              c.v21 = jm1;
              c.v22 = j;

              c.type = JOINT;
              c.step = siku.time.get_n();
              c.durability = 1.;

              return;
            }
        }

}

// --------------------------------------------------------------------------

void _dist_freeze( ContactDetector::Contact& c, Globals& siku, double tol )
{
  Element &e1 = siku.es[c.i1], &e2 = siku.es[c.i2];

  vec2d center;
  double size;
  std::vector<vec2d> loc_P1;  // e1.P vertices in local coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local coords
  std::vector<vec2d> dump;
  std::vector<PointStatus> ps;

  mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
  mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

  //OR vec2d r2 = - vec3_to_vec2( e1_to_e2 * NORTH );
  vec2d r12 = vec3_to_vec2( e2_to_e1 * NORTH );

  // just-for-sure check
  if( c.find_edges( siku ) )
    {
      // IMPROVE: rewrite this with adequate denotations
      // important vertices in 2d relied to e1
      vec2d t11 = vec3_to_vec2( e1.P[c.v11] ),
            t12 = vec3_to_vec2( e1.P[c.v12] ),
            t21 = vec3_to_vec2( e2_to_e1 * e2.P[c.v21] ),
            t22 = vec3_to_vec2( e2_to_e1 * e2.P[c.v22] );

      //OLD: if edges are close enough
//      if( segment2d_distance( t11, t12, t21, t22 ) < abs( r12 ) * tol )
      //
      //if vertices are close enough (relatively to edges length)
      if( ( abs(t11-t22) + abs(t12-t21) ) <
          ( abs(t11-t12) + abs(t21-t22) ) * tol )
        {
          c.type = ContType::JOINT;

          vec2d c1 = ( t11 + t22 ) * 0.5;
          vec2d c2 = ( t12 + t21 ) * 0.5;

          // points of contact in two coord systems
          c.p1 = c1;
          c.p2 = c2;
          vec3d tv1 = { c1.x, c1.y, sqrt( 1. - c1.x*c1.x - c1.y*c1.y ) };
          vec3d tv2 = { c2.x, c2.y, sqrt( 1. - c2.x*c2.x - c2.y*c2.y ) };
          c.p3 = vec3_to_vec2( e1_to_e2 * tv2 );
          c.p4 = vec3_to_vec2( e1_to_e2 * tv1 );

          c.init_wid = abs( c1 - c2 );  // initial width is distance between
                                        // 'springs'

          // initial length is a sum of distances between centers of elements
          // and contact zone (0.5 factored out of vectors averaging)
          c.init_len = ( abs(c.p1 + c.p2) + abs(c.p3 + c.p4) )*0.5;

          c.durability = 1.;
        }
    }
  else // in case of broken contact edge detection
    {
      cout<<"TEST ERROR IN _dist_freeze"<<endl;
      cout<<"elements: "<<c.i1<<", "<<c.i2<<endl;
      cout<<"dist: "<< c.find_edges(siku)<<endl;
      cout<<"vert inds: "<<c.v11<<", "<<c.v12<<"; "<<c.v21<<", "<<c.v22<<endl;
      cout<<"points:"<<endl
          <<siku.es[c.i1].P[c.v11]<<endl
          <<siku.es[c.i1].P[c.v12]<<endl
          <<siku.es[c.i2].P[c.v21]<<endl
          <<siku.es[c.i2].P[c.v22]<<endl;
      cin.get();
      throw;
    }
}

// --------------------------------------------------------------------------
