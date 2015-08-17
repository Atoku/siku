/*!

  \file vecfield.cc

  \brief Implementation of vector field class.

*/

#include "vecfield.hh"
#include "errors.hh"

//---------------------------------------------------------------------

Vecfield::Vecfield()
{
  mode = MODE_VEC_STD_FIELD1;
}

//---------------------------------------------------------------------

void Vecfield::get_at_xy( const vec3d& x,
                          vec3d* pv )
{
  switch ( mode )
    {
      case MODE_VEC_STD_FIELD1: 
        field1( x, pv );
        break;

      default:
        fatal( 1, "Not standard wind field is not implemented yet" );
        break;
    }
}

//---------------------------------------------------------------------

const static double Alpha {-1.0/sqrt(3.0)};
const static double Beta  {8.0*sqrt(2.0)/(3.0*sqrt(385.0))};

const static double k1 {sqrt( 3.0/( 4.0 * M_PI ) )};
const static double k2 {( 3.0 / 16.0 ) * sqrt( 385.0/( 2.0 * M_PI ) )};

void Vecfield::field1( const vec3d& x,
                       vec3d* pv )
{
  const double phi = atan2( x[1], x[0] );
  const double F =  4.0 * k2 * sin( 4 * phi );
  const double G =        k2 * cos( 4 * phi );
  const double Z = ( x[0]*x[0] + x[1]*x[1] );

  const vec3d dpsi = vec3d( Beta * F * x[1] * x[2] * Z,
                          - Beta * F * x[0] * x[2] * Z,
                            Alpha * k1 
                            + Beta*G * Z * ( 1 - 5 * x[2]*x[2] ) );

  *pv = glm::cross( x, dpsi );
}
