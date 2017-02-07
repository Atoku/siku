'''File: standard_callbacks.py

Module: standard_callbacks - a set of most common callback
functions to put them in scenario script without rewriting each time.

Functions may be either imported or copy-pasted to scenario.

Some functions depend on other ones thus they are grouped by names.

One may modify or extand this file, yet it is better to have some
reserve copy.

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

### replaceble object for import paradigm
class Siku:
    pass
siku = Siku()
#______________________________________

def basic_presave( t, n, ns ):
    '''Simple filename from timestamp.'''
    fname = 'siku-' + t.strftime("%Y-%m-%d-%H:%M:%S") + '.h5'
    return fname

def basic_pretimestep( t, n, ns):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    print("\r Step: " + str(n) + "\t", end = "", flush = True) # simple output

    # for further polygon drawing with GMT
    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # primitive time stepping for grid updating
    if t > ( siku.time.last + siku.time.dt ):
        status += siku.MASK['WINDS']
        #status += siku.MASK['CURRENTS'] # no currents in basic version
        siku.time.last = t

    return status

def grid_pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    print("\r Step: " + str(n) + "\t", end = "", flush=True)

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # step by NMC own time step
    if t >= siku.uw.times[siku.time.update_index + 1]:
        status += siku.MASK['WINDS']

    return status

def single_w_pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    print("\r Step: " + str(n) + "\t", end = "", flush=True)
    
    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # loading winds only once - at the very beginning
    if t >= siku.time.last:
        status += siku.MASK['WINDS']
        siku.time.last = siku.time.finish
        
    return status

def basic_global_monitor( t, n, ns, Sigma ):
    '''Track the stress.'''
    sigma = Sigma
    
    siku.local.Smin = -0.5*(Sigma[1] + Sigma[3])
    siku.local.Smax = -0.5*(Sigma[0] + Sigma[2])
    
    return 0

def basic_aftertimestep( t, n, ns ):
    siku.local.poly_f.close()

    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = siku.settings.picname_base + '%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    return 0

def basic_updatewind( siku, t ):
    print( "updating winds: ", end = "" )
    siku.time.update_index += 1
    siku.time.last_update = t
    siku.wind = \
              wnd.NMCSurfaceVField( siku.uw, siku.vw, siku.time.update_index )
    print( str( t ) )
    pass

def basic_initializations( siku, t ):
    '''Track the stress.'''
    siku.local.sigmaMax = 0
    siku.local.sigmaMin = 0
    pass

def basic_conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "convert -density 300 -delay 10 " \
                     + siku.settings.picname_base + \
                     "*.eps " + siku.settings.picname_base + \
                     ".gif", shell=True )
    pass

def basic_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss, \
                     m, I, i, A, a_f, w_f ):

    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    s = -0.5*(ss[0] + ss[1]) # s for stress, ss for stress matrix
##    sxy = -0.5*(ss[2]+ss[3])

    if s < siku.local.sigmaMin: siku.local.sigmaMin = s
    if s > siku.local.sigmaMax: siku.local.sigmaMax = s

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        col1 = (255, 128, 127) #light red
        col2 = (128, 127, 255) #light blue
        col = cworks.gmt_color_hsv_scale( col2, col1, s, 1e5 )

        poly = local.poly_f
        if st & element.Element.f_special:
            poly.write( '> -Gpink -W0.4p,'+col+' \n' ) 
        elif st & element.Element.f_static:
            poly.write( '> -G'+ col +' -W0.2p,black \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -G'+ col +' -W0.2p,white \n' )
        else:
            poly.write( '> -G'+ col +' -W0.05p,lightBlue \n' ) 
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return

def stress_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss, \
                   m, I, i, A, a_f, w_f ):
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    s = -0.5*(ss[0] + ss[1])

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        col1 = (255, 128, 127) #light red
        col2 = (128, 127, 255) #light blue
        col = cworks.gmt_color_hsv_scale( col2, col1, s, 1e5 )

        poly = siku.local.poly_f

        if st & element.Element.f_special:
            poly.write( '> -Gpink -W0.4p,'+col+' \n' )
        elif st & element.Element.f_static:
            poly.write( '> -G'+ col +' -W0.2p,black \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -G'+ col +' -W0.2p,white \n' )
        else:
            poly.write( '> -G'+ col +' -W0.1p,'+ col +' \n' ) 
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return
