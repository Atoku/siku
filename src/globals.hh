/*!

  \file globals.hh

  \brief Global variables are united in a structure for easy pass to
  initialization and saving functions

*/

#ifndef GLOBALS_HH
#define GLOBALS_HH

#include <vector>

#include "info.hh"
#include "element.hh"
#include "modeltime.hh"
#include "planet.hh"
#include "material.hh"
#include "vecfield.hh"
#include "diagnostics.hh"

//! \brief Structure to keep all "global" data here. This namespace
//! (will have the name siku) contains all the variables being
//! initialized from the config file and computed or re-computed.
struct Globals
{
  //! General information about the model
  Info info;

  //! Planet parameters (radius, angular velocity etc.)
  Planet planet;

  //! Materials
  std::vector<Material> ms;

  //! Elements data
  std::vector<Element> es;

  //! model time 
  ModelTime time;

  //! atmospheric wind data
  Vecfield wind;

  //! Current filename to save to
  string savefile { "siku_output.h5" };

  //! Datastructure to store diagnostics info 
  Diagnostics diagnostics;
};

#endif      /* GLOBALS_HH */
