/*!

  \file highio.cc

  \brief Implementation of saving and reading variables

*/

#include "highio.hh"

int Highio::save( const Globals& siku )
{
  int status = STATUS_OK;

  lowio.init( siku.savefile, lowio.ACCESS_F_OVERWRITE ); // writing to filename

  // saving attributes
  lowio.save_global_attribute_string( "model_name", siku.info.name );
  lowio.save_global_attribute_string( "model_version", siku.info.version );
  lowio.save_global_attribute_string( "model_description", siku.info.brief );
  lowio.save_global_attribute_string( "model_program_date", siku.info.date );
  lowio.save_global_attribute_string( "model_run_start_date", siku.info.rundate );

  // saving time

  ModelTimeTypes::timestamp mt_timestamp;

  siku.time.get_current_as_timestamp( &mt_timestamp );
  lowio.save_value( lowio.type_time(), "Time/Current",
                    &mt_timestamp, "Current model time for data", "TODO: fill" );

  siku.time.get_start_as_timestamp( &mt_timestamp );
  lowio.save_value( lowio.type_time(), "Time/Start",
                    &mt_timestamp, "Starting computation time", "TODO: fill" );

  siku.time.get_finish_as_timestamp( &mt_timestamp );
  lowio.save_value( lowio.type_time(), "Time/Finish",
                    &mt_timestamp, "Ending computation time", "TODO: fill" );

  // saving dt
  ModelTimeTypes::dtstamp mt_dt;

  siku.time.get_dt_as_dtstamp( &mt_dt );
  lowio.save_value( lowio.type_dt(), "Time/dt",
                    &mt_dt, "Time step", "TODO: fill" );

  siku.time.get_dts_as_dtstamp( &mt_dt );
  lowio.save_value( lowio.type_dt(), "Time/dts",
                    &mt_dt, "Saving frequency time step", "TODO: fill" );
  

  // saving elements
  lowio.save_array( lowio.type_element(), "Elements/Elements",
                    siku.es.data(), siku.es.size(), "TODO: fill", "TODO: fill" );

  // saving element groups
  vector<string> astrs;
  astrs.push_back( "Hello" );
  astrs.push_back( "Bye" );
  astrs.push_back( "Good morning" );
  string dataname( "dataname_forstr" );
  string description = string( "description string" );

  lowio.save_astrings ( astrs, dataname, description );

  // saving monitors
  lowio.save_astrings( siku.mons, string( "Monitor functions" ),
                       string( "TODO: fill" ) );

  // saving controls
  lowio.save_astrings( siku.cons, string( "Control functions" ),
                       string( "TODO: fill" ) );

  // saving polygon vertices
  presave_verts( siku );
  lowio.save_array( lowio.type_vert(), "Elements/Vertices",
                      verts.data(), verts.size(), "TODO: fill", "TODO: fill" );

  // saving flags and names
  lowio.save_string( string("Boarder File"), siku.board_file,  "TODO: fill",
                     "TODO: fill" );

  lowio.save_string( string("Save File"), siku.savefile,  "TODO: fill",
                     "TODO: fill" );

  lowio.save_value( lowio.stdtypes.t_ulong, string("Boarders flag"),
                    &siku.mark_boarders,  "TODO: fill", "TODO: fill" );

  //-------------------------------- sup calls ----------------------------

  save_info( siku );
  save_planet( siku );
  save_materials( siku );
  save_vecfield( siku );
  save_diagnostics( siku );
  save_condet( siku );

  //---------------------------------------------------------------------
  
  lowio.release();              // and stop working with this file

  return status;
}

//---------------------------------------------------------------------

void Highio::presave_verts( const Globals& siku )
{
  verts.clear();

  for( auto& e : siku.es )
    {
      for( auto& v : e.P )
        {
          verts.push_back( Element::vertex( v, e.id ) );
        }
    }
}

//---------------------------------------------------------------------

void Highio::save_info( const Globals& siku )
{
  lowio.save_string( string("Info/name"), siku.info.name,
                     "TODO: fill", "TODO: fill" );
  lowio.save_string( string("Info/brief"), siku.info.brief,
                       "TODO: fill", "TODO: fill" );
  lowio.save_string( string("Info/version"), siku.info.version,
                       "TODO: fill", "TODO: fill" );
  lowio.save_string( string("Info/date"), siku.info.date,
                       "TODO: fill", "TODO: fill" );
  lowio.save_string( string("Info/rundate"), siku.info.rundate,
                       "TODO: fill", "TODO: fill" );
}

//---------------------------------------------------------------------

