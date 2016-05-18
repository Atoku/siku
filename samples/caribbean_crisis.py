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
                     (295.0, 22.0) ] )
    
    coords.append( [ (291.0, 24.0), #18
                     (292.0, 24.0),
                     (292.0, 29.0),
                     (291.0, 29.0) ] )
    coords.append( [ (293.0, 26.0), #19
                     (294.0, 26.0),
                     (294.0, 27.0),
                     (293.0, 27.0) ] )

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

####    Works with caribbean polygons after initialization
    siku.elements[0].velo = ( 20, 0, 0 )
##    siku.elements[1].flag_state = element.Element.f_static
    siku.elements[2].velo = ( 20, 0, 0 )
##    siku.elements[3].flag_state = element.Element.f_static
    siku.elements[4].velo = ( 20, 0, 0 )
##    siku.elements[5].flag_state = element.Element.f_static

    siku.elements[6].velo = ( 0, 0, -0.0001 )
##    siku.elements[7].flag_state = element.Element.f_static
    siku.elements[8].velo = ( 0, 0, -0.0001 )
##    siku.elements[9].flag_state = element.Element.f_steady
    siku.elements[10].velo = ( 20, 0, 0,)#4, 0 )
##    siku.elements[11].flag_state = element.Element.f_steady

    siku.elements[13].velo = ( 20, 0, 0)#7, 0 )
##    siku.elements[14].flag_state = element.Element.f_steady
    siku.elements[15].velo = ( 20, 0, 0,)#5, 0 )
##    siku.elements[16].flag_state = element.Element.f_steady
    siku.elements[17].velo = ( 20, 0, 0,)#6.5, 0 )
##    siku.elements[18].flag_state = element.Element.f_steady

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'caribbean_plot.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  -10000.0, 1, 0.2, 0.1, 1 ]
    siku.settings.phys_consts = { 'rigidity' : 10.0,#10,
                                  'viscosity' : 1.0,#1.0,#1
                                  'rotatability' : 0.750,#0.75
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' :-50000000.0,#-5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 0.05,#0.05,
                                  'tensility' : 0.30,#0.615,

                                  'anchority' : 0.0005,
                                  'windage': 0.05, #0.05
                                  'fastency' : 0.50, #0.5

                                  'sigma' : 1.0,        # -//- rigidity
                                  'etha' : 1.0          # -//- viscosity
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

# --------------------------------------------------------------------------

def conclusions( siku, t ):
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

def drift_monitor( t, Q, Ps, st, index, ID, W, F, N, m, I, i, A, a_f, w_f ):
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
