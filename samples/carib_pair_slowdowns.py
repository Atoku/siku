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

from   siku import wnd
from   siku import noisy_grid as NG
 
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

    siku.settings.wind_source_type = siku.WIND_SOURCES['NONE']


    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( hours = 1 )

    siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    siku.time.finish   = siku.time.start + hour * 30 #120

    siku.time.dt       = ( siku.time.finish - siku.time.start ) / 1200 #60
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    siku.time.last = siku.time.start
    siku.time.last_update = siku.time.last
       
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []
## custom testing polygons for caribbeans # lon, lat convention
   
    nx = 2
    ny = 1
    coords, links \
        = NG.generate_plus( 268.0, -1.0, 272.0, 1.0, nx, ny, 0., 0. )
##    siku.settings.links = links

    # ---
    
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

    # ------------------------- speed settings ----------------------------

######    Works with caribbean polygons after initialization
    siku.elements[1].velo = ( 0.0, 5.0, 0. )
##    siku.elements[1].velo = ( 0, 0, 0.0001 )
    siku.elements[0].flag_state = element.Element.f_static


    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'caribbean_plot1.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  -10000.0, 1, 0.2, 0.1, 1 ]
    siku.settings.phys_consts = { 'rigidity' : 0.0,
                                  'viscosity' : 1.0,
                                  'rotatability' : 1.0,#0.75,
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' :-50000000.0,#-5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 0.05,#0.05,
                                  'tensility' : 10.3,#0.615,

                                  'anchority' : 0.0000,
                                  'windage':    0.0000000,
                                  'fastency' : 10.50, #0.5

                                  'sigma' : 10000000.0,        # -//- rigidity
                                  'etha' : 0.905          # -//- viscosity
                                  }

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = pretimestep
    siku.callback.aftertimestep = aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations

    ##
    siku.callback.presave = presave

    return 0

def presave( t, n, ns ):
##    if n%siku.diagnostics.monitor_period ==0:
##        fname = 'siku-' + t.strftime("%Y-%m-%d-%H:%M:%S") + '.h5'
##        return fname
    return

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_24cx20c"])
    siku.local.FF = open( './forces/N.txt', 'w' )

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    siku.local.FF.close()
    subprocess.call( "gnuplot ./forces/N.sh", shell=True )
    
    print('creating .gif')
    subprocess.call( "convert -density 100 -delay 10 carib*.eps caribbeans.gif", \
                     shell=True )


    
# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    return status

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    siku.local.poly_f.close()
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'carib%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index )

    #siku.local.poly_f.close()
    return 0

# --------------------------------------------------------------------------

def drift_monitor( t,n, Q, Ps, st, index, ID, W, F, N, m, I, i, A, a_f, w_f ):
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    if( ID == 1 ):
        ttt = math.sqrt( F[0]*F[0] + F[1]*F[1] + F[2]*F[2] )
##        ttt = math.sqrt( F[1]*F[1] ) #+ math.sqrt( F[1]*F[1] )
        siku.local.FF.write( str(n)+'\t' + str(W[2])+'\t' + str(ttt)+'\n' )

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        poly = siku.local.poly_f
##        if st & element.Element.f_errored: ##
##            poly.write( '> -Gred -W0.1p,red \n' ) ##
                                
##        elif
        if st & element.Element.f_special: ## elif -> if
            poly.write( '> -Gpink -W0.1p,lightBlue \n' ) 
        elif st & element.Element.f_static:
            poly.write( '> -Gbrown -W0.1p,lightBlue \n' )
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
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
