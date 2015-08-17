/*!

  \file material.hh

  \brief Materials

*/

#ifndef MATERIAL_HH
#define MATERIAL_HH

#include <vector>

//! \brief Material description
struct Material
{
  //! Name
  string name;

  //! Set of upper bounds for "ice" thickness 
  std::vector<double> thickness_intervals;

  //! Densitites for each thickness interval
  std::vector<double> rho;

  //! Shear strength for each interval
  std::vector<double> sigma_c;

  //! Tensile stresngth for each interval
  std::vector<double> sigma_t;

  //! Young modulus
  double E;

  //! Poisson ratio
  double nu;
};

#endif      /* MATERIAL_HH */

