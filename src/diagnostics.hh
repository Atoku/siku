/*!

  \file diagnostics.hh

  \brief Diagnostics: transfer the data to python functions about
  different fields values on different meshes for diagnostics
  purposes.

*/

#ifndef DIAGNOSTICS_HH
#define DIAGNOSTICS_HH

#include <vector>

#include "siku.hh"
#include "globals.hh"
#include "mesh.hh"
#include "scheduler.hh"

//! \brief matches index of python function stored in Sikupy class,
//! index of mesh stored here, and the scheduler.
class Diagbase
{
 public:
  size_t ifunc;                 //!< index of diagnostics function in
                                //! Sikupy
  size_t imesh;                 //!< index of mesh in Diagnostics
  Scheduler scheduler;          //!< scheduler when to run a
                                //! particular diagnostics
};

//! \brief class for all diagnostics on added meshes 
class Diagnostics
{
 public:

  //! diagnostics on (meshes present)
  const static unsigned int TURN_ON {0x1};

  //! diagnostics of winds on (wind is not empty)
  const static unsigned int WIND_ON {0x2};

  //! Dropping a particular flag 
  void flags_drop( unsigned int bitval ) { flag &= ~bitval; };
  
  //! Setting a flag
  void flags_set( unsigned int bitval ) { flag |= bitval; };

  //! Checking a flag
  bool flags_check( unsigned int bitval ) 
  { return ( (flag & bitval) == bitval ); }

  //! Turn diagnostics On (Need to call it)
  void turn_on() { flag |= TURN_ON; };

  //! Turn diagnostics Off (no meshes, no need to call it)
  void turn_off() { flag &= ~TURN_ON; };

  //! Check if the diagnostics turned on 
  bool turned_on() { return flag & TURN_ON; };

  //! Meshes are easier to make public than write all the methods
  std::vector<Mesh> meshes;

  //! Wind diagnostics option 
  std::vector<Diagbase> windbase;

 private:
  unsigned int flag {0x0};            // states
};

#endif      /* DIAGNOSTICS_HH */
