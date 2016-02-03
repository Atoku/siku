/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2013-2014 UAF. Author: Anton Kulchitsky
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

void _freeze( ContactDetector::Contact& c, Globals& siku, const double& tol );

inline double _sqr( const double& x ) { return x*x; }

inline double _dist2( const vec3d& v1, const vec3d& v2 )
{
  return dot( v2 - v1, v2 - v1 );
}

// =============================== Methods ==================================

// For sorting watch
// https://s-media-cache-ak0.pinimg.com/originals/5f/fc/42/5ffc4224b938d1fb0abee887e4add84b.jpg
// Yet simple 'sort' from <algorithm> is used.

// IMPROVE: remove code duplication in 'find_pairs'
void ContactDetector::sweep_n_prune( Globals& siku )
{
  // preparing additional vector:
  // setting pointers in supporting vector (for sortings, etc)
  siku.pes.clear();
  for( size_t i = 0; i < siku.es.size(); ++i )
    siku.pes.push_back( &siku.es[i] );

  // sorting
  std::sort( siku.pes.begin(), siku.pes.end(), el_pointers_x_compare );

  // IMPROVE: need to create empty vector of instantly large capacity
  static std::vector<ContactDetector::Contact> news;

  // contact search
  for ( size_t i = 0; i < siku.pes.size () - 1; ++i )
    {
      if( siku.pes[i]->flag & Element::F_ERRORED ) continue;

      for ( size_t j = i + 1; j < siku.pes.size (); ++j )
        {
          if( siku.pes[j]->flag & Element::F_ERRORED ) continue;

          if ( _dist2( siku.pes[i]->Glob, siku.pes[j]->Glob ) <
              _sqr( siku.pes[i]->sbb_rmin + siku.pes[j]->sbb_rmin ) )
            {
              //// deprecated
              //add_cont( siku, siku.pes[i]->id, siku.pes[j]->id,
              //          siku.time.get_n () );

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

  for ( size_t i = 0; i < siku.es.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.es.size (); ++j )
        {
          if ( _dist2( siku.es[i].Glob, siku.es[j].Glob ) <
              _sqr( siku.es[i].sbb_rmin + siku.es[j].sbb_rmin ) )
            add_cont( siku, i, j, siku.time.get_n() );
        }
    }

  std::sort( news.begin(), news.end() );

  merge_contacts( siku.ConDet.cont, news );

  news.clear();  // should be O(1) without deallocation
}

// --------------------------------------------------------------------------

void ContactDetector::clear()
{
  size_t size = cont.size();
  for( size_t i = 0; i < size; ++i )
    {
      if( cont[i].type == ContType::JOINT )  // joints untouched
        {
          if( cont[i].durability < 0.05 )  // destruction
            cont[i].type = ContType::COLLISION;
        }
      else  // deleting contact
        {
          if( cont[i].generation > 1 )  // IMPROVE
            {
              cont[i] = cont[--size];  // replacing current with last
              cont.pop_back();         // and deleting last
            }
          else
            cont[i].generation++;
        }
    }

  // sorting of remains. Required by contact_exist check
  std::sort( cont.begin(), cont.end() );
}

//---------------------------------------------------------------------

void ContactDetector::detect( Globals& siku )
{
  // elements amount check: less then 2 - contacts are impossible
  if( siku.es.size() < 2 || !is_detect_time( siku ) )
    return;

  //cout<<"DETECT-----------------"<<endl;  // for tests, IMPROVE:remove this

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
  for( auto& c : cont )
    _freeze( c, siku, tol );

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
  size_t oi = 0, ni = 0;
  while( true ) //oi < olds.size() || ni < news.size() )
    {
      // IMPROVE: remove unnecessary conditions
      if( ni < news.size() )
        {

          if( oi < olds.size() )
//            {
//              if( news[ni] < olds[oi] )
//                temp.push_back( news[ni++] );
//              else
//                temp.push_back( olds[oi++] );
//            }
            temp.push_back( news[ni] < olds[oi] ? news[ni++] : olds[oi++] );
          else
            temp.push_back( news[ni++] );

        }
      else if( oi < olds.size() )
        temp.push_back( olds[oi++] );
      else
        break; // instead of 'while' args

    }

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

// add new contact to list with type checks
void add_cont( Globals& siku, const size_t& i1, const size_t& i2, const int& t )
{
  if( !contact_exists( siku.ConDet, i1, i2 ) )
    {
      siku.ConDet.cont.push_back ( ContactDetector::Contact ( i1, i2, t ) );
    }
}

// --------------------------------------------------------------------------

//using namespace BG;
// perform freezing on two elements by contact
// implementation copied from 'contact_force'
// TODO: move to 'geometric' module
void _freeze( ContactDetector::Contact& c, Globals& siku, const double& tol )
{
  int ires;  // !static? temporal variable to store geometry results
  //vec3d center; // !static
  //double dump;  // !static? temporal dump
  std::vector<vec2d> loc_P1;  // e1.P vertices in local coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local coords

  mat3d src_to_dest = loc_to_loc_mat( siku.es[c.i1].q, siku.es[c.i2].q );
      // !static
  mat3d dest_to_src = loc_to_loc_mat( siku.es[c.i2].q, siku.es[c.i1].q );
      // !static

  for( auto& p : siku.es[c.i1].P )
    loc_P1.push_back( vec3_to_vec2( p * tol ) );
  for( auto& p : siku.es[c.i2].P )
    loc_P2.push_back( vec3_to_vec2( src_to_dest * p * tol ) );

  if( Geometry::intersect( loc_P1, loc_P2 , loc_P2 ) )
    c.type = ContType::JOINT;
}

// --------------------------------------------------------------------------

