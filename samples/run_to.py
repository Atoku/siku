'''
Basic test for saving-loading procedure.
Consists of two scripts those represent one continuous modelling session.

This particular sript generates a set of files to run from and the first half
of the pictures.
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

    #start time index
    st_t_ind = 2
    
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
    siku.time.start       = siku.uw.times[st_t_ind]
    siku.time.last        = siku.time.start
    siku.time.last_update = siku.time.last
    siku.time.finish      = siku.time.start + 12 * hour
    siku.time.dt          = (siku.time.finish - siku.time.start) / 1800
    siku.time.dts         = 0.5 * hour
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    # ---------------------- voronoi initialization ------------------------
    print('\nLoading polygons')

    PV = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )

    PV.filter_( 0, 360, 60, 90 )
    
    print('Deleting land polygons')
    PV.clear_the_land()

    coords = PV.coords

    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
           0.0, 0.0, 0.0, 0.0, 0.0 ]
    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        E.set_gh( gh, ice )
        
        siku.elements.append( E )

    print('Marking borders with GMT')
    bor = PV.get_border_by_gmt()
    for b in bor:
        siku.elements[ b ].flag_state = element.Element.f_static
    print('Done\n\n')
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'RTF_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 60
    siku.diagnostics.step_count = 0
    siku.settings.picname_base = 'RTF_0'

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = scb.single_w_pretimestep
    siku.callback.aftertimestep = scb.basic_aftertimestep
    siku.callback.initializations = initializations
    siku.callback.presave = scb.basic_presave
    
    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_24cx20c"])
    pass

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )

