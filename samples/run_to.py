'''Siku scenario

   Sea ice element simple free drift example

   Creates a (large) polygon in polar region and sets some basic winds
   in still water. No interaction with any boundaries, just a free
   float of the polygon in the area and output in KML format of its
   locations.

   Be sure that siku module is in your PYTHONPATH.

   Use python3 for checking. It is not compatible with python2.x

   (c)2014 UAF, written by Anton Kulchitsky
   GPLv3 or later license (same as siku)

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

    siku.settings.wind_source_type = siku.WIND_SOURCES['NMC']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]
   
    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------


    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    #siku.time.last = siku.time.start
    hour = datetime.timedelta ( minutes = 60 )

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.last = siku.uw.times[st_t_ind]
    siku.time.last_update = siku.time.last
    siku.time.finish = siku.uw.times[st_t_ind] + 6* hour
    #siku.time.dt = datetime.timedelta ( milliseconds = 1 )
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 66
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    # ---------------------- voronoi initialization ------------------------
    print('\nLoading polygons')
    ## North cap
    PV = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )
    ## Channel (handmade)
    PC = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )
    
    PV.filter_( 0, 360, 60, 90 )
    PC.filter_( 179, 187, 54, 60 )
    
    print('Deleting land polygons')
    PV.clear_the_land()

    coords = PV.coords
    coords = coords + PC.coords

    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        # Element declaration
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
               0.0, 0.0, 0.0, 0.0, 0.0 ]
        E.set_gh( gh, ice )
        
        # all elements in the list
        siku.elements.append( E )

    ## Core will mark polygons, those contain at leas one point from next
    ## file as 'static'
    siku.settings.border_mark = 1
    siku.settings.borders = 'contours.ll'

    print('Marking borders with GMT')
    bor = PV.get_border_by_gmt()
    for b in bor:
        siku.elements[ b ].flag_state = element.Element.f_static
    print('Done\n\n')
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'plot_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 5
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']

    # name of file to load from
    #siku.settings.loadfile = 'siku-2014-01-01-12:00:00.h5'
    siku.settings.loadfile = 'save_test.h5'


    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = pretimestep
    siku.callback.aftertimestep = aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations
    
    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_24cx20c"])

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "nice convert -density 300 -delay 10 drift*.eps drift.gif", \
                     shell=True )

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # step by NMC own time step
    if t >= siku.time.last: #siku.uw.times[siku.time.update_index + 1]:
        status += siku.MASK['WINDS']
        siku.time.last = siku.time.finish#t
        
    return status

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    siku.local.poly_f.close()
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'drift%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )

        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    #siku.local.poly_f.close()
    return 0

# --------------------------------------------------------------------------

def drift_monitor( t, Q, Ps, i, st ):
    if st & element.Element.f_errored:
        return
    
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        poly = siku.local.poly_f
        if st & element.Element.f_special:
            poly.write( '> -Gpink -W0.1p,purple \n' ) 
        elif st & element.Element.f_static:
            poly.write( '> -Gbrown -W0.1p,lightBlue \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -GlightGreen -W0.1p,lightBlue \n' )
        else:
            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return

# --------------------------------------------------------------------------

def winds_diag( t, winds ):

    mesh = siku.diagnostics.meshes[0]
    ez = mathutils.Vector( (0,0,1) )

    siku.diagnostics.wind_counter += 1

    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
