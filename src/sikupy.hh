/*
 * Siku: Discrete element method sea-ice model: sikupy.hh
 *       Siku Python interface class
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
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

/*!

 \file sikupy.hh

 \brief Siku Python interface class

 */

#ifndef SIKUPY_HH
#define SIKUPY_HH

extern "C"
{
#include <Python.h>
#undef tolower                  // fuck this practice by python
#include "config.h"
}

#include <string>
using namespace std;

#include "siku.hh"
#include "globals.hh"
#include "diagnostics.hh"

#include "vecfield.hh"

////////////
#include <iostream>

//! \brief Python interface wrapper
class Sikupy
{

public:

  //! \brief Start python, declare the variable
  Sikupy ( string filename );

  //! \brief Do all initialization and open conf file
  void
  initialize ( Globals& siku );

  //! \brief Finalize (technically should be called only from
  //! destructor, however it is left public in case session should be
  //! restarted).
  void
  finalize ();

  // -- Function to call call backs

  //! \brief Perform precalculations of each time step with internal call
  //! of the pretimestep method in python scenario file
  //! \param[in] siku main global variables container
  int
  fcall_pretimestep ( Globals& siku );

  //! \brief Perform aftercalculations of each time step
  //! \param[in] siku main global variables container
  int
  fcall_aftertimestep ( Globals& siku );

  //! \brief call presave (updates siku.savefile)
  int
  fcall_presave ( Globals& siku );

  //! \brief call monitor function
  //! \param[in] siku main global variables container
  //! \param[in] i index of the element we call the function for
  //! \param[in] fname function name to call
  int
  fcall_monitor ( const Globals& siku, const size_t i, const char* fname );

  //! \brief call diagnostics function for vector field
  //! \param[in] siku main global variables container
  //! \param[in] i index of diagnostics function to call
  //! \param[in] data data to output
  int
  fcall_diagnostics_vec3d ( const Globals& siku, const size_t i,
                            const vector < vec3d >& data );

  //! \brief Update wind from NMC source
  //! \param[in] siku main global variables container
  int
  fcall_update_nmc_wind ( Globals& siku );

//  //! \brief Check and perform winds update
//  //! \param[in] siku main global variables container
//  int
//  fcall_winds ( Globals& siku );

  //! \brief Perform python conclusive actions (before program exits)
  //! \param[in] siku main global variables container
  int
  fcall_conclusions ( Globals& siku );

  //! \brief Release all owened PyObjs
  ~Sikupy ();

  // Fast call return status constants
  static const int FCALL_OK
    { 0x0 };
  static const int FCALL_ERROR_NO_FUNCTION
    { 0x1 };
  static const int FCALL_ERROR_PRESAVE_NOSTRING
    { 0x2 };
  static const int FCALL_ERROR_PRETIMESTEP_NOLONG
    { 0x4 };
  static const int FCALL_ERROR_NOWINDS
    { 0x8 };

  //---------------------------------------------------------------------
  //                       PRIVATE METHODS
  //---------------------------------------------------------------------

private:

  // The following PyObject live all the time while the model runs to
  // make sure that there is an access to all the siku elements

  PyObject *pModule;            //!< Main configuration module (new)
  PyObject *pDict;              //!< Dictionary in it (borrowed)

  //! \brief Access to the siku name space with all the initialization
  //! data (incremented)
  PyObject *pSiku;

  //! \brief Access to siku.callback object to reach callback functions
  //! (incremented)
  PyObject *pSiku_callback;

  //! \brief Access to siku.diagnostics object to reach all
  //! diagnostics stuff if defined;
  PyObject *pSiku_diagnostics;

  //! \brief List of functions to call (common list for all general
  //! functions)
  vector < PyObject* > pSiku_funcs;

  //! \brief Current time, generated at the beginning of each time step
  //! in fcall_pretimestep and cleared at the end in dcall_aftertimestep
  //! !! NOT in constructor and destructor!
  PyObject* pCurTime
    { nullptr };

  unsigned int flag
    { 0 };   //!< different states for the class

  // -----------------------------------------------------------------
  // local methods to structurize initialize method in sections mostly
  // -----------------------------------------------------------------  

  //! \brief Reading default values and settings
  int
  read_default ( Globals& siku );

  //! \brief Reading info data
  int
  read_info ( Info& info );

  //! \brief Reading Planet data like R, omega. 
  int
  read_planet ( Planet& planet );

  //! \brief Reading model time parameters
  int
  read_modeltime ( ModelTime& time );

  //! \brief Reading materials
  int
  read_materials ( vector < Material >& ms );

  //! \brief Reading elements
  int
  read_elements ( vector < Element >& es );

  //! \brief Reading diagnostics class if it exists
  int
  read_diagnostics ( Diagnostics& diag );

  //! \brief Reading diagnostics meshes
  int
  read_diagnostics_meshes ( Diagnostics& diag );

  //! \breif Reading how to diagnose winds 
  int
  read_diagnostics_winds ( Diagnostics& diag );

  // -----------------------------------------------------------------
  //! \brief Reading NMC vector grid with/from wnd.py
  int
  read_nmc_vecfield ( NMCVecfield& vField, const char* vName );

  // -----------------------------------------------------------------
  // local methods to simplify access to some data types
  // -----------------------------------------------------------------  

  //! \brief Reads quaternion
  //! \param pquat Tuple (or any other object if appropriate) 
  //! to read quaternion from
  //! \param q quaternion to read the data to
  //! \return true on success, false on fail
  bool
  read_quat ( PyObject* pquat, quat& q );

  //! \brief Reads usigned long
  //! \param pLong a number to read from
  //! \param x ref. to where it is read
  //! \return true on success, false if any errors occurred
  bool
  read_ulong ( PyObject* pLong, unsigned long& x );

  //! \brief Reads float
  //! \param pfloat a number to read from
  //! \param x ref. to where it is read
  //! \return true on success, false on fail
  bool
  read_double ( PyObject* pfloat, double& x );

  //! \brief Reads int
  //! \param pint a number to read from
  //! \param x a value to read to
  //! \return true on success, false on fail
  bool
  read_long ( PyObject* pint, long& x );

  //! \brief Reads string
  //! \param pstr an Python object to read from
  //! \param str where to read to
  //! \return true on success, false on fail
  bool
  read_string ( PyObject* pstr, string& str );

  //! \brief Reads vector of doubles from a list
  //! \param plist an Python object to read from
  //! \param xs where to read to
  //! \return true on success, false on fail
  bool
  read_double_vector ( PyObject* plist, vector < double >& xs );

  //! \brief Reads vector of vec3d from a list of 3 element lists
  //! \param plist an Python object to read from
  //! \param vs where to read to
  //! \return true on success, false on fail
  bool
  read_vec3d_vector ( PyObject* plist, vector < vec3d >& vs );

  //! \brief reading time_duration object
  //! \param[in] pobj a Python object to read from
  //! \param[out] dt time duration object read
  //! \return true on success, false on fail
  bool
  read_dt ( PyObject* pobj, boost::posix_time::time_duration& dt );

  //! \brief reading posix time object
  //! \param[in] pobj a Python object to read from
  //! \param[out] t time object read
  //! \return true on success, false on fail
  bool
  read_time ( PyObject* pobj, boost::posix_time::ptime& t );

  //! \brief Flag value saying that Py_Initialize was called
  static const unsigned int FLAG_PY_INITIALIZED
    { 0x1 };

  //! \brief Flag value saying that initialize was called. The flag is
  //! dropped in finalize member.
  static const unsigned int FLAG_INITIALIZED
    { 0x2 };

};

#endif      /* SIKUPY_HH */
