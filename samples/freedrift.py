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
    
    siku.defaults.wind_source_type = siku.WIND_SOURCES['NONE']

    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( hours = 1 )

    siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    siku.time.finish   = siku.time.start + hour * 360 #120

    siku.time.dt       = ( siku.time.finish - siku.time.start ) / 600 #60
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
    coords.append( [ (269.0, 12.0), #2
                     (271.0, 12.0),
                     (271.0, 14.0),
                     (269.0, 14.0) ] )
    
    coords.append( [ (271.0, 12.0),                    
                     (273.0, 12.0),
                     (273.0, 14.0),
                     (271.0, 14.0) ] )
    coords.append( [ (273.0, 12.0), #4
                     (275.0, 12.0),
                     (275.0, 14.0),
                     (273.0, 14.0) ] )
    
    coords.append( [ (275.0, 12.0),    #5  
                     (277.0, 12.0),
                     (277.0, 14.0),
                     (275.0, 14.0) ] )
    coords.append( [ (277.0, 12.0),    #6  
                     (279.0, 12.0),
                     (279.0, 14.0),
                     (277.0, 14.0) ] )

    coords.append( [ (279.0, 12.0),    #7  
                     (281.0, 12.0),
                     (281.0, 14.0),
                     (279.0, 14.0) ] )
    coords.append( [ (281.0, 12.0),    #8  
                     (283.0, 12.0),
                     (283.0, 14.0),
                     (281.0, 14.0) ] )

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

    siku.elements[0].flag_state = element.Element.f_static

######    Works with caribbean polygons after initialization
##    siku.elements[0].velo = ( 2, 0, 0 )
####    siku.elements[1].flag_state = element.Element.f_static
##    siku.elements[2].velo = ( 2, 0, 0 )
####    siku.elements[3].flag_state = element.Element.f_static
##    siku.elements[4].velo = ( 2, 0, 0 )
####    siku.elements[5].flag_state = element.Element.f_static
##
##    siku.elements[6].velo = ( 0, 0, -0.00001 )
####    siku.elements[7].flag_state = element.Element.f_static
##    siku.elements[8].velo = ( 0, 0, -0.00001 )
####    siku.elements[9].flag_state = element.Element.f_steady
##    siku.elements[10].velo = ( 2, 0, 0 )
####    siku.elements[11].flag_state = element.Element.f_steady
##
##    siku.elements[13].velo = ( 2, 0, 0 )
####    siku.elements[14].flag_state = element.Element.f_steady
##    siku.elements[15].velo = ( 2, 0, 0 )
####    siku.elements[16].flag_state = element.Element.f_steady
##    siku.elements[17].velo = ( 2, 0, 0 )
####    siku.elements[18].flag_state = element.Element.f_steady
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'caribbean_plot.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.defaults.contact_method = siku.CONTACT_METHODS['sweep']

    siku.defaults.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
                                  -300000.0, 1, 0.3, 0.01, 1 ]

    siku.defaults.manual_inds = [7]
#    siku.defaults.manual_forces = [ (100.0, 0.0, 0.1) ]
    siku.defaults.manual_forces = [ (10.0, 0.0, 0.1) ]

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

def drift_monitor( t, Q, Ps, i, st ): 
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
