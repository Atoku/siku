/*!

 \file sikupy.cc

 \brief Python interface implementation for siku

 */

#include <iostream>
using namespace std;

extern "C"
{
#include <Python.h>
#undef tolower                  // fuck this practice by python
#include <stdio.h>
#include <datetime.h>           // python module to get datetime objs
}

#include "errors.hh"
#include "sikupy.hh"
#include "auxutils.hh"

#include "coordinates.hh"
using namespace Coordinates;

//---------------------------------------------------------------------

Sikupy::Sikupy( string filename )
{
  // Initialize the Python Interpreter
  Py_Initialize();
  flag |= FLAG_PY_INITIALIZED;

  // Now we are just getting access to siku namespace: the only
  // namespace we actually read from

  // Adding the current path to python path
  //string python_path = string( Py_GetPath() );
  //cout << python_path << endl;
  // python_path = ".:" + python_path;
  // char* pc = strdup( python_path.c_str() );
  // PySys_SetPath( pc );
  // free( pc );

  // Converting the module name to PyString
  PyObject *pName;
  string module_name = auxutils::remove_file_extension( filename );
  pName = PyUnicode_FromString( module_name.c_str() );// new
  assert( pName );

  // Load the module object
  pModule = PyImport_Import( pName );// new
  Py_DECREF( pName );// no need for pName anymore
  if ( !pModule )
  fatal( 1, "module not found:   %s", module_name.c_str() );

  // Never fails if module OK
  pDict = PyModule_GetDict( pModule );// borrowed

  // access to the siku field. Ura!
  pSiku = PyDict_GetItemString( pDict, "siku" );// borrowed
  assert( pSiku );
  Py_INCREF( pSiku );// incremented to store in class
  // object

  // access to the siku.callback

  // new ref
  pSiku_callback = PyObject_GetAttrString( pSiku, "callback" );
  assert ( pSiku_callback );

  if ( !PyObject_IsInstance( pSiku_callback, pSiku ) )
  fatal( 1, "Cannot read siku.callback" );

  // access to the diagnostics (always defined)
  pSiku_diagnostics = PyObject_GetAttrString( pSiku, "diagnostics" );
  assert( pSiku_diagnostics );

  if ( !PyObject_IsInstance( pSiku_diagnostics, pSiku ) )
  fatal( 1, "Cannot read siku.diagnostics" );

}

//---------------------------------------------------------------------

void Sikupy::initialize(Globals &siku)
  {
    int success
      { 0 }; // success code

    success = read_settings( siku );
    assert(success);
    success = read_links( siku );
    assert(success);
    success = read_info(siku.info);
    assert(success);
    success = read_planet(siku.planet);
    assert(success);
    success = read_modeltime(siku.time);
    assert(success);
    success = read_materials(siku.ms);
    assert(success);
    success = read_elements(siku);
    assert(success);
    success = read_diagnostics(siku.diagnostics);
    assert(success);
    if ( success == 0 )
    fatal( 1, "Something  wrong went on initialization" );
  }

/*
  TODO: It is better to be able to initialize from files located in
  different directories. For this, the path and the filename should be
  separated. The path should be added to sys.path, the file
  name should be stripped from .py as it is done already.
*/

//---------------------------------------------------------------------

void
Sikupy::finalize ()
{
  Py_DECREF( pSiku_callback );
  Py_DECREF( pSiku_diagnostics );
  for ( auto pfunc : pSiku_funcs )
    Py_DECREF( pfunc );
  Py_DECREF( pSiku );
  Py_DECREF( pModule );

  flag = flag & ( ~FLAG_INITIALIZED );

  // Finish the Python Interpreter
  Py_Finalize ();
}

//---------------------------------------------------------------------

