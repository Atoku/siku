'''
The fullest example. Demonstrates basic way to run the modelling process.
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
from   siku import ice_nc

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
    ice.E = 4e5 #4e4 for 'hopkins'
    
    siku.materials.append( ice )    # list of all materials

    # table of material names for convenience
    matnames = {
        'ice': 0,
    }
    
    # ---------------------------------------------------------------------
    #  Wind and current initializations (NMC grid example)
    # ---------------------------------------------------------------------
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )

    ### approximate start time
    start =  datetime.datetime  ( 2014, 2, 2, 10, 10, 30 )
    ### find closest time point in .nc file
    for i in range(len( siku.uw.times )):
        if siku.uw.times[i] >= start:
            break
    st_t_ind = i
    siku.time.update_index = i - 1
    print( 'start time: ' + str( start ) + ' at position: ' + str( i ) + \
           ' of ' + str( len( siku.uw.times ) ) + '\n\n' )
    
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

    siku.settings.wind_source_type = siku.GRID_SOURCES['NMC']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]

#### Uncomment this to fill wind grid with test vectors (east-north components
#### in make_test_field)
##    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
##    w.make_test_field( 0.,0. )
##    siku.wind = w
   
    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------

    hour = datetime.timedelta ( minutes = 60 )

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.last_update = siku.time.start
    siku.time.finish = siku.time.start + 24 * hour
    siku.time.dt = (siku.time.finish - siku.time.start) / 3601
    siku.time.dts      = 30 * hour

   
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

    # --------------------------- applying mask ---------------------------

    ## using grid data as some ice-state mask. In this case - setting
    ## some landfast ice as 'steady' 'special' elements.
    print('Applying masks')
    Mask = ice_nc.InterGrid('init_Beaufort.nc')
    Mask1 = ice_nc.InterGrid('init_Chukchi.nc')

    q2ll = geocoords.quat_to_ll

    eln = []
    for e in siku.elements:
        
        ll = q2ll( mathutils.Quaternion( e.q ) )
        v = Mask.get( ll[0], ll[1] )
        v1 = Mask1.get( ll[0], ll[1] )

        if v:
            v = abs(int(v))
        if v1:
            v1 = abs(int(v1))

        ## refine the values! int-uint conversions are plausible
        if (v == 255 or v == 128 or v == 1) or\
           (v1 == 255 or v1 == 128 or v1 == 1):
            e.flag_state = element.Element.f_special | \
                           element.Element.f_static
            
        if not ((v == 0 or v == 64) and (v1 == 0 or v1 == 64)):
            eln.append( e )
    siku.elements = eln
    print('Done')

    # ---------------------------------------------------------------------

    print('Marking borders with GMT')
    bor = PV.get_border_by_gmt()
    for b in bor:
        siku.elements[ b ].flag_state = element.Element.f_static
    print('Done\n\n')

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'complete_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 60
    siku.drift_monitor = scb.stress_monitor
    siku.settings.picname_base = 'C_'

    siku.settings.phys_consts['windage'] = 0.001
    siku.settings.phys_consts['anchority'] = 0.01
    siku.settings.phys_consts['tensility'] = 0.5
    siku.settings.phys_consts['etha'] = 0.001

##    ## Uncomment this to test 'hopkins' physical model
##    siku.settings.phys_consts['rigidity'] = 0.001 #0.1 
##    
##    siku.settings.force_model = siku.CONTACT_FORCE_MODEL['hopkins']
##
##    siku.settings.phys_consts['sigma_t'] = 3.5e4
##    siku.settings.phys_consts['sigma_c'] = 1.285e4
##    siku.settings.phys_consts['sigma_s'] = 3.5e4
##    siku.settings.phys_consts['tan_mu'] = 1.
##    
##    siku.settings.phys_consts['hop_visc'] = 0.01
##    siku.settings.phys_consts['dest_threshold'] = 0.5
##    
##    siku.settings.phys_consts['n_integ_segments'] = 10.

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = scb.grid_pretimestep
    siku.callback.aftertimestep = scb.basic_aftertimestep
    siku.callback.conclusions = scb.basic_conclusions
    siku.callback.initializations = initializations
    siku.callback.updatewind = scb.basic_updatewind

    siku.callback.presave = scb.basic_presave

    siku.err_test = {}
    
    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx15c"])
    pass

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
