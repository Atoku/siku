/*!

  \file lowio.cc

  \brief Lowio implementation

*/

//---------------------------------------------------------------------

#include "element.hh"
#include "lowio.hh"
#include "modeltime.hh"

// Macro to register a compound odatatype
#define dtype_freg( dtype, structure, field, ftype )           \
  {                                                            \
    H5Tinsert( dtype,                                          \
               #field,                                         \
               HOFFSET( structure, field ),                    \
               ftype );                                        \
  }

//---------------------------------------------------------------------

// Macro to create a compound datatype using a structure or type
#define dtype_create( dtype, structure )                           \
  {                                                                \
    dtype = H5Tcreate ( H5T_COMPOUND, sizeof (structure) );        \
  }

//---------------------------------------------------------------------

Lowio::Lowio()
{
  // datatypes for standard data
  stdtypes.t_int    = H5T_NATIVE_INT;
  stdtypes.t_long   = H5T_NATIVE_LONG;
  stdtypes.t_double = H5T_NATIVE_DOUBLE;

  // Registering the datatypes

  // quat
  stdtypes.t_quat = H5Tcreate( H5T_COMPOUND, sizeof(double)*4 );
  H5Tinsert( stdtypes.t_quat, "a", 0,   stdtypes.t_double );
  H5Tinsert( stdtypes.t_quat, "b", 8,   stdtypes.t_double );
  H5Tinsert( stdtypes.t_quat, "c", 2*8, stdtypes.t_double );
  H5Tinsert( stdtypes.t_quat, "d", 3*8, stdtypes.t_double );

  // time
  typedef ModelTimeTypes::timestamp mytime; // for short
  stdtypes.t_time =  H5Tcreate( H5T_COMPOUND, sizeof( mytime ) );
  dtype_freg( stdtypes.t_time, mytime, year, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, month, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, day, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, hours, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, minutes, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, seconds, stdtypes.t_int );
  dtype_freg( stdtypes.t_time, mytime, microseconds, stdtypes.t_long );

  typedef ModelTimeTypes::dtstamp mydt; // for short
  stdtypes.t_dt =  H5Tcreate( H5T_COMPOUND, sizeof( mydt ) );
  dtype_freg( stdtypes.t_dt, mydt, hours, stdtypes.t_long );
  dtype_freg( stdtypes.t_dt, mydt, minutes, stdtypes.t_long );
  dtype_freg( stdtypes.t_dt, mydt, seconds, stdtypes.t_long );
  dtype_freg( stdtypes.t_dt, mydt, microseconds, stdtypes.t_long );
  
  // element  //!!! TESTING
  typedef Element myel; // for short
  stdtypes.t_element=  H5Tcreate( H5T_COMPOUND, sizeof( myel ) );
  dtype_freg( stdtypes.t_element, myel, q, stdtypes.t_quat );
  //!!! TESTING -^

}

//---------------------------------------------------------------------

Lowio::~Lowio()
{
  H5Tclose( stdtypes.t_quat );
  //H5Tclose( stdtypes.t_element);
}

//---------------------------------------------------------------------

