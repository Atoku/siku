/*!

  \file vecfield.hh

  \brief Vector field class. A basic class to contains air wind and
  sea streams vector fields, produce interpolation and provide values
  of the vector field in particular points.

*/

#ifndef VECFIELD_HH
#define VECFIELD_HH

#include "siku.hh"

class Vecfield
{
 public:

  //! \brief Flag points to standard field from Fuselier paper
  static const int MODE_VEC_STD_FIELD1 {1};

  Vecfield();

  //! \brief sets the vector field model (standard, specific
  //! interpolation model etc.: see MODE_VEC_ constants
  void set_model( int model )
  { mode = model; };

  //! \brief Returns vector (3D) global coordinates at extrinsic
  //! coordinates (x,y).
  void get_at_xy( const vec3d& x,
                  vec3d* pv );

 private:

  int mode;

  //! \brief Implementation of filed1 from Fuselier, Edward J and
  //! Wright, Grady B article.
  void field1( const vec3d& x, vec3d* pv );

};

#endif      /* VECFIELD_HH */