void Highio::save_planet( const Globals& siku )
{
  lowio.save_value( lowio.stdtypes.t_double, "Planet/R",
                      &siku.planet.R, "TODO: fill", "TODO: fill" );
  lowio.save_value( lowio.stdtypes.t_double, "Planet/R_rec",
                      &siku.planet.R_rec, "TODO: fill", "TODO: fill" );
  lowio.save_value( lowio.stdtypes.t_double, "Planet/R2",
                      &siku.planet.R2, "TODO: fill", "TODO: fill" );
  lowio.save_value( lowio.stdtypes.t_double, "Planet/R2_rec",
                      &siku.planet.R2_rec, "TODO: fill", "TODO: fill" );
}

//---------------------------------------------------------------------

void Highio::save_materials( const Globals& siku )
{
  for( size_t i = 0; i < siku.ms.size(); ++i )
    {
      save_material( string("Materials/"), (void*)&siku.ms[i] );
    }
}

//---------------------------------------------------------------------

void Highio::save_vecfield( const Globals& siku )
{
  lowio.save_value(lowio.stdtypes.t_ulong, string("Wind/Source"),
                   &siku.wind.FIELD_SOURCE_TYPE, "TODO: fill", "TODO: fill" );

  switch( siku.wind.FIELD_SOURCE_TYPE )
  {
    case Vecfield::NMC:
      save_nmc( string("Wind/"), (void*)siku.wind.NMCVec );
      break;
  }
}

//---------------------------------------------------------------------

void Highio::save_diagnostics( const Globals& siku )
{
  for( auto& m : siku.diagnostics.windbase )
    {
      save_mesh( string("Diag/Meshes/"), (void*)&m );
    }

  for( auto& w : siku.diagnostics.windbase )
    {
      save_diag( string("Diag/Windbases/"), (void*)&w );
    }
}

//---------------------------------------------------------------------

void Highio::save_condet( const Globals& siku )
{
  lowio.save_value( lowio.stdtypes.t_ulong, string("Contacts/det_method"),
                    &siku.ConDet.det_meth, "TODO: fill", "TODO: fill" );

  lowio.save_array( lowio.stdtypes.t_contact, string("Contacts/Contacts"),
                    siku.ConDet.cont.data(), siku.ConDet.cont.size(),
                    "TODO: fill", "TODO: fill" );
}

//---------------------------------------------------------------------

int Highio::save_diag ( const string& location, void* pdiag )
{
  Diagbase* diag = (Diagbase*) pdiag;
  ModelTimeTypes::dtstamp pmts;
  ModelTimeTypes::timestamp ts;
  int ret = 0;
  ret = lowio.save_value( lowio.stdtypes.t_size, location+string("ifunc"),
                    &diag->ifunc, "TODO: fill", "TODO: fill" );
  ret |= lowio.save_value( lowio.stdtypes.t_size, location+string("imesh"),
                    &diag->imesh, "TODO: fill", "TODO: fill" );
  diag->scheduler.get_dt_as_dtstamp( &pmts );
  ret |= lowio.save_value( lowio.stdtypes.t_dt,
                           location+string("Scheduler/duration"),
                           &pmts, "TODO: fill", "TODO: fill" );
  diag->scheduler.get_tevent_as_timestamp( &ts );
  ret |= lowio.save_value( lowio.stdtypes.t_time,
                           location+string("Scheduler/event"),
                           &ts, "TODO: fill", "TODO: fill" );
  return ret;
}

//---------------------------------------------------------------------

int Highio::save_material ( const string& location, void* pmat )
{
  Material* mat = (Material*) pmat;
  int ret = 0;

  lowio.save_material( location, pmat, string("No taste, no color, nothing") );

//  ret = lowio.save_string( location+string("name"), mat->name, "TODO: fill",
//                           "TODO: fill" );
//  ret |= lowio.save_array( lowio.stdtypes.t_double,
//                           location+string("thic_inters"),
//                           mat->thickness_intervals.data(),
//                           mat->thickness_intervals.size(),
//                           "TODO: fill", "TODO: fill" );
//  ret |= lowio.save_array( lowio.stdtypes.t_double, location+string("rho"),
//                           mat->rho.data(),mat->rho.size(), "TODO: fill",
//                           "TODO: fill" );
//  ret |= lowio.save_array( lowio.stdtypes.t_double, location+string("sigma_c"),
//                           mat->sigma_c.data(), mat->sigma_c.size(),
//                           "TODO: fill", "TODO: fill" );
//  ret |= lowio.save_array( lowio.stdtypes.t_double, location+string("sigma_t"),
//                           mat->sigma_t.data(), mat->sigma_t.size(),
//                           "TODO: fill", "TODO: fill" );
//  ret |= lowio.save_value( lowio.stdtypes.t_double, location+string("E"),
//                           &mat->E, "TODO: fill", "TODO: fill" );
//  ret |= lowio.save_value( lowio.stdtypes.t_double, location+string("nu"),
//                           &mat->nu, "TODO: fill", "TODO: fill" );

//  H5Dcreate)
//  herr_t status;                /* error code */
//  hid_t dataspace, dataset;     /* dataspace and dataset for HDF5 */
//
//  /* dataspace creation depends on the len value */
//  if ( len == 0 )
//    {
//      dataspace = H5Screate ( H5S_SCALAR );
//    }
//  else
//    {
//      hsize_t dims[1];              /* dims[0] = length of array */
//      dims[0] = len;
//      dataspace = H5Screate_simple (1, dims, NULL );
//    }
//  assert( dataspace >= 0 );
//
//  /* dataset creation is the same */
//  dataset = H5Dcreate ( fileid, dataname.c_str(),
//                        dtype, dataspace,
//                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
//  assert( dataset >= 0 );
//
//  status = H5Dwrite ( dataset, dtype,
//                      H5S_ALL, H5S_ALL, H5P_DEFAULT,
//                      data );
//
//  /* Saving units and long_names if they are not NULL */
//  if ( units.size() != 0 )
//     save_attribute( dataset, TITLE_UNITS, units );
//
//  if ( description.size() != 0 )
//    save_attribute( dataset, TITLE_DESCRIPTION, description );
//
//  /* -- END attributes */
//
//  /* freeing memory from dataset and dataspace in HDF */
//  H5Dclose (dataset);
//  H5Sclose (dataspace);
//
//  return status;



  return ret;
}

