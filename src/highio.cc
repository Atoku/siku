/*!

  \file highio.cc

  \brief Implementation of saving and reading variables

*/

#include "highio.hh"
#include "modeltime.hh"

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
  lowio.save_array( lowio.type_element(), "Data/Elements", 
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

  //---------------------------------------------------------------------
  
  lowio.release();              // and stop working with this file

  return status;
}