void Lowio::init( const string& param_filename, const unsigned int access )
{
  filename = param_filename;
  flags = access;

  /* we either create file or open it depending on access type */
  /* we create the group Time if access is right */
  if ( access == Lowio::ACCESS_F_OVERWRITE ||
       access == Lowio::ACCESS_F_EXCLUSIVE )
    {
      fileid = H5Fcreate( filename.c_str(), access, 
                          H5P_DEFAULT, H5P_DEFAULT);
      group_time_id = H5Gcreate( fileid, "/Time", 
                                 H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      group_data_id = H5Gcreate( fileid, "/Data", 
                                 H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
  else
    {
      fileid = H5Fopen ( filename.c_str(), access, H5P_DEFAULT );
    }
}


//---------------------------------------------------------------------

void Lowio::save_global_attribute_string( const string& aname, const string& avalue )
{
  hid_t attr_dspace;
  hid_t attr_strtype;
  hid_t attr;

  attr_dspace  = H5Screate( H5S_SCALAR );
  attr_strtype = H5Tcopy( H5T_C_S1 );

  H5Tset_size( attr_strtype, avalue.size() );
  attr = H5Acreate( fileid, aname.c_str(),
                    attr_strtype, attr_dspace,
                    H5P_DEFAULT, H5P_DEFAULT );

  H5Awrite( attr, attr_strtype, avalue.c_str() );

  H5Aclose( attr );
  H5Sclose ( attr_dspace );
}

/* ----------------------------------------------------------------- */

int Lowio::save( const hid_t dtype,           //!< data type
                 const string& dataname,      //!< name for dataset
                 const void* data,                  /* array */
                 const int len,               /* length of array */
                 const string& units,     /* string with units */
                 const string& description /* description string */  )
{
  herr_t status;                /* error code */
  hid_t dataspace, dataset;     /* dataspace and dataset for HDF5 */

  /* dataspace creation depends on the len value */
  if ( len == 0 )
    {
      dataspace = H5Screate ( H5S_SCALAR );
    }
  else
    {
      hsize_t dims[1];              /* dims[0] = length of array */
      dims[0] = len;
      dataspace = H5Screate_simple (1, dims, NULL );
    }
  assert( dataspace >= 0 );

  /* dataset creation is the same */
  dataset = H5Dcreate ( fileid, dataname.c_str(),
                        dtype, dataspace,
                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  assert( dataset >= 0 );

  status = H5Dwrite ( dataset, dtype,
                      H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      data );

  /* Saving units and long_names if they are not NULL */
  if ( units.size() != 0 )
     save_attribute( dataset, TITLE_UNITS, units );

  if ( description.size() != 0 )
    save_attribute( dataset, TITLE_DESCRIPTION, description );

  /* -- END attributes */

  /* freeing memory from dataset and dataspace in HDF */
  H5Dclose (dataset);
  H5Sclose (dataspace);

  return status;

}

//---------------------------------------------------------------------

int Lowio::save_value( const hid_t dtype,    /* data type */
                       const string& dataname,  /* name for dataset */
                       const void* data,            /* pointer to the data */
                       const string& units,     /* string with units */
                       const string& description /* description string */ )
{
  return save( dtype, dataname, data, 0, units, description );
}

//---------------------------------------------------------------------

int Lowio::save_array( const hid_t dtype,    /* data type */
                       const string& dataname,  /* name for dataset */
                       const void* data,            /* array */
                       const int len,               /* length of array */
                       const string& units,     /* string with units */
                       const string& description /* description string */
                     )
{
  return save( dtype, dataname, data, len, units, description );
}

//---------------------------------------------------------------------

int Lowio::save_astrings ( const vector<string>& strs,
                           const string& dataname,  
                           const string& description )
{
  // We start from getting pS and N
  char** pS;
  size_t N;

  // We have to copy all the data from strs as the only chance to get
  // a proper pointer: this is the cost of C++ here.
  N = strs.size();
  pS = (char**) malloc ( N * sizeof( char* ) );
  for ( size_t i = 0; i < N; ++i )
    {
      pS[i] = (char*) malloc( strs[i].size() * ( 1 + sizeof( char ) ) );
      strcpy( pS[i], strs[i].c_str() );
    }

  // saving itself
  herr_t status;                /* error code */
  hid_t dataspace, dataset;   /* dataspace and dataset for HDF5 */
  hid_t hid_str;              /* datatype variable length string */

  /* create a string of variable length */
  hid_str = H5Tcopy( H5T_C_S1 );
  H5Tset_size( hid_str, H5T_VARIABLE );

  /* dataspace for array of something of length N */
  hsize_t dims[1];
  dims[0] = N;
  dataspace = H5Screate_simple( 1, dims, NULL );
  assert( dataspace >= 0 );

  /* dataset creation */
  dataset = H5Dcreate( fileid, dataname.c_str(),
                       hid_str, dataspace,
                       H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  assert( dataset >= 0 );

  status = H5Dwrite ( dataset, hid_str,
                      H5S_ALL, H5S_ALL, H5P_DEFAULT,
                      pS );

  /* save attribute description */
  if ( description.size() != 0 )
    save_attribute( dataset, "Description", description );

  // freeing memory from temp buffer
  for ( size_t i = 0; i < N; ++i )     free( pS[i] );
  free( pS );

  /* freeing memory from dataset and dataspace in HDF */
  H5Dclose (dataset);
  H5Sclose (dataspace);
  H5Tclose ( hid_str );

  return status;
}


//---------------------------------------------------------------------

void Lowio::save_attribute( hid_t dataset, 
                            const string& aname, 
                            const string& s )
{
  hid_t attr_dspace;
  hid_t attr_strtype;
  hid_t attr;

  attr_dspace  = H5Screate( H5S_SCALAR );
  attr_strtype = H5Tcopy( H5T_C_S1 );

  H5Tset_size( attr_strtype, s.size() );
  attr = H5Acreate( dataset, aname.c_str(),
                    attr_strtype, attr_dspace,
                    H5P_DEFAULT, H5P_DEFAULT );

  H5Awrite( attr, attr_strtype, s.c_str() );

  H5Aclose( attr );
  H5Sclose ( attr_dspace );
}

//---------------------------------------------------------------------

size_t Lowio::get_dim( const string& name )
{
  /* first we check if the dataset exists */
  if ( ! H5Lexists( fileid, name.c_str(), H5P_DEFAULT ) )
    return DATASET_MISSING;

  hid_t dataset = H5Dopen( fileid, name.c_str(), H5P_DEFAULT );
  assert( dataset >= 0 );

  hid_t filespace = H5Dget_space( dataset );
  hsize_t dims[1];              /* dims[0] = length of array */
  (void) H5Sget_simple_extent_dims( filespace, dims, NULL );

  H5Sclose( filespace );
  H5Dclose( dataset );

  return dims[0];
}

//---------------------------------------------------------------------

int Lowio::read( const string& name, void* buff )
{
  /* first we check if the dataset exists */
  if ( ! H5Lexists( fileid, name.c_str(), H5P_DEFAULT ) )
    return DATASET_MISSING;

  hid_t dataset = H5Dopen( fileid, name.c_str(), H5P_DEFAULT );
  assert( dataset >= 0 );

  hid_t datatype = H5Dget_type( dataset );

  int status = H5Dread( dataset, datatype, H5S_ALL,
                        H5S_ALL, H5P_DEFAULT, buff );

  H5Dclose( dataset );
  return status;
}

//---------------------------------------------------------------------

void Lowio::release()
{
  /* close the groups */
  if ( group_time_id != EMPTY_ID ) H5Gclose( group_time_id );
  if ( group_data_id != EMPTY_ID ) H5Gclose( group_data_id );

  /* close the file */
  if ( fileid != EMPTY_ID ) H5Fclose( fileid );
}

//---------------------------------------------------------------------
