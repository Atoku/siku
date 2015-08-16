/*!

  \file diagnostics.cc

  \brief Implementation of diagnostics class

*/

//#include "diagnostics.hh"

#ifdef AAA

void Diagnostics::run( Globals& siku, 
                       Sikupy& sikupy )
{
  if ( ! turned_on() ) return;

  run_wind( siku, sikupy );
}

void Diagnostics::run_wind( Globals& siku, 
                            Sikupy& sikupy )
{
  // all that need to be output
  for ( auto& dw: diag_wind )
    {
      cout << diag_wind.ifunc << endl;
      // vector<vec3d> data;

      // // Restore values
      // for ( auto& x: meshes[ dw.imesh ] )
      //   {
      //     vec3d u;
      //     wind.get_at_xy( x, &u );
      //     data.push_back( u );
      //   }

      // // Call callback from python
      // sikupy.fcall_diagnostics_vec3d( siku, dw.ifunc, data );
    }
}

#endif
