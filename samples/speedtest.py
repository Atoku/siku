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
import sys
import datetime
import mathutils

import siku
from   siku import polygon
from   siku import element
from   siku import material
from   siku import geocoords
from   siku import gmt_Plotter
GMT_Plotter = gmt_Plotter.GMT_Plotter
 
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

    siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    siku.time.finish   = datetime.datetime  ( 2012, 3, 12, 1, 00, 1 )
    siku.time.dt       = datetime.timedelta ( seconds = 60 )
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    siku.time.last = siku.time.start
    siku.time.last_update = siku.time.last
    
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------

    coords = []

    #poly # lon, lat convention
    coords.append( [ (179.0, -1.0),
                     (181.0, -1.0),
                     (181.0, 1.0),
                     (179.0, 1.0) ] )
    
    coords.append( [ (179.0, 29.0),
                     (181.0, 29.0),
                     (181.0, 31.0),
                     (179.0, 31.0) ] )
    
    coords.append( [ (194.0, 14.0),
                     (196.0, 14.0),
                     (196.0, 16.0),
                     (194.0, 16.0) ] )


    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0,  # thickness distribution, the sum of
           0.0, 0.0, 0.0, 0.0, 0.0 ] # layers should always be equal to 1
    
    for c in coords:
        siku.P.update( c )
     
        # Element declaration
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        E.set_gh( gh, ice )
        
        siku.elements.append( E )

    #elements` starting speed in EastV-NorthV-AngV(CCW) terms
    siku.elements[0].velo = ( 900.0, 0.0 , 0.0 ) #just flying
    siku.elements[1].velo = ( 0.0, -900.0 , 2.0*3.14/3600 ) #both
    siku.elements[2].velo = ( 0.0, 0.0 , 2.0*3.14/3600 ) #just spinning

    siku.elements[0].flag_state = element.Element.f_steady
    siku.elements[1].flag_state = element.Element.f_steady
    siku.elements[2].flag_state = element.Element.f_steady
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'speedplot.py' )
    
    siku.diagnostics.monitor_period = 2
    siku.drift_monitor = drift_monitor

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
    siku.local.monit = []
    subprocess.call(["gmtset", "PS_MEDIA=Custom_24cx20c"])

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "convert -density 100 -delay 10 drift*.eps drift.gif", \
                     shell=True )

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n
    
    return status

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'drift%02d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )

        with open( 'Polygons.txt', 'w' ) as poly:
            for t in siku.local.monit:
                poly.write( '> \n' )
                for v in t[1]:
                    poly.write( str( geocoords.norm_lon(v[0]) ) + '\t' + \
                                str( v[1] ) + '\n' )

        siku.local.monit = []
        siku.plotter.plot( pic_name )
    return 0

# --------------------------------------------------------------------------

def drift_monitor( t,n, Q, Ps, st, index, ID, W, F, N, m, I, i, A, a_f, w_f ):
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

        siku.local.monit.append( ( st, vert ) )
    return

# --------------------------------------------------------------------------

def winds_diag( t, winds ):
    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
