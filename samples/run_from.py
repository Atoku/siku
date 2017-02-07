'''
Basic test for saving-loading procedure.
Consists of two scripts those represent one continuous modelling session.

This particular sript loads some previously saved file, continues the
modelling and generates the second half of the pictures.
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
    siku.time.start       = siku.uw.times[st_t_ind] + 12 * hour
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

    # ---------------------- loading from file ----------------------------

    print('file start atempt\n')

    try:
        hl = hload('siku-2014-01-01-23:30:00.h5')
    except:
        print("Could not open the file!!")
        exit()

    #### partially loading
    hl.load_fnames()
    hl.load_mats()
    hl.load_els()  # most time spent here (inevitable)
    print('\n')

    siku.elements = hl.extract_els()
    siku.materials = hl.extract_mats()

    hl = None
    print('DONE!\n')

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'RTF_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 60
    siku.diagnostics.step_count = 0
    siku.settings.picname_base = 'RTF_1'

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = scb.single_w_pretimestep
    siku.callback.aftertimestep = scb.basic_aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations
    
    return 0

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "convert -density 300 -delay 10 RTF_*.eps RTF.gif", \
                     shell=True )
    pass

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_24cx20c"])

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
