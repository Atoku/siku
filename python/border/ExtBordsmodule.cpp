//============================================================================
// Name        : ExtBoardsmodule.cpp
// Author      : kgill-leebr
// Version     :
// Copyright   : PRIVATE WHILE ON DEV
// Description : Python3 extension
//============================================================================

#include "Python.h"
#undef tolower //...

#include <cmath>
#include <vector>

static PyObject *ExtError;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ inner types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct LL
{
  double lon;
  double lat;

  //LL(): lon(0.), lat(0.) {}
  LL(const double& lo = 0., const double& la = 0.) : lon(lo), lat(la) {}
};

struct XYZ
{
  double x;
  double y;
  double z;

  //XYZ(): x(0.), y(0.), z(0.) {}
  XYZ(const double& xx = 0., const double& yy = 0., const double& zz = 0.) :
    x(xx), y(yy), z(zz) {}
};

struct RTP
{
  double r;
  double theta;
  double phi;

  RTP(const double& rr = 0., const double& tt = 0., const double& pp = 0.) :
    r(rr), theta(tt), phi(pp) {}
};

//-------------------------- private methods ----------------------------------

//! \brief converts degrees to radians
inline double deg_to_rad( const double& deg ){ return deg * M_PI / 180.; }

//! \brief converts radians to degrees
inline double rad_to_deg( const double& rad ){ return rad * 180. / M_PI; }

//! normalizing latitude in range [ -90, 90 ]
inline double norm_lat( double lat )
{
  while( lat > 90. )
    lat -= 90.;
  while( lat < -90. )
    lat += 90.;
  return lat;
}

//! normalizing longitude in range [0, 360]
inline double norm_lon( double lon )
{
  while( lon >= 360. )
    lon -= 360.;
  while( lon < 0. )
    lon += 360.;
  return lon;
}

//! converting Cartesian xyz coordinates into geographical lon-lat (deg)
LL xyz_to_ll ( const XYZ& xyz )
{
  double theta = atan2( sqrt( xyz.x * xyz.x + xyz.y * xyz.y ), xyz.z );
  double phi = atan2( xyz.y, xyz.x );
  return LL( norm_lon( rad_to_deg( phi ) ),
             norm_lat( rad_to_deg( M_PI*0.5 - theta ) ) );
}

//! converting geographical lon-lat (deg) coordinates into Cartesian xyz
//! (on unit sphere)
XYZ ll_to_xyz ( const LL& ll )
{
  double phi = deg_to_rad( ll.lon );
  double theta = M_PI*0.5 - deg_to_rad( ll.lat );
  double st = sin( theta );
  double ct = cos( theta );
  double sp = sin( phi );
  double cp = cos( phi );
  return XYZ( st*cp, st*sp, ct );
}

//! converting Cartesian xyz coordinates into spherical r-theta-phi (rad)
RTP xyz_to_rtp ( const XYZ& xyz )
{
  double r = sqrt( xyz.x * xyz.x + xyz.y * xyz.y + xyz.z * xyz.z );
  return RTP( r, atan2( xyz.y, xyz.x ),  M_PI*0.5 - acos( xyz.z / r ) );
}

//! converting spherical r-theta-phi (rad) coordinates into Cartesian xyz
XYZ rtp_to_xyz ( const RTP& rtp )
{
  double st = sin( rtp.theta );
  double ct = cos( rtp.theta );
  double sp = sin( rtp.phi );
  double cp = cos( rtp.phi );
  return XYZ( rtp.r * st * cp, rtp.r* st * sp, rtp.r * ct );
}

//! domain check
inline bool is_inside( const LL& ll, const double& minlon, const double& maxlon,
                       const double& minlat, const double& maxlat )
{
  return ll.lon >= minlon && ll.lon <= maxlon
      && ll.lat >= minlat && ll.lat <= maxlat;
}

//! dot product
inline double dot( const XYZ& p1, const XYZ& p2 )
{
  return p1.x*p2.x + p1.y*p2.y + p1.z*p2.z;
}

// ============================ public methods ==============================

//! filter contours by angular density
static PyObject* Ext_filter_contours( PyObject* self, PyObject *args)
{
  PyObject* pypnts;
  PyObject* pyitem;
  PyObject* ret;
  double dens, minlon, maxlon, minlat, maxlat;
  std::vector<LL> coords;
  std::vector<LL> exterior;
  std::vector<XYZ> points;
  LL ll;

  // parsing args
  if( !PyArg_ParseTuple(args, "Od(dddd)", &pypnts, &dens, &minlon, &maxlon,
                        &minlat, &maxlat ) ) // borrowed
      return NULL;

  // density is given in degrees
  double comp = cos( deg_to_rad ( dens ) );

  // list preparations
  assert( PyList_Check( pypnts ) );
  Py_ssize_t N = PyList_Size ( pypnts );

  // parsing list
  for ( Py_ssize_t i = 0; i < N; ++i )
    {
      pyitem = PyList_GetItem ( pypnts, i ); // borrowed

      ll = LL ( PyFloat_AsDouble( PyList_GetItem( pyitem, 0 ) ),
                PyFloat_AsDouble( PyList_GetItem( pyitem, 1 ) ) );

      if( is_inside( ll, minlon, maxlon, minlat, maxlat ) )
        coords.push_back( ll );
      else
        exterior.push_back( ll );
    }

  // sizing
  N = coords.size();
  points.resize( N );

  // converting to xyz
  for( size_t i = 0; i < (size_t)N; ++i )
    points[i] = ll_to_xyz( coords[i] );

  // filtering
  size_t size = points.size();
  for( size_t i = 0; i < size - 1; ++i )
    for( size_t j = i + 1; j < size; ++j )
      if( dot( points[i], points[j] ) > comp ) // if too close - remove point
        {
          points[j] = points[--size];
          points.pop_back();
        }

  // sizing
  coords.resize( size );

  // converting back to ll
  for( size_t i = 0; i < size; ++i )
      coords[i] = xyz_to_ll( points[i] );

  // appending exterior
  coords.insert( coords.end(), exterior.begin(), exterior.end() );

  // building return value
  ret = PyList_New( coords.size() );
  for( size_t i = 0; i < coords.size(); ++i )
    {
      pyitem = PyList_New( 2 ); // new
      PyList_SetItem( pyitem, 0, PyFloat_FromDouble( coords[i].lon ) ); //steal
      PyList_SetItem( pyitem, 1, PyFloat_FromDouble( coords[i].lat ) ); //steal

      PyList_SetItem( ret, i, pyitem );  //steal
    }

  return ret;
}


//static PyObject* Ext_add( PyObject* self, PyObject* args)
//{
//  double a, b;
//  double sts=0;
//  if(!PyArg_ParseTuple( args, "dd", &a, &b))
//    {
//      return NULL;
//    }
//
//  sts = a + b;
//
//  return Py_BuildValue("d", sts);
//}

// ======================== registration of methods =========================

static PyMethodDef ExtBoard_methods[] =
    {
        {"filter_contours", Ext_filter_contours, METH_VARARGS,
         "filter contour points by angular density inside specified domain"},
        //{"add", Ext_add, METH_VARARGS, "Add two numbers in C++"},
        {NULL, NULL, 0, NULL}
    };

//--------------------------- module initialization --------------------------

static struct PyModuleDef extmodule = {
   PyModuleDef_HEAD_INIT,
   "extboards",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   ExtBoard_methods
};

PyMODINIT_FUNC PyInit_ExtBoards(void)
{
  PyObject *m;

  m = PyModule_Create(&extmodule);
  if (m == NULL)
      return NULL;

  ExtError = PyErr_NewException("ExtBoards.error", NULL, NULL);
  Py_INCREF(ExtError);
  PyModule_AddObject(m, "error", ExtError);
  return m;
}
