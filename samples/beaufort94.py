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
    
    siku.uw = wnd.NMCVar( 'u1994.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v1994.nc', 'vwnd' )

    start =  datetime.datetime  ( 1994, 2, 16, 00, 00, 00 )
    for i in range(len( siku.uw.times )):
        if siku.uw.times[i] >= start:
            break
    st_t_ind = i
    siku.time.update_index = i - 1
    print( 'start time: ' + str( start ) + ' at position: ' + str( i ) + \
           ' of ' + str( len( siku.uw.times ) ) + '\n\n' )
    
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

    siku.settings.wind_source_type = siku.WIND_SOURCES['NMC']
    siku.settings.wind_source_names = [ 'u1994.nc', 'v1994.nc' ]
##    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
##    w.make_test_field( 0.,0. )
##    siku.wind = w
   
    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------

    #siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    #siku.time.finish   = datetime.datetime  ( 2012, 3, 13 )
    #siku.time.finish   = datetime.datetime  ( 2012, 3, 12, 00, 00, 10 )
    #siku.time.dt       = datetime.timedelta ( seconds = 1 )
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    #siku.time.last = siku.time.start
    hour = datetime.timedelta ( minutes = 60 )

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.last = siku.uw.times[st_t_ind]
    siku.time.last_update = siku.time.last
    siku.time.finish = siku.uw.times[st_t_ind] + hour * 60
    #siku.time.dt = datetime.timedelta ( milliseconds = 1 )
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 2400
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    # ---------------------- voronoi initialization ------------------------
##    print('\nLoading polygons')
##    ## North cap
##    PV = PolyVor( 'alaska.voronoi.xyz', 'alaska.voronoi.xyzf' )
##    ## Channel (handmade)
####    PC = PolyVor( 'alaska.voronoi.xyz', 'alaska.voronoi.xyzf' )
##    
##    PV.filter_( 0, 360, 60, 90 )
####    PC.filter_( 179, 187, 54, 60 )
##
####TESTING!
######    PV.filter_( 190, 230, 62, 82 )
####    PC.filter_( 190, 230, 62, 82 )
####/TESTING
##
##    
##    print('Deleting land polygons')
##    PV.clear_the_land()
##
##    coords = PV.coords
####    coords = coords + PC.coords
##
##    siku.tempc = coords # for debug
##
##    ### Initializing elements with polygon vertices
##    for c in coords:
##        siku.P.update( c )
##     
##        # Element declaration
##        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
##        E.monitor = "drift_monitor"
##        gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
##               0.0, 0.0, 0.0, 0.0, 0.0 ]
##        E.set_gh( gh, ice )
##        
##        # all elements in the list
##        siku.elements.append( E )
##
##    ## Core will mark polygons, those contain at leas one point from next
##    ## file as 'static'
##    siku.settings.border_mark = 1
##    siku.settings.borders = 'contours.ll'
##
##    print('Marking borders with GMT')
##    bor = PV.get_border_by_gmt()
##    for b in bor:
##        siku.elements[ b ].flag_state = element.Element.f_static
##    print('Done\n\n')

    # ---------------------- loading from file ----------------------------

    print('loading from file\n')
    
    hl = hload('save_test.h5')
##    #hl = hload('siku-2014-01-01-12:50:46.h5')
##
##    #hl.load()
    hl.load_fnames()
    hl.load_mats()
    hl.load_els()
    print('\n')

    siku.elements = hl.extract_els()
    siku.materials = hl.extract_mats()
          
    hl = None

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'beaufort94_plot.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

    # name of file to load from
    #siku.settings.loadfile = 'siku-2014-01-01-12:00:00.h5'
    siku.settings.loadfile = 'save_test.h5'

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  1, 1, 1, 1, 1 ]
        
    siku.settings.phys_consts = { 'rigidity' : 1.0,
                                  'viscosity' : 1.0,
                                  'rotatability' : 0.750,#0.75
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' : 5000.0,#-5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 0.5,#0.05,
                                  'tensility' : 0.30,#0.615,

                                  'anchority' : 0.0000001,
                                  'windage':    0.00000001,
                                  'fastency' : 0.30, #0.5

                                  'sigma' : 5000.0,        # -//- rigidity
                                  'etha' : 0.0051          # -//- viscosity
                                  }
    

##    siku.settings.contact_freq_met = siku.CONTACT_DET_FREQ_MET['speed']
##    siku.settings.contact_value = 1000

    # ---------------------------------------------------------------------
    #  Diagnostics function for the winds
    # ------------------------------abs2( e.V )---------------------------------------
    
