/*!

  \file monitoring.cc

  \brief Implementation of monitoring function.

*/

#include "monitoring.hh"
#include "errors.hh"

//---------------------------------------------------------------------

void monitoring( Globals& siku, Sikupy& sikupy )
{
  // Monitoring the elements

  for ( size_t i = 0; i < siku.es.size(); ++i )
    {
    if ( siku.es[i].flag & Element::F_MONITORED )
      {
        // new version with mons` names list
        int status = sikupy.fcall_monitor( siku, i,
                                siku.mons[ siku.es[i].mon_ind ].c_str() );

        switch ( status )
          {
          case Sikupy::FCALL_ERROR_NO_FUNCTION:
            fatal( 2, "No monitor function named  %s  found",
                   siku.mons[ siku.es[i].mon_ind ].c_str() );
            break;
          }
      }

    }
}

//---------------------------------------------------------------------

void diagnosting( Globals& siku, Sikupy& sikupy )
{
  if ( ! siku.diagnostics.turned_on() ) return;


  // WIND

  vector<vec3d> data;           // data to send to python

  for ( auto& dw: siku.diagnostics.windbase )
    {
      if ( dw.scheduler.is_event( siku.time.get_current_as_is() ) )
        {
          // reschedule
          dw.scheduler.increment_event_time();

          // Restore values
          for ( auto& x: siku.diagnostics.meshes[ dw.imesh ].data )
            {
              vec3d u;
              siku.wind.get_at_xy( x, &u );
              data.push_back( u );
            }
          
          // Call callback from python
          sikupy.fcall_diagnostics_vec3d( siku, dw.ifunc, data );

        }
    }

}
