/*!

  \file material.hh

  \brief Materials

*/

#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <vector>

#include "siku.hh"

//! \brief Material description
struct Material
{
  //! Single layer properties
  struct Layer
  {
    double thickness;  //< Upper bounds for "ice" thickness
    double rho;        //< Density for thickness interval
    double sigma_c;    //< Shear strength for interval
    double sigma_t;    //< Tensile strength for interval

    //! Both default and simple parametric constructor
    Layer( const double& t = 0., const double& r = 0.,
           const double& sc = 0., const double& st = 0. ) :
             thickness( t ), rho( r ), sigma_c( sc ), sigma_t( st ) {}
  };

// ----------------------------- Members ------------------------------

  //! Name
  string name;

  //! Young modulus
  double E;

  //! Poisson ratio
  double nu;

  //! Set of layers
  Layer layers[ MAT_LAY_AMO ];

//// OLD
//  //! Set of upper bounds for "ice" thickness
//  std::vector<double> thickness_intervals;
//
//  //! Densitites for each thickness interval
//  std::vector<double> rho;
//
//  //! Shear strength for each interval
//  std::vector<double> sigma_c;
//
//  //! Tensile stresngth for each interval
//  std::vector<double> sigma_t;
};

#endif      /* MATERIAL_HH */

