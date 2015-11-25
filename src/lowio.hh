/*!

  \file lowio.hh

  \brief low level IO: the Siku interface to HDF5

*/

#ifndef LOWIO_HH
#define LOWIO_HH

#include <string>
#include <vector>
using namespace std;

extern "C" {
#include <hdf5.h>
}

#include "globals.hh"

//! \brief Class for working with HDF5 in Siku 
class Lowio
{
public:
  //! \brief highio can access private members (actually add for stdtypes)
  friend class Highio;

  //! \brief File access for init function to overwrite the file
  const unsigned int ACCESS_F_OVERWRITE     { H5F_ACC_TRUNC };
  const unsigned int ACCESS_F_EXCLUSIVE     { H5F_ACC_EXCL };
  const unsigned int ACCESS_F_READONLY      { H5F_ACC_RDONLY };
  const unsigned int ACCESS_F_RDWR          { H5F_ACC_RDWR };
  const hid_t        EMPTY_ID               { 0 };

  //! \brief no dataset found error message for read functions
  const size_t DATASET_MISSING                 { size_t(-1) };

  //! \brief Mostly constructing the datatypes
  Lowio();

  //! \brief Freeing the constructed datatypes 
  ~Lowio();

  //! \brief Starting working with the file
  //! \param[in] filename file name to work with
  //! \param[in] access one of the ACCESS_F codes
  void init( const string& filename, 
             const unsigned int access );

  //! \brief saves global string attribute into a file
  //! \param[in] aname Attribute name
  //! \param[in] avalue Attribute value
  void save_global_attribute_string( const string& aname, 
                                     const string& avalue );

  //! \brief Save a simple value
  //! \param[in] dtype data type (use Types functions)
  //! \param[in] dataname name for the dataset
  //! \param[in] data pointer to the data to save
  //! \param[in] units string with units description
  //! \param[in] description string with data description
  int save_value( const hid_t dtype,
                  const string& dataname,
                  const void* data,
                  const string& units,
                  const string& description );

  //! \brief Save an array
  //! \param[in] dtype data type of elements (use Types functions)
  //! \param[in] dataname name for the dataset
  //! \param[in] data pointer to the data to save
  //! \param[in] len the length of the array
  //! \param[in] units string with units description
  //! \param[in] description string with data description
  int save_array( const hid_t dtype,
                  const string& dataname,
                  const void* data,
                  const int len,
                  const string& units,
                  const string& description );

  int save_string ( const string& name,
                    const string& str,
                    const string& units,
                    const string& description );

  int save_astrings ( const vector<string>& strs,
                      const string& dataname,  
                      const string& description );

  //! \brief getting dimension of the dataset
  //! \param[in] name name of the dataset
  size_t get_dim( const string& name );

  //! \brief read variable into a buffer
  //! \param[in] name name of the dataset
  //! \param[out] buff enough memory to hold the data
  int read( const string& name, void* buff );

//////////////////////////////////////////////////////////////////////////
  int save_material( const string& location, void* pmat,
                     const string& description );

  // Types -----

  //! \brief returns element type 
  hid_t type_element() const { return stdtypes.t_element; };

  //! \brief returns time timestamp type
  hid_t type_time() const { return stdtypes.t_time; };

  //! \brief returns time ststamp type
  hid_t type_dt() const { return stdtypes.t_dt; };

  //! \brief returns time vertex type
  hid_t type_vert() const { return stdtypes.t_vertex; };

  //! \brief closes the file and releases the memory. Note: it is not
  //! in a destructor as we might want to use the same object many
  //! times for different files
  void release();

private:

  hid_t fileid { EMPTY_ID };    //!< File ID with file to work with 
  unsigned int flags { 0 };           //!< Some extra info
  string filename;              //!< File name
  hid_t group_time_id { EMPTY_ID };  //!< Group ID for time data
  hid_t group_data_id { EMPTY_ID };  //!< Group ID for numerical data data 

  //! \brief Main types that are created on construction
  struct stdtypes_s
  {
    // basic
    hid_t t_int;
    hid_t t_long;
    hid_t t_double;
    hid_t t_uint;
    hid_t t_ulong;
    hid_t t_size;
    hid_t t_char;
    hid_t t_bool;
    hid_t t_string;

    // composite
    hid_t t_vec;
    hid_t t_quat;
    hid_t t_vertex;
    hid_t t_contact;
    hid_t t_gridnode;
    hid_t t_matlayer;
    hid_t t_matlayarr;
    hid_t t_elemgh;
    hid_t t_info;
    hid_t t_planet;

    // complex
    hid_t t_material;
    hid_t t_time;               //!< ModelTimeTypes::timestamp
    hid_t t_dt;                 //!< ModelTimeTypes::dtstamp
    hid_t t_element;            //!< Plain Element
  } stdtypes;

  //! \brief Composite types for large classes` entities
//  struct comptypes_s
//  {
//    hid_t t_info;
//    hid_t t_planet;
//    hid_t t_material;
//    hid_t t_vecfield;
//    hid_t t_;
//  } comptypes;

  //! \brief General save function. len=0 for scalar
  //! \param[in] dtype data type (use Types functions)
  //! \param[in] dataname name for the dataset
  //! \param[in] data pointer to the data to save
  //! \param[in] len =0 for scalar, length of data for the array
  //! \param[in] units string with units description
  //! \param[in] description string with data description
  int save( const hid_t dtype,
            const string& dataname,
            const void* data,
            const int len,
            const string& units,
            const string& description );

  //! \brief Saving attribute for the dataset
  //! \param[in] dataset id of the dataset
  //! \param[in] aname name for the attribute
  //! \param[in] s content of the attribute string
  void save_attribute( const hid_t dataset, 
                       const string& aname, 
                       const string& s );

  //! \brief Some constant strings 
  const string TITLE_UNITS        { "Physical units" };
  const string TITLE_DESCRIPTION  { "Description" };

  // 

};

#endif      /* LOWIO_HH */