//---------------------------------------------------------------------

int Highio::save_mesh ( const string& location, void* pmesh )
{
  Mesh* mesh = (Mesh*) pmesh;
  return lowio.save_array( lowio.stdtypes.t_vec, location+string("data"),
                           mesh->data.data(), mesh->data.size(), "TODO: fill",
                           "TODO: fill" );
}

//---------------------------------------------------------------------

int Highio::save_nmc( const string& loc, void* pnmc)
{
  int res;
  NMCVecfield* nmc = (NMCVecfield*) pnmc;
  size_t lons = nmc->get_lon_size(), lats = nmc->get_lat_size();

  res = lowio.save_value( lowio.stdtypes.t_size, loc+string("Size Lon"), &lons,
                          "TODO: fill", "TODO: fill" );
  res |= lowio.save_value( lowio.stdtypes.t_size, loc+string("Size Lat"), &lats,
                          "TODO: fill", "TODO: fill" );

  NMCVecfield::GridNode* raw = new NMCVecfield::GridNode[ lons*lats ];
  for (size_t i = 0; i < lats; i++ )
    for (size_t j = 0; j < lons; j++)
      {
        raw[ lats*j + i ] = nmc->get_node( i, j );
      }

  res |= lowio.save_array( lowio.stdtypes.t_gridnode, loc+string("Grid"),
                           raw, lats*lons, "TODO: fill", "TODO: fill" );

  delete[] raw;
  return res;
}

//---------------------------------------------------------------------

int Highio::load ( Globals& siku, const string& file_name )
{
  cout<<"TRY TO LOAD\n";
  cout<<"temporally diabled\n";

  vec3d* ppos = new vec3d;
  memcpy( ppos, &verts[i].pos, sizeof( vec3d ) );
  push_back( *pos );
 
//
//  // file init and read dimensions
//  lowio.init( siku.loadfile, lowio.ACCESS_F_READONLY );
//  Dims dims;
//  load_dims( dims );
//
//  cout<<" read elements"<<endl;
//  // read elements
//  siku.es.clear();
//  //siku.es.resize( dims.elem_s, Element() );
//  Element* temp = new Element[dims.elem_s];
//  //lowio.read( "Elements/Elements", siku.es.data() );
//  lowio.read( "Elements/Elements", temp );
//  cout<<"BDFB\n";
//  for(size_t i=0; i< dims.elem_s;++i)
//    {
//      siku.es.push_back(temp[i]);
//    }
//
//
//  cout<<siku.es.size()<<endl<<" read vertices"<<endl;
//  // reading vertices
//  verts.resize( dims.vert_s );
//  lowio.read( "Elements/Vertices", verts.data() );
//  for( size_t i = 0; i < dims.vert_s; ++i )
//    siku.es[ verts[i].elem_id ].P.push_back( verts[i].pos );
//
//
//
//
//  lowio.release();
//
//  cout<<"LOADED\n";
  return 0;
}

//---------------------------------------------------------------------


void Highio::load_dims( Highio::Dims& dims )
{
  dims.contact_s = lowio.get_dim("Contacts/Contacts");
  dims.control_s = lowio.get_dim("Control functions");
  dims.monit_s = lowio.get_dim("Monitor functions");
  dims.elem_s = lowio.get_dim("Elements/Elements");
  dims.vert_s = lowio.get_dim("Elements/Vertices");
  dims.grid_s = lowio.get_dim("Wind/Grid");
}
