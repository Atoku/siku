'''
Simple test to demonstrate the movement of polygons on the sphere.
The core uses complicated calculations with quaternions for the positioning of
elements thus require some testings (especially check the scaling of
velocities).
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
from   siku import color_works as cworks

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
    # date/time settings
    # ---------------------------------------------------------------------

    hour = datetime.timedelta ( minutes = 60 )

    siku.time.start  = datetime.datetime  ( 2014, 1, 1, 00, 00, 00 )
    siku.time.finish = siku.time.start + 24 * hour
    siku.time.dt     = (siku.time.finish - siku.time.start) / 3600
    siku.time.dts    = datetime.timedelta ( seconds = 600 )
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    coords.append( [
        (-35, -5),
        (-25, -5),
        (-25, 5),
        (-35, 5)
        ] )
    coords.append( [
        (-35, 55),
        (-25, 55),
        (-25, 65),
        (-35, 65)
        ] )
    coords.append( [
        ( 25, 55),
        ( 35, 55),
        ( 35, 65),
        ( 25, 65)
        ] )


    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
           0.0, 0.0, 0.0, 0.0, 0.0 ] #ice thickness
    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        # Element declaration
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        E.set_gh( gh, ice )
        
        siku.elements.append( E )

    # ------------------------- speed settings ----------------------------

    day = 24. * 3600. # 1 day in minutes
    R = 6400 * 1000 # Earth radius in meters
    part = 1./6. # part of full turnover. 1/6 = 60 degrees (around the globe
                 # or the element itself in case of spin (third component))
    L = 2*3.14*R # equator length inmeters

    # 'part' of equator length eastward + 1 turnover  in 1 day 
    siku.elements[0].velo = ( part * L / day, 0, 2*3.14 / day )

    # 'part' of equator down the meridian in 1 day
    siku.elements[1].velo = ( 0, -part * L / day , 0 )

    # 'part' of equator down + 'part' west as the two components of velocity
    # vector, thus the absolute value of velocity is sqrt(2) times greater
    siku.elements[2].velo = ( -part * L / day, -part * L / day, 0 )

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'GP_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 60  # 1 picture for each 60 steps
    siku.settings.picname_base = 'GP_'

    # ---------------------------------------------------------------------
    #  Physical constants
    # ---------------------------------------------------------------------

    ## no intaraction with wind or water
    siku.settings.phys_consts['windage'] = 0.
    siku.settings.phys_consts['anchority'] = 0.

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.conclusions = scb.basic_conclusions
    siku.callback.initializations = initializations

    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx17c"])
    pass 

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