Sikupy::~Sikupy ()
{
  // Finalize if necessary
  if ( flag & FLAG_INITIALIZED )
    finalize ();
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
// local methods to structurize initialize method in sections mostly
//---------------------------------------------------------------------
//---------------------------------------------------------------------

int
Sikupy::read_settings( Globals& siku )
{
  int success = 1;

  // temporal object for reading
  PyObject* pTemp;
  unsigned long int i;

  // Settings handler
  PyObject* pDef;
  pDef = PyObject_GetAttrString ( pSiku, "settings" );
  assert( pDef );


  // read manual forces
  // read physical constants
  pTemp = PyObject_GetAttrString ( pDef, "manual_inds" );
  assert( pTemp );
  success &= read_long_vector( pTemp, siku.man_inds );
  Py_DECREF( pTemp );

  pTemp = PyObject_GetAttrString ( pDef, "manual_forces" );
  assert( pTemp );
  success &= read_vec3d_vector( pTemp, siku.man_forces );
  Py_DECREF( pTemp );


  // TODO: clear
  // read physical constants
//  pTemp = PyObject_GetAttrString ( pDef, "phys_consts" );
//  assert( pTemp );
//
//  success &= read_double_vector( pTemp, siku.phys_consts );
//  Py_DECREF( pTemp );

  pTemp = PyObject_GetAttrString ( pDef, "phys_consts" );
  assert( pTemp );

  success &= read_str_doub_map( pTemp, siku.phys_consts );
  Py_DECREF( pTemp );

  // read contact freezing method
  pTemp = PyObject_GetAttrString ( pDef, "contact_method" );
  assert( pTemp );

  success &= read_ulong( pTemp, siku.ConDet.det_meth );
  Py_DECREF( pTemp );

  // read contact detection frequency type
  pTemp = PyObject_GetAttrString ( pDef, "contact_freq_met" );
  assert( pTemp );

  success &= read_ulong( pTemp, siku.ConDet.det_freq_t );
  Py_DECREF( pTemp );

  // read contact detection value (speed, period, etc.)
  pTemp = PyObject_GetAttrString ( pDef, "contact_value" );
  assert( pTemp );

  success &= read_double( pTemp, siku.ConDet.det_value );
  Py_DECREF( pTemp );

  // read contact force model flag
  pTemp = PyObject_GetAttrString ( pDef, "force_model" );
  assert( pTemp );

  success &= read_ulong( pTemp, i );
  siku.cont_force_model = CONTACT_FORCE_MODEL( i );
  Py_DECREF( pTemp );

  // read wind source
  pTemp = PyObject_GetAttrString ( pDef, "wind_source_type" );
  assert( pTemp );

  success &= read_ulong( pTemp, i );
  siku.wind.FIELD_SOURCE_TYPE = Vecfield::Source_Type( i );
  Py_DECREF( pTemp );

  // read initial freezing mask
  pTemp = PyObject_GetAttrString ( pDef, "initial_freeze" );
  assert( pTemp );

  success &= read_ulong( pTemp, siku.ConDet.inital_freeze );
  Py_DECREF( pTemp );

  // read border marking mask
  pTemp = PyObject_GetAttrString ( pDef, "border_mark" );
  assert( pTemp );

  success &= read_ulong( pTemp, siku.mark_borders );
  Py_DECREF( pTemp );

  // read borders file name
  pTemp = PyObject_GetAttrString ( pDef, "borders" );
  assert( pTemp );

  success &= read_string( pTemp, siku.bord_file );

  // read load file name. zero-length string for no load
  pTemp = PyObject_GetAttrString ( pDef, "loadfile" );
  assert( pTemp );
  success &= read_string( pTemp, siku.loadfile );

  // cleaning
  Py_DECREF( pTemp );
  Py_DECREF( pDef );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_links ( Globals& siku )
{
  int success = 1;

  // Settings handler
  PyObject* pDef;
  pDef = PyObject_GetAttrString ( pSiku, "settings" );
  assert( pDef );

  PyObject* pLinks;  // list of links
  PyObject* pTemp;  // temporal object for reading
  unsigned long i1, i2;


  pLinks = PyObject_GetAttrString ( pDef, "links" );
  assert( pLinks );
  assert( PyList_Check( plist ) );
  Py_ssize_t N = PyList_Size ( pLinks );

  //PyObject* plist, vector < long >& xs;

  // array sets length
  siku.ConDet.links.resize ( N );

  // reading all links in this loop
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pLinks, i ); // borrowed

      // check if it is a list
      if ( !PyTuple_Check( pitem ) ) return false;

      Py_ssize_t K = PyTuple_Size ( pitem );
      if ( K != 2 ) return false; // must be pair (2)

      pTemp = PyTuple_GetItem ( pitem, 0 ); // borrowed
      success &= read_ulong( pTemp, i1 );

      pTemp = PyTuple_GetItem ( pitem, 1 ); // borrowed
      success &= read_ulong( pTemp, i2 );

      siku.ConDet.links[i] = Link( i1, i2 );
    }


  Py_DECREF( pDef );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_info ( Info& info )
{
  int success = 1;

  // currently we do nothing
  PyObject *pSiku_info;
  pSiku_info = PyObject_GetAttrString ( pSiku, "info" ); // new
  assert( pSiku_info );

  // check it is dictionary
  if ( !PyDict_Check( pSiku_info ) )
    fatal( 1, "siku.info is not a dictionary" );

  // Reading fields
  PyObject *pobj;               // temp object holder

  // (name) program name
  pobj = PyDict_GetItemString ( pSiku_info, "name" ); // borrowed
  assert( pobj );
  if ( !read_string ( pobj, info.name ) )
    fatal( 1, "cannot read siku.info.name" );

  // (brief) program description
  pobj = PyDict_GetItemString ( pSiku_info, "brief" ); // borrowed
  assert( pobj );
  if ( !read_string ( pobj, info.brief ) )
    fatal( 1, "cannot read siku.info.brief" );

  // (version) program version
  pobj = PyDict_GetItemString ( pSiku_info, "version" ); // borrowed
  assert( pobj );
  if ( !read_string ( pobj, info.version ) )
    fatal( 1, "cannot read siku.info.version" );

  // (date) program version
  pobj = PyDict_GetItemString ( pSiku_info, "date" ); // borrowed
  assert( pobj );
  if ( !read_string ( pobj, info.date ) )
    fatal( 1, "cannot read siku.info.date" );

  // (rundate) we creating the date when the program run
  namespace pt = boost::posix_time;
  pt::ptime now = pt::second_clock::local_time ();
  std::stringstream ss;
  ss << now.date ().month () << " " << now.date ().day () << ", "
      << now.date ().year ();
  info.rundate = string ( ss.str () );

  // free the info reference at the end
  Py_DECREF( pSiku_info );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_planet ( Planet& planet )
{
  int success = 1;
  int status;

  // reserving the string
  PyObject* p_str_radius;                         // .radius
  p_str_radius = PyUnicode_FromString ( "radius" ); // new
  assert( p_str_radius );

  // getting planet class
  PyObject *pSiku_planet;
  pSiku_planet = PyObject_GetAttrString ( pSiku, "planet" ); // new
  assert( pSiku_planet );

  if ( !PyObject_IsInstance ( pSiku_planet, pSiku ) )
    fatal(
        1,
        "Wrong siku.planet option," " must be an instance of siku.planet.Planet class" );

  // general object reference for all following actions
  PyObject* pobj;

  // reading radius siku.planet.radius value
  pobj = PyObject_GetAttr ( pSiku_planet, p_str_radius ); // new
  assert( pobj );

  status = read_double ( pobj, planet.R );
  if ( !status )
    fatal( 1, "Cannot read planet radius from siku.planet" );

  Py_DECREF( pobj );

  // getting the reciprocal value for the radius
  planet.R_rec = 1.0 / planet.R;

  // Freeing what we got at the beginning
  Py_DECREF( p_str_radius );
  Py_DECREF( pSiku_planet );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_modeltime ( ModelTime& time )
{
  int success = 1;
  bool status;                  // temp status

  // here we reading siku.time instance of ModelTime
  PyObject *pSiku_time;      // siku.time

  // borrowed
  pSiku_time = PyObject_GetAttrString ( pSiku, "time" ); // new
  assert( pSiku_time );

  if ( !PyObject_IsInstance ( pSiku_time, pSiku ) )
    fatal( 1, "Failed on reading siku.time" );

  // Invoking the DateTime
  PyDateTime_IMPORT;

  PyObject *pobj;

  // (start) reading start
  boost::posix_time::ptime tmp_t;

  pobj = PyObject_GetAttrString ( pSiku_time, "start" ); // new
  assert( pobj );
  status = read_time ( pobj, tmp_t );
  if ( !status )
    fatal( 1, "Failed to read siku.time.start" );
  Py_DECREF( pobj );            // done with it

  time.set_marker ( ModelTime::MARKER_START, tmp_t );
  time.restart_current ();       // current = start

  // fist save is set to the start time
  time.set_marker ( ModelTime::MARKER_SAVE, tmp_t );

  // (finish) reading .finish
  pobj = PyObject_GetAttrString ( pSiku_time, "finish" ); // new
  assert( pobj );
  status = read_time ( pobj, tmp_t );
  if ( !status )
    fatal( 1, "Failed to read siku.time.finish" );
  Py_DECREF( pobj );            // done with it

  time.set_marker ( ModelTime::MARKER_FINISH, tmp_t );

  // (dt) reading .dt
  boost::posix_time::time_duration tmp_dt;

  pobj = PyObject_GetAttrString ( pSiku_time, "dt" ); // new
  assert( pobj );

  status = read_dt ( pobj, tmp_dt );
  if ( !status )
    fatal( 1,
           "Failed to read siku.time.dt" " (must be datetime.delta instance)" );
  Py_DECREF( pobj );           // done with it

  time.set_dt ( ModelTime::DT_DT, tmp_dt );

  // (dts) reading .dts
  pobj = PyObject_GetAttrString ( pSiku_time, "dts" ); // new
  assert( pobj );

  status = read_dt ( pobj, tmp_dt );
  if ( !status )
    fatal( 1,
           "Failed to read siku.time.dts" " (must be datetime.delta instance)" );
  Py_DECREF( pobj );           // done with it

  time.set_dt ( ModelTime::DT_DTS, tmp_dt );

  // freeing the reference
  Py_DECREF( pSiku_time );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_materials ( vector < Material >& ms )
{
  int success = 1;
  std::vector<double> tvec;

  PyObject *pSiku_materials;    // siku.materials list

  // new
  pSiku_materials = PyObject_GetAttrString ( pSiku, "materials" );
  assert( pSiku_materials );

  if ( !PyList_Check( pSiku_materials ) )
    fatal( 1, "Failed to read siku.materials or it is not a list" );

  Py_ssize_t nms = PyList_Size ( pSiku_materials );

  // array sets length
  ms.resize ( nms );

  // reading all the materials in this loop
  for ( Py_ssize_t i = 0; i < nms; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pSiku_materials, i );

      // TODO: include correct check of the element
      // if ( !PyObject_IsInstance( pitem, pSiku ) )
      //   fatal( 1, "Wrong element #%ld of siku.elements list,"
      //          " must be an instance of siku.Material class", (long int)i );

      PyObject* pobj;           // for all the objects

      // reading name
      pobj = PyObject_GetAttrString ( pitem, "name" ); // new
      assert( pobj );

      success = read_string ( pobj, ms[i].name );
      assert( success );

      Py_DECREF( pobj );

      // reading ice thicknesses
      pobj = PyObject_GetAttrString ( pitem, "thickness_intervals" ); // new
      assert( pobj );

      success = read_double_vector ( pobj, tvec );
      assert( success );
      for( size_t j = 0; j < tvec.size(); ++j )
        ms[i].layers[j].thickness = tvec[j];

      Py_DECREF( pobj );

      // reading densities
      pobj = PyObject_GetAttrString ( pitem, "rho" ); // new
      assert( pobj );

      success = read_double_vector ( pobj, tvec );
      assert( success );
      for( size_t j = 0; j < tvec.size(); ++j )
        ms[i].layers[j].rho = tvec[j];

      Py_DECREF( pobj );

      //assert( ms[i].rho.size () == ms[i].thickness_intervals.size () );

      // reading sigma_c
      pobj = PyObject_GetAttrString ( pitem, "sigma_c" ); // new
      assert( pobj );

      success = read_double_vector ( pobj, tvec );
      assert( success );
      for( size_t j = 0; j < tvec.size(); ++j )
        ms[i].layers[j].sigma_c = tvec[j];

      Py_DECREF( pobj );

      //assert( ms[i].sigma_c.size () == ms[i].thickness_intervals.size () );

      // reading sigma_t
      pobj = PyObject_GetAttrString ( pitem, "sigma_t" ); // new
      assert( pobj );

      success = read_double_vector ( pobj, tvec );
      assert( success );
      for( size_t j = 0; j < tvec.size(); ++j )
        ms[i].layers[j].sigma_t = tvec[j];

      Py_DECREF( pobj );

      //assert( ms[i].sigma_t.size () == ms[i].thickness_intervals.size () );

      // reading Young modulus
      pobj = PyObject_GetAttrString ( pitem, "E" ); // new
      assert( pobj );

      success = read_double ( pobj, ms[i].E );

      Py_DECREF( pobj );

      // reading Poisson ratio
      pobj = PyObject_GetAttrString ( pitem, "nu" ); // new
      assert( pobj );

      success = read_double ( pobj, ms[i].nu );

      Py_DECREF( pobj );

    }

  Py_DECREF( pSiku_materials );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_elements ( Globals& siku )
{
  int success = 1;
  long tmp = 0;                 // just a buffer
  string stmp;                  // just a buffer
  std::vector<double> tvec;     // double buffer

  PyObject *pSiku_elements;      // siku.elements list

  // new
  pSiku_elements = PyObject_GetAttrString ( pSiku, "elements" );
  assert( pSiku_elements );

  if ( !PyList_Check( pSiku_elements ) )
    fatal( 1, "Failed to read siku.elements or it is not a list" );

  Py_ssize_t nes = PyList_Size ( pSiku_elements );

  // array sets length
  siku.es.resize ( nes );

  // reading all elements in this loop
  for ( Py_ssize_t i = 0; i < nes; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pSiku_elements, i );

      // TODO: include correct check of the element
      // if ( !PyObject_IsInstance( pitem, pSiku ) )
      //   fatal( 1, "Wrong element #%ld of siku.elements list,"
      //          " must be an instance of siku.Material class", (long int)i );

      PyObject* pobj;           // for all the objects

      // reading geom. moment of inertia (I/m)
      pobj = PyObject_GetAttrString ( pitem, "i" ); // new
      assert( pobj );

      success = read_double ( pobj, siku.es[i].i );
      assert( success );

      Py_DECREF( pobj );

      // reading area
      pobj = PyObject_GetAttrString ( pitem, "A" ); // new
      assert( pobj );

      success = read_double ( pobj, siku.es[i].A );
      assert( success );

      Py_DECREF( pobj );

      // reading quaternion q (loc->global)
      pobj = PyObject_GetAttrString ( pitem, "q" ); // new
      assert( pobj );

      success = read_quat ( pobj, siku.es[i].q );
      assert( success );

      Py_DECREF( pobj );

      // reading g(h) - ice thickness distribution for the element
      pobj = PyObject_GetAttrString ( pitem, "gh" );
      assert( pobj );

      success = read_double_vector ( pobj, tvec );
      if ( !success )
        fatal( 2, "g(h) is not set of element %lu", i );
      for( size_t j = 0; j < tvec.size(); ++j )
        siku.es[i].gh[j] = tvec[j];

      Py_DECREF( pobj );

      // reading sphere radius
      pobj = PyObject_GetAttrString ( pitem, "sbb_rmin" ); // new
      assert( pobj );

      success = read_double ( pobj, siku.es[i].sbb_rmin );
      assert( success );

      Py_DECREF( pobj );

      // reading anchority factor
      pobj = PyObject_GetAttrString ( pitem, "anchority" ); // new
      assert( pobj );

      success = read_double ( pobj, siku.es[i].anchority );
      assert( success );

      Py_DECREF( pobj );

      // reading windage factor
      pobj = PyObject_GetAttrString ( pitem, "windage" ); // new
      assert( pobj );

      success = read_double ( pobj, siku.es[i].windage );
      assert( success );

      Py_DECREF( pobj );

      // reading all local coordinates of vertices Pi
      pobj = PyObject_GetAttrString ( pitem, "verts_xyz_loc" );
      assert( pobj );

      success = read_vec3d_vector ( pobj, siku.es[i].P );
      assert( success );

      Py_DECREF( pobj );

      // reading material index
      pobj = PyObject_GetAttrString ( pitem, "imat" );
      assert( pobj );

      success = read_long ( pobj, tmp );
      assert( success );

      siku.es[i].imat = (unsigned int) tmp;

      Py_DECREF( pobj );

      // reading flag for the state (free/steady/static)
      pobj = PyObject_GetAttrString ( pitem, "flag_state" );
      assert( pobj );

      success = read_long ( pobj, tmp );
      assert( success );

      siku.es[i].flag = (unsigned int) tmp;

      Py_DECREF( pobj );

      // reading name of monitor function if exists

      pobj = PyObject_GetAttrString ( pitem, "monitor" );
      assert( pobj );

      if ( read_string ( pobj, stmp ) )
        {
//          siku.es[i].monitor = string( stmp.c_str() );
//                              //instead //strdup ( stmp.c_str () );
//          assert( siku.es[i].monitor );
          siku.add_monit( stmp, siku.es[i] );

          // we also need to set a flag for this element
          siku.es[i].flag |= Element::F_MONITORED;
        }

      Py_DECREF( pobj );

      // reading inital velocity and rotation
      //es[i].V = nullvec;

      pobj = PyObject_GetAttrString ( pitem, "velo" );

      read_vec3d( pobj, siku.es[i].V );

      Py_DECREF( pobj );

      // Additional initialization without reading

      // could be inited in Globals.post_init() in main()
      siku.es[i].W = nullvec3d;

    }

  Py_DECREF( pSiku_elements );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_diagnostics ( Diagnostics& diag )
{
  int success;
  success = read_diagnostics_meshes ( diag );

  success = read_diagnostics_winds ( diag );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_diagnostics_meshes ( Diagnostics& diag )
{
  // this will read siku.diagnostics data
  int success = 1;

  // First read meshes
  PyObject *pMeshes;            // siku.diagnostics.meshes

  // new
  pMeshes = PyObject_GetAttrString ( pSiku_diagnostics, "meshes" );
  assert( pMeshes );

  if ( !PyList_Check( pMeshes ) )
    fatal( 1, "siku.diagnostics.meshes must be a list" );

  // diagnostics.meshes present or not?
  Py_ssize_t M = PyList_Size ( pMeshes );
  if ( M == 0 )                 // no meshes - nothing to do
    {
      diag.turn_off ();
      goto read_diagnostics_meshes_finalize;
    }
  else
    diag.turn_on ();

  diag.meshes.resize ( M );

  // reading all the meshes in this loop
  for ( Py_ssize_t i = 0; i < M; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pMeshes, i );

      if ( !PyList_Check( pitem ) )
        fatal( 2, "A mesh %u is not a list", (unsigned int ) i );

      bool status = read_vec3d_vector ( pitem, diag.meshes[i].data );
      if ( !status )
        fatal( 3, "Wrong mesh %u", (unsigned int ) i );
    }

  read_diagnostics_meshes_finalize:

  Py_DECREF( pMeshes );

  return success;

}

//---------------------------------------------------------------------

// It reads list of tuples: (function, number) to process
int
Sikupy::read_diagnostics_winds ( Diagnostics& diag )
{
  int success = 1;

  PyObject *pLst;            // siku.diagnostics.wind

  // new
  pLst = PyObject_GetAttrString ( pSiku_diagnostics, "wind" );
  assert( pLst );

  if ( !PyList_Check( pLst ) )
    fatal( 1, "siku.diagnostics.wind must be a list" );

  // diagnostics.meshes present or not?
  Py_ssize_t M = PyList_Size ( pLst );
  if ( M == 0 )                 // no meshes - nothing to do
    {
      diag.flags_drop ( Diagnostics::WIND_ON );
      goto read_diagnostics_winds_finalize;
    }
  else
    diag.flags_set ( Diagnostics::WIND_ON );

  // reading all the meshes in this loop
  for ( Py_ssize_t i = 0; i < M; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pLst, i );

      if ( !PyTuple_Check( pitem ) || PyTuple_Size ( pitem ) != 4 )
        fatal( 2, "diagnostics.wind element %u is not a tuple size 4",
               (unsigned int )i );

      // getting the function
      PyObject* psubitem;
      psubitem = PyTuple_GetItem ( pitem, 0 );
      assert( psubitem );

      if ( !PyCallable_Check ( psubitem ) )
        fatal( 3, "diagnostics.wind element %u element 0 is not a function",
               (unsigned int )i );

      // adding the function to the list
      Py_INCREF( psubitem );    // was borrowed, want to protect
      pSiku_funcs.push_back ( psubitem );

      // and index of mesh
      psubitem = PyTuple_GetItem ( pitem, 1 );
      long int imesh;
      bool bstatus = read_long ( psubitem, imesh );
      if ( !bstatus )
        fatal(
            4,
            "could not read mesh index" " from diagnostics.wind at element %u",
            (unsigned int )i );

      // read scheduler data
      boost::posix_time::ptime stime;
      boost::posix_time::time_duration sdt;

      psubitem = PyTuple_GetItem ( pitem, 2 );
      bstatus = read_time ( psubitem, stime );
      if ( !bstatus )
        fatal(
            4,
            "could not read initial time" " from diagnostics.wind at element %u",
            (unsigned int )i );

      psubitem = PyTuple_GetItem ( pitem, 3 );
      bstatus = read_dt ( psubitem, sdt );
      if ( !bstatus )
        fatal(
            4,
            "could not read time frequency" " from diagnostics.wind at element %u",
            (unsigned int )i );

      // set the diagnostics values
      Diagbase dbase;
      dbase.ifunc = pSiku_funcs.size () - 1;
      dbase.imesh = (size_t) imesh;
      dbase.scheduler.set_event_time ( stime );
      dbase.scheduler.set_dt ( sdt );
      diag.windbase.push_back ( dbase );

    }

  read_diagnostics_winds_finalize:

  Py_DECREF( pLst );

  return success;
}

//---------------------------------------------------------------------

int
Sikupy::read_nmc_vecfield ( NMCVecfield& vField, const char* vName )
{
  int success = 1;

  // getting 'wind' attribute
  PyObject *pSiku_wind;
  pSiku_wind = PyObject_GetAttrString ( pSiku, vName ); // NEW!!
  assert( pSiku_wind );

  /*
   * TODO: check while pSiku_wind is really the NMCSurfaceVField
   */

  // readeng time index
  PyObject* pTemp = PyObject_GetAttrString ( pSiku_wind, "time" ); //new
  read_long( pTemp, vField.time_step );
  Py_DECREF( pTemp );

  // preparing grid
  PyObject* Lat = PyObject_GetAttrString ( pSiku_wind, "lat" ); //new
  size_t lat_s = PyList_Size ( Lat );

  PyObject* Lon = PyObject_GetAttrString ( pSiku_wind, "lon" ); //new
  size_t lon_s = PyList_Size ( Lon );

  vField.init_grid ( lat_s, lon_s );

  // reading gride nodes` coordinates
  double dtemp; // temporal double

  for ( size_t i = 0; i < lat_s; ++i )
    {
      // reversed indexsation in NMC structures: lat[0] = 90, lat[size] = -90
      //pTemp = PyList_GetItem ( Lat, lat_s ); //borrowed
      pTemp = PyList_GetItem ( Lat, lat_s - i - 1 ); //borrowed

      read_double ( pTemp, dtemp );
      vField.lat_indexer[dtemp] = i;
      vField.lat_valuator[i] = dtemp;
    }

  for ( size_t i = 0; i < lon_s; ++i )
    {
      pTemp = PyList_GetItem ( Lon, i ); //borrowed

      read_double ( pTemp, dtemp );
      vField.lon_indexer[dtemp] = i;
      vField.lon_valuator[i] = dtemp;
    }

  // reading the vector values in grid
  pTemp = PyObject_GetAttrString ( pSiku_wind, "vec" ); //new
  PyObject* pTuple;

  double ew, nw; // temporal variables for next loop

  for ( size_t i = 0; i < lat_s; ++i )
    {
      // reversed indexsation in NMC structures: lat[0] = 90, lat[size] = -90
      PyObject* pLine = PyList_GetItem ( pTemp, lat_s - i - 1 ); //borrowed
      //PyObject* pLine = PyList_GetItem ( pTemp, i ); //borrowed

      for ( size_t j = 0; j < lon_s; ++j )
        {
          pTuple = PyList_GetItem ( pLine, j ); //borrowed

          read_double ( PyTuple_GetItem ( pTuple, 0 ), ew );
          read_double ( PyTuple_GetItem ( pTuple, 1 ), nw );

          vField.set_vec (
              geo_to_cart_surf_velo ( deg_to_rad ( vField.lat_valuator[i] ),
                                      deg_to_rad ( vField.lon_valuator[j] ),
                                      ew, nw ),
              i, j );
        }
    }

  // cleaning the mess
  Py_DECREF( Lat );
  Py_DECREF( Lon );
  Py_DECREF( pTemp );
  Py_DECREF( pSiku_wind );

  return success;
}

//---------------------------------------------------------------------
//               CALL BACK FUNCTIONS INTERFACE
//---------------------------------------------------------------------

int
Sikupy::fcall_pretimestep ( Globals& siku )
{
  int status = FCALL_OK;

/////////////// Not needed any more
//  // reading siku.callback.pretimestep
//  PyObject* pFunc =
//      PyObject_GetAttrString (pSiku_callback, "pretimestep"); // new
//  assert(pFunc);
//  if (!PyCallable_Check (pFunc))
//    return FCALL_ERROR_NO_FUNCTION;

  // preparing paramaters to pass (we send model time and dt) as
  // datetime object
  const size_t n = siku.time.get_n ();
  const size_t ns = siku.time.get_ns ();

  // creating datetime object
  long microseconds = siku.time.get_total_microseconds ()
      - 1000000 * siku.time.get_total_seconds ();

  pCurTime = PyDateTime_FromDateAndTime(
      siku.time.get_year (), // new
      siku.time.get_month(), siku.time.get_day (),
      (int ) siku.time.get_hours (), (int ) siku.time.get_minutes (),
      (int ) siku.time.get_seconds (), (int ) microseconds );
  assert( pCurTime );

  // calling python 'pretiestep' method
  PyObject* pReturnValue = PyObject_CallMethod ( pSiku_callback,
                                                  "pretimestep",
                                                  "(O,i,i)",
                                                  pCurTime, n, ns ); //new

  // should return long. If I`m not wrong- there is no 'int' methods nor values.
  if ( !PyLong_Check( pReturnValue ) )
    return FCALL_ERROR_PRETIMESTEP_NOLONG;

  read_ulong ( pReturnValue, siku.callback_status );

  // Calls for inner methods. Mask is being checked inside each of them
  status |= fcall_update_wind ( siku );

  Py_DECREF( pReturnValue );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_aftertimestep ( Globals& siku )
{
  int status = FCALL_OK;

  // preparing current step numbers
  const size_t n = siku.time.get_n ();
  const size_t ns = siku.time.get_ns ();

  // calling python 'aftertimestep' method
  PyObject* pReturn_value = PyObject_CallMethod ( pSiku_callback,
                                                  "aftertimestep", "(O,i,i)",
                                                  pCurTime, n, ns ); //new

  Py_DECREF( pReturn_value );
  Py_DECREF( pCurTime );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_update_wind ( Globals& siku )
{
  if ( !( siku.callback_status & STATUS_WINDS ) )
    return FCALL_OK;

  // temporal reference. Static coz only one update call is possible at a time.
  static PyObject* pTemp;

  // updating grid with specification of source type
  switch (siku.wind.FIELD_SOURCE_TYPE)
    {
    case Vecfield::NMC:
      // update itself
      cout << "Updating wind. New time is: \n";

      pTemp = PyObject_CallMethod ( pSiku_callback, "updatewind", "(O,O)",
                                    pSiku, pCurTime ); //new

      if ( !pTemp )
        return FCALL_ERROR_NO_FUNCTION;

      Py_DECREF( pTemp );

      if ( !read_nmc_vecfield ( *siku.wind.NMCVec, "wind" ) )
        return FCALL_ERROR_NOWINDS;

      // read wind source names
      PyObject* pDef;
      pDef = PyObject_GetAttrString ( pSiku, "settings" ); // Settings handler
      assert( pDef );

      pTemp = PyObject_GetAttrString ( pDef, "wind_source_names" ); // List
      assert( pTemp );

      read_string_vector( pTemp, siku.wind_crs );  // Read names

      Py_DECREF( pTemp );  // clear memory
      Py_DECREF( pDef );

      break;  //-----------------------------

    case Vecfield::TEST:
      cout << "Test wind field: no need to update\n";
      break;  //-----------------------------

    default:
      fatal( 1, "No source specified" )      ;

      break;  //-----------------------------
    }

  siku.callback_status &= ~STATUS_WINDS;
  return FCALL_OK;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_inits ( Globals& siku )
{
  int status = FCALL_OK;

  // creating datetime object
  long microseconds = siku.time.get_total_microseconds ()
      - 1000000 * siku.time.get_total_seconds ();

  pCurTime = PyDateTime_FromDateAndTime(
      siku.time.get_year (), // new
      siku.time.get_month(), siku.time.get_day (),
      (int ) siku.time.get_hours (), (int ) siku.time.get_minutes (),
      (int ) siku.time.get_seconds (), (int ) microseconds );
  assert( pCurTime );

  // conslusions call
  PyObject* pTemp = PyObject_CallMethod ( pSiku_callback, "initializations",
                                          "(O,O)", pSiku, pCurTime ); //new

  if ( !pTemp )
    status = FCALL_ERROR_NO_FUNCTION;

  Py_DECREF( pCurTime );
  Py_DECREF( pTemp );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_conclusions ( Globals& siku )
{
  int status = FCALL_OK;

  // creating datetime object
  long microseconds = siku.time.get_total_microseconds ()
      - 1000000 * siku.time.get_total_seconds ();

  pCurTime = PyDateTime_FromDateAndTime(
      siku.time.get_year (), // new
      siku.time.get_month(), siku.time.get_day (),
      (int ) siku.time.get_hours (), (int ) siku.time.get_minutes (),
      (int ) siku.time.get_seconds (), (int ) microseconds );
  assert( pCurTime );

  // conslusions call
  PyObject* pTemp = PyObject_CallMethod ( pSiku_callback, "conclusions",
                                          "(O,O)", pSiku, pCurTime ); //new

  if ( !pTemp )
    status = FCALL_ERROR_NO_FUNCTION;

  Py_DECREF( pCurTime );
  Py_DECREF( pTemp );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_presave ( Globals& siku )
{
  int status = FCALL_OK;

//  // reading siku.callback.presave
//  PyObject* pFunc;
//
//  pFunc = PyObject_GetAttrString ( pSiku_callback, "presave" ); // new
//  assert( pFunc );
//  if ( !PyCallable_Check ( pFunc ) )
//    return FCALL_ERROR_NO_FUNCTION;

  // preparing paramaters to pass (we send model time and dt) as
  // datetime object
  const size_t n = siku.time.get_n ();
  const size_t ns = siku.time.get_ns ();

//  PyObject* pargs;
//  pargs = Py_BuildValue ( "(O,i,i)", pCurTime, n, ns ); // new
//  assert( pargs );
//
//  // calling the object
//  PyObject* pReturn_value;
//  pReturn_value = PyObject_CallObject ( pFunc, pargs ); // new

  // instead of all that is commented out
  PyObject* pReturnValue =
        PyObject_CallMethod ( pSiku_callback, "presave", "(O,i,i)", pCurTime,
                              n, ns ); //new

  // if not returned a string, then it is a wrong presave
  if ( !PyUnicode_Check( pReturnValue ) )
    return FCALL_ERROR_PRESAVE_NOSTRING;

  read_string ( pReturnValue, siku.savefile );

  // do not need arguments and value anymore
//  Py_DECREF( pargs );
  Py_DECREF( pReturnValue );
//  Py_DECREF( pFunc );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_monitor( const Globals& siku, const size_t i, const char* fname )
{
  int status = FCALL_OK;

  // direct reference to the element to store
  const Element* pe = &siku.es[i];
  //Element ee = *pe;


  // creating quaternion object ( q )
  PyObject* pQTuple = PyTuple_New ( 4 ); // quaternion as a new tuple, len = 4

  for ( Py_ssize_t k = 0; k < 4; ++k )
    {
      PyObject* pNum = PyFloat_FromDouble ( pe->q[ (k+3)%4 ] );
          // ^-number to fill into the tuple // new

      PyTuple_SET_ITEM( pQTuple, k, pNum );  // steals pNum
    }


  // creating list of tuples with vertices ( P )
  PyObject* pPiList = PyList_New ( pe->P.size () ); // new

  for ( Py_ssize_t j = 0; j < Py_ssize_t ( pe->P.size () ); ++j )
    {
      // setting tuples
      PyObject* pPTuple = PyTuple_New ( 3 ); // new

      for ( Py_ssize_t k = 0; k < 3; ++k )
        {
          PyObject* pNum = PyFloat_FromDouble ( pe->P[j][k] );
              // ^-number to fill into the tuple // new
          PyTuple_SET_ITEM( pPTuple, k, pNum );  // steals pNum
        }
      // inserting tuples
      PyList_SetItem( pPiList, j, pPTuple ); // steals pPTuple // discard previous value, if it existed
    }


  // preparing rotation vector
  PyObject* pWTuple = PyTuple_New ( 3 ); // new

  for ( Py_ssize_t k = 0; k < 3; ++k )
    {
      PyObject* pNum = PyFloat_FromDouble ( pe->W[k] );
          // ^-number to fill into the tuple // new
      PyTuple_SET_ITEM( pWTuple, k, pNum );  // steals pNum
    }


  // preparing force vector
  PyObject* pFTuple = PyTuple_New ( 3 ); // new

  for ( Py_ssize_t k = 0; k < 3; ++k )
    {
      PyObject* pNum = PyFloat_FromDouble ( pe->F[k] );
          // ^-number to fill into the tuple // new
      PyTuple_SET_ITEM( pFTuple, k, pNum );  // steals pNum
    }


  // calling the 'monitor' method with all the arguments
  // !! synchronized with python
  PyObject* pReturnValue =      // new
      PyObject_CallMethod ( pSiku, fname, "(O,O,O,I,I,k,O,O,d,d,d,d,d,d,d)",
                            pCurTime,       // time

                            pQTuple,        // quat
                            pPiList,        // vertices
                            pe->flag,       // state flag
                            i,              // index in array
                            pe->id,         // id of element
                            pWTuple,        // angle velocity
                            pFTuple,        // current force
                            pe->N,          // torque (2d)

                            pe->m,          // mass
                            pe->I,          // moment of inertia
                            pe->i,          // geometric moment of inertia
                            pe->A,          // area (on unit sphere)
                            pe->anchority,  // water interaction factor
                            pe->windage     // wind interaction factor
                            );  //new

  if ( !pReturnValue )
    PyErr_Print ();
  else
    Py_DECREF( pReturnValue );
  Py_DECREF( pQTuple );
  Py_DECREF( pPiList );
  Py_DECREF( pWTuple );
  Py_DECREF( pFTuple );

  return status;
}

//---------------------------------------------------------------------

int
Sikupy::fcall_diagnostics_vec3d ( const Globals& siku, const size_t i,
                                  const vector < vec3d >& data )
{
  int status = FCALL_OK;

  // get direct pointer to function object
  PyObject* pFunc = pSiku_funcs[i];

  // creating the list of tuples3 with data

  PyObject* pDataList = PyList_New ( data.size () ); // new

  for ( Py_ssize_t j = 0; j < Py_ssize_t ( data.size () ); ++j ) // <- this conversion scares me
    {
      // setting tuples
      PyObject* pPTuple = PyTuple_New ( 3 ); // new

      for ( Py_ssize_t k = 0; k < 3; ++k )
        {
          PyObject* pNum = PyFloat_FromDouble ( data[j][k] );
              // ^- number to fill into the tuple // new
          PyTuple_SET_ITEM( pPTuple, k, pNum );  // steals pNum
        }

      // inserting tuples
      //PyList_SET_ITEM( pDataList, j, pPTuple ); // steals pPTuple
      PyList_SetItem( pDataList, j, pPTuple ); // steals pPTuple // discard previous value, if it existed
    }

//  // setting the arguments
//  PyObject* pargs;
//  pargs = Py_BuildValue ( "(O,O)", pCurTime, pDataList ); // new
//  assert( pargs );
//
//  // calling the object
//  PyObject* pReturnValue;
//  pReturnValue = PyObject_CallObject ( pFunc, pargs ); // new
//  if ( !pReturnValue )
//    PyErr_Print ();
//  assert( pReturnValue );

  PyObject* pReturnValue =
      PyObject_CallFunction ( pFunc, "(O,O)", pCurTime, pDataList ); //new

  if ( !pReturnValue )
    PyErr_Print ();
  else
    Py_DECREF( pReturnValue );

  if ( pDataList )
    Py_DECREF( pDataList );

  return status;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//           PRIVATE METHODS FOR WORKING WITH PYTHON
//---------------------------------------------------------------------
//---------------------------------------------------------------------

//! \brief reading quaternion (so far from a 4-tuple)
bool
Sikupy::read_quat ( PyObject* pquat, quat& q )
{
  // check if it is a list
  if ( !PyList_Check( pquat ) )
    return false;
  Py_ssize_t K = PyList_Size ( pquat );

  if ( K != 4 )
    return false; // must be 4 elements of q

  for ( Py_ssize_t k = 0; k < K; ++k )
    {
      ///////////////ERROR WAS HERE!!!!
      // mathutil.quat has wrong indexes order: ( (vec), scal )

      PyObject* pitem;
      pitem = PyList_GetItem ( pquat, k ); // borrowed
      if ( !PyFloat_Check( pitem ) )
        return false;

      // fixing indexes
      q[ (k+3)%K ] = PyFloat_AS_DOUBLE( pitem );
      //q[k] = PyFloat_AS_DOUBLE( pitem );
    }

  return true;
}

//! \brief reading unsigned long number
bool
Sikupy::read_ulong ( PyObject* pLong, unsigned long& x )
{
  // extract u_long from PyObject with error checking
  PyErr_Clear ();

  x = PyLong_AsUnsignedLong ( pLong );

  if ( PyErr_Occurred () ) // Returns borrowed ref, so no pointer is needed
    return false;
  return true;
}

//! \brief reading double/float number
bool
Sikupy::read_double ( PyObject* pfloat, double& x )
{
  // extract double from PyObject with error checking
  PyErr_Clear ();

  x = PyFloat_AsDouble ( pfloat );

  if ( PyErr_Occurred () ) // Returns borrowed ref, so no pointer is needed
    return false;
  return true;

//  // OLD!!
//  // check if pquat is the correct type
//  if (!( PyFloat_Check(pfloat) or PyLong_Check(pfloat)))
//    return false;
//
//  x = PyFloat_AsDouble (pfloat);
//
//  return true;
}

//! \brief reading long int number
bool
Sikupy::read_long ( PyObject* plong, long& x )
{

  // check if pquat is the correct type
  if ( !PyLong_Check( plong ) )
    return false;

  x = PyLong_AsLong ( plong );

  return true;
}

//! \brief reading string field (creating new!)
bool
Sikupy::read_string ( PyObject* pstring, string& str )
{
  if ( !PyUnicode_Check( pstring ) )
    return false;

  str = string ( PyUnicode_AsUTF8 ( pstring ) );

  return true;
}

//! \brief reading string list to vector
bool
Sikupy::read_string_vector ( PyObject* pstrlist, vector < string >& strv )
{
  assert( PyList_Check( pstrlist ) );
  Py_ssize_t N = PyList_Size ( pstrlist );

  // array sets length
  strv.resize ( N );

  // reading all the materials in this loop
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( pstrlist, i ); // borrowed

      // type check
      if ( !PyUnicode_Check( pitem ) )
        return false;

      strv[i] = string ( PyUnicode_AsUTF8 ( pitem ) );
    }

  return true;
}

bool
Sikupy::read_long_vector ( PyObject* plist, vector < long >& xs )
{
  assert( PyList_Check( plist ) );
  Py_ssize_t N = PyList_Size ( plist );

  // array sets length
  xs.resize ( N );

  // reading all the materials in this loop
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( plist, i ); // borrowed

      if( !read_long( pitem, xs[i] ) )
        return false;
    }

  return true;
}

bool
Sikupy::read_double_vector ( PyObject* plist, vector < double >& xs )
{
  assert( PyList_Check( plist ) );
  Py_ssize_t N = PyList_Size ( plist );

  // array sets length
  xs.resize ( N );

  // reading all the materials in this loop
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( plist, i ); // borrowed

      if( !read_double( pitem, xs[i] ) )
        return false;

//// Deprecated due to a bug!
//      // check if it is a number
//      if ( !PyFloat_Check( pitem ) )
//        return false;
//
//      xs[i] = PyFloat_AS_DOUBLE( pitem );
    }

  return true;
}

//---------------------------------------------------------------------

bool Sikupy::read_str_doub_map ( PyObject* pDict, map < string, double >& m )
{
  assert( PyDict_Check( pDict ) );
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  string str;
  double d;
  PyErr_Clear ();

  while ( PyDict_Next( pDict, &pos, &key, &value ) )
    {
      d = PyFloat_AsDouble ( value );
      if ( PyErr_Occurred () )  return false;

      str = string ( PyUnicode_AsUTF8 ( key ) );
      if ( PyErr_Occurred () )  return false;

      m[str] = d;
    }

  return true;
}

//---------------------------------------------------------------------

/*
 It reads list of size 3 tuples, the list of vectors.
 */
bool
Sikupy::read_vec3d_vector ( PyObject* plist, vector < vec3d >& vs )
{
  assert( PyList_Check( plist ) );
  Py_ssize_t N = PyList_Size ( plist );

  // array sets length
  vs.resize ( N );

  // reading all the materials in this loop
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      PyObject* pitem;
      pitem = PyList_GetItem ( plist, i ); // borrowed

      // check if it is a list
      if ( !PyTuple_Check( pitem ) ) return false;

      Py_ssize_t K = PyTuple_Size ( pitem );
      if ( K != 3 ) return false; // must be 3D vector

      for ( Py_ssize_t k = 0; k < K; ++k )
        {
          PyObject* psubitem;
          psubitem = PyTuple_GetItem ( pitem, k ); // borrowed

          read_double( psubitem, vs[i][k] );

//// deprecated!
//          if ( !PyFloat_Check( psubitem ) )
//            return false;
//          vs[i][k] = PyFloat_AS_DOUBLE( psubitem );
        }
    }

  return true;
}

bool
Sikupy::read_vec3d ( PyObject* pVec, vec3d& v )
{
  bool success = true;
  // check if it is a tuple of correct size
  if ( !PyTuple_Check( pVec ) )
    return false;

  Py_ssize_t K = PyTuple_Size ( pVec );
  if ( K != 3 )
    return false; // must be 3D vector

  // reading tuple items with errors detection
  for ( Py_ssize_t k = 0; k < K; ++k )
   {
     PyObject* pItem = PyTuple_GetItem ( pVec, k ); // borrowed
     success ^= read_double( pItem, v[k] );
   }

  return success;
}

//---------------------------------------------------------------------

using namespace boost::posix_time;
using namespace boost::gregorian;

//! \brief reading time_duration object
bool
Sikupy::read_dt ( PyObject* pobj, boost::posix_time::time_duration& dt )
{
  if ( !PyDelta_Check( pobj ) )
    return false;

  // temp values
  int day, second, ms;

  // we need to get it field by field
  PyObject* pitem;

  pitem = PyObject_GetAttrString ( pobj, "days" ); // new
  assert( pitem );
  if ( !PyLong_Check( pitem ) )
    return false;
  day = PyLong_AsLong ( pitem );
  Py_DECREF( pitem );           // done

  pitem = PyObject_GetAttrString ( pobj, "seconds" ); // new
  assert( pitem );
  if ( !PyLong_Check( pitem ) )
    return false;
  second = PyLong_AsLong ( pitem );
  Py_DECREF( pitem );           // done

  pitem = PyObject_GetAttrString ( pobj, "microseconds" ); // new
  assert( pitem );
  if ( !PyLong_Check( pitem ) )
    return false;
  ms = PyLong_AsLong ( pitem ) / 1000;
  Py_DECREF( pitem );           // done

  dt = hours ( day * 24 ) + seconds ( second ) + millisec ( ms );

  return true;
}

//---------------------------------------------------------------------

bool
Sikupy::read_time ( PyObject* pobj, boost::posix_time::ptime& t )
{
  int year, month, day, hour, minute, second, ms;

  if ( !PyDateTime_Check( pobj ) )
    return false;

  year = PyDateTime_GET_YEAR( pobj );
  month = PyDateTime_GET_MONTH( pobj );
  day = PyDateTime_GET_DAY( pobj );
  hour = PyDateTime_DATE_GET_HOUR( pobj );
  minute = PyDateTime_DATE_GET_MINUTE( pobj );
  second = PyDateTime_DATE_GET_SECOND( pobj );
  ms = PyDateTime_DATE_GET_MICROSECOND( pobj ) / 1000;

  date start_date ( year, month, day );
  t = ptime (
      start_date,
      hours ( hour ) + minutes ( minute ) + seconds ( second )
          + millisec ( ms ) );

  return true;
}

