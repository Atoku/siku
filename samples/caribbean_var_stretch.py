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

    # ---------------------------------------------------------------------
    #  Wind initializations (NMC grid example)
    # ---------------------------------------------------------------------

    #start time index
    st_t_ind = 2
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

######    siku.settings.wind_source_type = siku.WIND_SOURCES['NONE']
    siku.settings.wind_source_type = siku.WIND_SOURCES['NMC']
    #siku.settings.wind_source_type = siku.WIND_SOURCES['TEST']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]
    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
    w.make_test_field( -0.0, 0.0 )
    siku.wind = w

    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( hours = 1 )

    #siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.finish   = siku.time.start + hour * 120 #120

    siku.time.dt       = ( siku.time.finish - siku.time.start ) / 1200 #60
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    siku.time.last = siku.time.start
    siku.time.last_update = siku.time.last
       
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []
    n_filled = 0
## custom testing polygons for caribbeans # lon, lat convention

    x = 5
    y = 2

    NN = [ 1, 2, 4, 8 ]

    nx = [ x*i for i in NN ]
    ny = [ y*i for i in NN ]

    left_inds = []
    right_inds = []
    left_gi = {}
    right_gi = {}
    
    ###test sections
    for i in range( len( NN ) ):
        cds, links = NG.generate_plus( \
            275.0, 9.0+i*5.0, 285.0, 13.0+i*5.0, nx[i], ny[i], 0., 0. )

        coords = coords + cds
##        siku.settings.links = siku.settings.links + links

##        left_inds = left_inds + \
##                     [ n_filled+j*nx[i] for j in range(ny[i]) ]
##        right_inds = right_inds + \
##                     [ n_filled+j*nx[i]+nx[i]-1 for j in range(0, ny[i]) ]
##
        n_filled += nx[i]*ny[i]

    ###handles
    for i in range( len( NN ) ):
        cds, links = NG.generate_plus( \
            275.0-10.0/nx[i], 9.0+i*5.0, 275.0, 13.0+i*5.0, 1, ny[i], \
            0., 0. )
        coords = coords + cds
        t = [ n_filled+j for j in range(ny[i]) ]
        left_inds = left_inds + t
        for j in t:
            left_gi[j] = i
        n_filled += ny[i]

        cds, links = NG.generate_plus( \
            285.0, 9.0+i*5.0, 285.0+10.0/nx[i], 13.0+i*5.0, 1, ny[i], \
            0., 0. )
        coords = coords + cds
        t = [ n_filled+j for j in range(ny[i]) ]
        right_inds = right_inds + t
        for j in t:
            right_gi[j] = i 
        n_filled += ny[i]

    siku.local.NN = NN
    siku.local.nx = nx
    siku.local.ny = ny
    siku.local.right_inds = right_inds
    siku.local.left_inds = left_inds
    siku.local.right_gi = right_gi
    siku.local.left_gi = left_gi

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

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'caribbean_plot_wnd.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

##    siku.settings.force_model = \
##                    siku.CONTACT_FORCE_MODEL['Hopkins_Frankenstein']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  -10000.0, 1.00, 0.2, 0.1, \
##                                  0.01 ] #wind interaction adjuster

    siku.settings.phys_consts = { 'rigidity' : 1.0,
                                  'viscosity' : 1.0,
                                  'rotatability' : 1.0,#0.75
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' : 200000.0,#-5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 0.5,#0.05,
                                  'tensility' : 0.30,#0.615,

                                  'anchority' : 0.0000001,
                                  'windage':    0.001,
                                  'fastency' : 0.250, #0.5

                                  'sigma' : 500000.0,        # -//- rigidity
                                  'etha' : 0.051          # -//- viscosity
                                  }

    # ------------------------- speed settings ----------------------------
 
    for i in left_inds:
        siku.elements[i].flag_state = element.Element.f_static

    
##    siku.settings.manual_inds = right_inds
##    amo = len(right_inds)
##    F = 800.0
##    siku.settings.manual_forces = [ (F/amo, -F/14/amo, 0.0) \
##                                    #-(i/nx)*1.0, -0.2*(i/ny/nx))
##                                    for i in right_inds ]

    for i in right_inds:
        siku.elements[i].velo = (2, -0.1, 0.0)
        siku.elements[i].flag_state = element.Element.f_steady

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = pretimestep
    siku.callback.aftertimestep = aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations
    siku.callback.updatewind = updatewind

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
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx11.5c"]) #24_20

    siku.local.fces = []
    for i in range( len( siku.local.NN ) ):
        siku.local.fces.append( open( './forces/group' + str(i) + '.txt', 'w' ) )
        

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "convert -density 100 -delay 10 carib*.eps caribbeans.gif", \
                     shell=True )

    [ i.close() for i in siku.local.fces ]

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # primitive time stepping
##    if t > ( siku.time.last + siku.time.dt ):
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # step by NMC own time step
    if t >= siku.uw.times[siku.time.update_index + 1]: #siku.time.last: #
        status += siku.MASK['WINDS']
        siku.time.last = t #siku.time.finish#

    #accumulated force vectr and stress
    siku.local.FF = [ mathutils.Vector( (0.0,0.0,0.0) )\
                      for i in siku.local.NN ]
    siku.local.ff = [ 0 for i in siku.local.NN ]

    return status

# --------------------------------------------------------------------------

def updatewind( siku, t ):
##    siku.time.update_index += 1
##    siku.time.last_update = t
##    siku.wind = \
##              wnd.NMCSurfaceVField( siku.uw, siku.vw, siku.time.update_index )
    pass

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    siku.local.poly_f.close()
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'carib%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    for i in range( len( siku.local.FF ) ):
        f = siku.local.FF[ i ]
        s = siku.local.ff[ i ]
        t = f.length
        siku.local.fces[i].write( \
            str(n) + '\t' + str(s) + '\t' + str(t) + '\t' + \
            str(f[0]) + '\t' + str(f[1]) + '\t' + str(f[2]) + '\n' )

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

    #forces monitor
    if index in siku.local.right_gi:
        siku.local.FF[ siku.local.right_gi[ index ] ] += \
                       mathutils.Vector( F )
        siku.local.ff[ siku.local.right_gi[ index ] ] += \
                       mathutils.Vector( F ).length

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
