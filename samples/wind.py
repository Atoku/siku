'''
A test for wind grids usage, picturing and interpolation.
Generates a set of pictures with wind grids and additional interpolated vectors
in some region.
'''

import subprocess
import os
import math
import sys
import datetime
import mathutils
import numpy

import siku
from   siku import polygon
from   siku import element
from   siku import material
from   siku import geocoords
from   siku import regrid
from   siku import gmt_Plotter
GMT_Plotter = gmt_Plotter.GMT_Plotter
from   siku import poly_voronoi
PolyVor = poly_voronoi.PolyVor
from   siku import h5load
hload = h5load.Loader

from   siku import wnd

# ---------------------------------------------------------------------

import standard_callbacks as scb
scb.siku = siku
 
def main():

    # ---------------------------------------------------------------------
    # Define material
    # ---------------------------------------------------------------------

    ice = material.Material()       # default ice values, 10 thicknesses
    ice.name = 'ice'                # prefer to use our own name instead
                                    # of default
    
    siku.materials.append( ice )    # list of all materials

    # table of material names for convenience
    matnames = {
        'ice': 0,
    }

    # ---------------------------------------------------------------------
    #  Wind initializations (NMC grid example)
    # ---------------------------------------------------------------------

    st_t_ind = 1 # first time entry in .nc files
    N_steps = 10
    siku.time.update_index = st_t_ind - 1
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )  
    
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

    siku.settings.wind_source_type = siku.GRID_SOURCES['NMC']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]
       
    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( minutes = 60 )

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[st_t_ind]

    siku.time.last_update = siku.uw.times[st_t_ind]

    # .nc files have 6-hours timestep. In this way we should have windfield
    # updated at each second step
    siku.time.finish = siku.uw.times[st_t_ind] + 3 * hour * N_steps
    
    siku.time.dt  = (siku.time.finish - siku.time.start) / N_steps
    siku.time.dts = datetime.timedelta ( seconds = 600 )


    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------

    #No elements! Just some winds and maps

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'wind_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 1
    siku.drift_monitor = drift_monitor
    siku.settings.picname_base = 'W_'

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = scb.grid_pretimestep
    siku.callback.aftertimestep = scb.basic_aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations
    siku.callback.updatewind = scb.basic_updatewind

    
    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx17c"])
    pass

# --------------------------------------------------------------------------

def conclusions( siku, t ):   
    print('creating .gif')
    subprocess.call( "nice convert -density 600 -delay 100 W_*.eps W.gif", \
                     shell=True )
    pass

# --------------------------------------------------------------------------

def drift_monitor( t,n, Q, Ps, st, index, ID, W, F, N, m, I, i, A, a_f, w_f ):
    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
