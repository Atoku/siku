/*!

 \file globals.hh

 \brief Global variables are united in a structure for easy pass to
 initialization and saving functions

 */

#ifndef GLOBALS_HH
#define GLOBALS_HH

#include <vector>
#include <string>

#include "info.hh"
#include "element.hh"
#include "modeltime.hh"
#include "planet.hh"
#include "material.hh"
#include "vecfield.hh"
#include "diagnostics.hh"
#include "contact_detect.hh"

enum : unsigned long
{
  STATUS_NONE = 0x0,
  STATUS_SAVE = 0x1,
  STATUS_WINDS = 0x2,
  STATUS_CURRENTS = 0x4,
  STATUS_EXIT = 0x80 // aka 128
};

enum CONTACT_FORCE_MODEL : unsigned long
{
  CF_DEFAULT = 0,
  CF_TEST_SPRINGS = 0,
  CF_HOPKINS = 1
};

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
  std::vector < Material > ms;

  Material tmat;

  //! Elements data !! Important: the order of elements in this array should
  //! remain unchanged. Element IDs match their position in this vector.
  //! For any sorting necessities use 'pes' vector.
  std::vector < Element > es;

  //! Elements` pointers vector for sortings and temporal processings
  std::vector < Element* > pes;

  //! Monitors
  std::vector < std::string > mons;

  //! Controls
  std::vector < std::string > cons;

  //! manual forces
  std::vector < long > man_inds;
  std::vector < vec3d > man_forces;

  //! a list of wind source files` names
  std::vector < std::string > wind_crs;

  // IMPROVE: reconsider this mechanism
  //! physical constants
  std::vector <double> phys_consts;

  //! model time 
  ModelTime time;

  //! atmospheric wind data
  Vecfield wind;

  //! water currents data (parametrical constructor call)
  Vecfield flows = Vecfield( Vecfield::NONE ); //YET NO FLOWS

  //! Datastructure to store diagnostics info 
  Diagnostics diagnostics;

  //! Contact detector and store
  ContactDetector ConDet;

  //! Boarders` points file name
  string board_file
      { "NO BOARDERS" };

  //! Current filename to save to
  string savefile
      { "siku_output.h5" };

  //! Filename to load from
  string loadfile;

  //! Flag for marking boarder polygons as 'static' or else
  unsigned long mark_boarders { 0 };

  //! Callback-returned status (masked flags)
  unsigned long callback_status { STATUS_NONE };

  //! contact force model
  CONTACT_FORCE_MODEL cont_force_model { CF_DEFAULT };

  // ------------------------------ METHODS ---------------------------------

  //! Post-initialization (with loaded values)
  void post_init();

  //! Default constructor
  Globals();

  //! Registration of element`s monitor function
  void add_monit( std::string& mon, Element& e );

  //! Registration of element`s control function
  void add_contr( std::string& con, Element& e );


};



#endif      /* GLOBALS_HH */
