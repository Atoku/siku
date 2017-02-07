'''
A bit complicated test for physics. Contains a variety of colliding elements.
Should be useful to prove interaction realism.
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
from   siku import noisy_grid as NG

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
    ice.E = 1e8 #huge stiffness
    
    siku.materials.append( ice )    # list of all materials

    # table of material names for convenience
    matnames = {
        'ice': 0,
    }

    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( hours = 1 )

    siku.time.start    = datetime.datetime  ( 2014, 1, 1, 00, 00, 00 )
    siku.time.finish   = siku.time.start + hour * 24

    siku.time.dt       = (siku.time.finish - siku.time.start) / 3600
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
       
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    ## custom testing polygons for caribbeans # lon, lat convention
    coords.append( [ (267.0, 12.0),      
                    (269.0, 12.0),
                    (269.0, 14.0),
                    (267.0, 14.0) ] )
    coords.append( [ (270.0, 12.0), #2
                     (272.0, 12.0),
                     (272.0, 14.0),
                     (270.0, 14.0) ] )
    
    coords.append( [ (265.0, 20.0), #3
                     (267.0, 18.0),
                     (269.0, 20.0),
                     (267.0, 22.0) ] )
    coords.append( [ (268.0, 18.0), #4
                     (270.0, 16.0),
                     (272.0, 18.0),
                     (270.0, 20.0) ] )
    
    coords.append( [ (265.0, 28.0), #5
                     (266.0, 26.5),
                     (267.0, 28.0),
                     (266.0, 29.5) ] )
    coords.append( [ (267.0, 25.5), #6
                     (268.0, 24.0),
                     (269.0, 25.5),
                     (268.0, 27.0) ] )
    # ---
    
    coords.append( [ (281.0, 10.0), #7
                     (282.0, 10.0),
                     (282.0, 16.0),
                     (281.0, 16.0) ] )
    coords.append( [ (282.5, 10.0), #8
                     (283.5, 10.0),
                     (283.5, 16.0),
                     (282.5, 16.0) ] )
    
    coords.append( [ (278.0, 19.5), #9
                     (282.0, 19.5),
                     (282.0, 20.5),
                     (278.0, 20.5) ] )
    coords.append( [ (281.0, 18.0), #10
                     (285.0, 18.0),
                     (285.0, 19.0),
                     (281.0, 19.0) ] )
    
    coords.append( [ (278.0, 24.0), #11
                     (280.0, 24.0),
                     (280.0, 28.0),
                     (278.0, 28.0) ] )
    coords.append( [ (282.0, 28.0), #12
                     (283.0, 26.0),
                     (285.0, 26.0),
                     (284.0, 28.0) ] )
    coords.append( [ (282.0, 24.0), #13
                     (284.0, 24.0),
                     (285.0, 26.0),
                     (283.0, 26.0) ] )
    # --- 

    coords.append( [ (293.0, 12.0), #14
                     (294.0, 12.0),
                     (294.0, 13.0),
                     (293.0, 13.0) ] )
    coords.append( [ (295.0, 10.0), #15
                     (296.0, 10.0),
                     (296.0, 15.0),
                     (295.0, 15.0) ] )
    
    coords.append( [ (293.0, 21.0), #16
                     (294.0, 21.0),
                     (294.0, 22.0),
                     (293.0, 22.0) ] )
    coords.append( [ (295.0, 17.0), #17
                     (296.0, 17.0),
                     (296.0, 22.0),
                     (295.0, 22.0) ] )##    siku.callback.presave = presave

    
    coords.append( [ (291.0, 24.0), #18
                     (292.0, 24.0),
                     (292.0, 29.0),
                     (291.0, 29.0) ] )
    coords.append( [ (293.0, 26.0), #19
                     (294.0, 26.0),
                     (294.0, 27.0),
                     (293.0, 27.0) ] )

    # ---
    
    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
           0.0, 0.0, 0.0, 0.0, 0.0 ]
    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"    
        E.set_gh( gh, ice )
        
        siku.elements.append( E )

    # ------------------------- speed settings ----------------------------

    #### Start velocities and rotations
    siku.elements[0].velo = ( 20, 0, 0 )
    siku.elements[2].velo = ( 20, 0, 0 )
    siku.elements[4].velo = ( 20, 0, 0 )

    siku.elements[6].velo = ( 0, 0, -0.0001 )
    siku.elements[8].velo = ( 0, 0, -0.0001 )
    siku.elements[10].velo = ( 20, 0, 0,)#4, 0 )

    siku.elements[13].velo = ( 20, 0, 0)#7, 0 )
    siku.elements[15].velo = ( 20, 0, 0,)#5, 0 )
    siku.elements[17].velo = ( 20, 0, 0,)#6.5, 0 )

##    siku.elements[1].flag_state = element.Element.f_static
##    siku.elements[3].flag_state = element.Element.f_static
##    siku.elements[5].flag_state = element.Element.f_static

##    siku.elements[7].flag_state = element.Element.f_static
##    siku.elements[9].flag_state = element.Element.f_steady
##    siku.elements[11].flag_state = element.Element.f_steady

##    siku.elements[14].flag_state = element.Element.f_steady
##    siku.elements[16].flag_state = element.Element.f_steady
##    siku.elements[18].flag_state = element.Element.f_steady

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'CC_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.settings.picname_base = 'CC_'

    # ---------------------------------------------------------------------
    #  Physical constants
    # ---------------------------------------------------------------------

    ## no intaraction with wind or water
    siku.settings.phys_consts['windage'] = 0.
    siku.settings.phys_consts['anchority'] = 1e-5

    ## some viscosity
    siku.settings.phys_consts['etha'] = 1 #0.9051 

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.conclusions = scb.basic_conclusions
    siku.callback.initializations = initializations

    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_20cx15c"])
    pass

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