##    # We create a grid and append it to monitor grids
##    siku.diagnostics.wind_counter = 0
##    rg = regrid.Regrid()
##    mesh_01 = rg.globe_coverage( 5.0 )
##    siku.diagnostics.meshes.append( mesh_01 )
##    siku.diagnostics.wind.append( 
##        ( winds_diag, 0, siku.time.start, 2*siku.time.dt ) )

    # ---------------------------------------------------------------------
    #  Settings
    # ---------------------------------------------------------------------

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

    siku.err_test = {}
    
    return 0

def presave( t, n, ns ):
    '''no saving at all'''
    return

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx13c"])

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    
    with open("err_time.txt", 'w') as erf:
        for i in siku.err_test:
            erf.write( str(i) + ' : ' )#+ ':\n' )
            erf.write( str( len( siku.err_test[i] ) ) )
##            for t in siku.err_test[i]:
##                erf.write( str( t ) + '   ' )
            erf.write( '\n' )

    
    print('creating .gif')
    subprocess.call( "nice convert -density 300 -delay 10 beaufort*.eps beaufort.gif", \
                     shell=True )

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # some specific checks should be placed.

    # primitive time stepping
##    if t > ( siku.time.last + siku.time.dt ):
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # step by NMC own time step
    if t >= siku.uw.times[siku.time.update_index + 1]: # siku.time.last: #
        status += siku.MASK['WINDS']
        siku.time.last = t# siku.time.finish# 

    # and change the winds here
    # ~!wind is changed with another call

    # and save the current time in a structure
    # ~!current time is saved in siku.time.last
    return status

# --------------------------------------------------------------------------

def updatewind( siku, t ):
    siku.time.update_index += 1
    siku.time.last_update = t
    siku.wind = \
              wnd.NMCSurfaceVField( siku.uw, siku.vw, siku.time.update_index )
    print( str( t ) + '\n' )
    pass

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    siku.local.poly_f.close()
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'beaufort%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )

        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    #siku.local.poly_f.close()
    return 0

# --------------------------------------------------------------------------

def drift_monitor( t,n, Q, Ps, st, index, ID, W, F, N, m, I, i, A, a_f, w_f ):
##    #static polygons (generally shores) may be simply passed
##    if st & element.Element.f_static:
##        return
    
##    if st & element.Element.f_errored:
##        if siku.err_test.get( i, None ):
##            siku.err_test[i].append(t)
##        else:
##            siku.err_test[i] = [ t ]
##        return

##    print(st)
##    input()

##    #errored export x-y:
##    if st & element.Element.f_errored:
##        with open("errored"+str(i)+".txt", 'w') as erf:
##            for p in Ps:
##                erf.write( str( p[0] ) +'\t'+ str( p[1] )+'\n' )
    
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

## for debug
##            #errored export lon-lat:
##            with open("err/errored"+str(i)+".txt", 'w') as erf:
##                for v in vert:
##                    erf.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
##                        str( v[1] )+'\n' )
##            #errored original export lon-lat:
##            with open("err/original"+str(i)+".txt", 'w') as erf:
##                #for v in siku.elements[i].verts_xyz_loc:
##                for v in siku.tempc[i]:
##                    erf.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
##                        str( v[1] )+'\n' )
## /for debug
                    
        if st & element.Element.f_errored: ##
            poly.write( '> -Gred -W0.1p,red \n' ) ##
##        if
        elif st & element.Element.f_special: ## elif -> if
            poly.write( '> -Gpurple -W0.1p,pink \n' ) 
        elif st & element.Element.f_static:
##            return
            poly.write( '> -Gbrown -W0.1p,lightBlue \n' )#<<--- this
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -GlightGreen -W0.1p,lightBlue \n' )
        else:
            poly.write( '> -GlightCyan -W0.1p,lightCyan \n' )
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return

# --------------------------------------------------------------------------

def winds_diag( t, winds ):

    mesh = siku.diagnostics.meshes[0]
    ez = mathutils.Vector( (0,0,1) )

###### Commented to stop that file breeding while other modules are being tested
##    fp = open( 'winds-%02d.txt' % (siku.diagnostics.wind_counter), 'w' )
##
##    for i, w in enumerate( winds ):
##        x = mathutils.Vector( mesh[i] )
##        u = mathutils.Vector( w )
##        uval = u.length
##        lon, lat = geocoords.lonlat_deg( x )
##        a = ez - x
##        
##        mdl = a.length * uval
##        if ( mdl != 0 ):
##            azimuth = 180 * math.acos( (a*u) / mdl ) / math.pi
##            fp.write( "%f %f %f %f %f\n" % \
##                      ( lon, lat, 0.25*uval, azimuth, 0.7*uval ) )
##            
##
##    fp.close()
    siku.diagnostics.wind_counter += 1

    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
