
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

def is_inside( lo, la, mo, Mo, ma, Ma ):
    return lo > mo and lo < Mo and la > ma and la < Ma
 
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
    siku.time.finish = siku.uw.times[st_t_ind] + hour
    #siku.time.dt = datetime.timedelta ( milliseconds = 1 )
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 1
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    # ---------------------- voronoi initialization ------------------------
    print('\nLoading polygons')
    ## North cap
##    PV = PolyVor( 'high.voronoi.xyz', 'high.voronoi.xyzf' )
    PV = PolyVor( 'h.voronoi.xyz', 'h.voronoi.xyzf' )
##    PV = PolyVor( 'AnC5.voronoi.xyz', 'AnC5.voronoi.xyzf' )
    
    PV.filter_( 0, 360, 60, 90 )
    
    print('Deleting land polygons')
    PV.clear_the_land()

    coords = PV.coords
##    coords = coords + PC.coords

    siku.tempc = coords # for debug

    ### Initializing elements with polygon vertices
    print('Initializing elements')
    for c in coords:
        siku.P.update( c )
     
        # Element declaration
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
               0.0, 0.0, 0.0, 0.0, 0.0 ]
        E.set_gh( gh, ice )

        # If area is too small - element is a garbage
        if E.A > 1e-15:
            siku.elements.append( E )

    ## Core will mark polygons, those contain at least one point from next
    ## file as 'static'
##    siku.settings.border_mark = 0 #1
##    siku.settings.borders = 'contours.ll'

    print('Marking borders with GMT')
    bor = PV.get_border_by_gmt( siku.elements)
    for b in bor:
        siku.elements[ b ].flag_state = element.Element.f_static
    print('Done\n\n')

    # ----------------------- average size analisis -----------------------

    COUNT = 0
    SIZE = 0.0
    for e in siku.elements:
        lo, la = geocoords.quat_to_ll( mathutils.Quaternion(e.q) )
        if is_inside( lo, la, 210, 215, 70, 75 ):
            COUNT += 1
            SIZE += 2. * math.sqrt( e.A / math.pi )

    print('\n@ Number of elements in region:\n  ' + \
          str( COUNT ) + '\n@ Average diameter:\n  ' + \
          str( SIZE / COUNT * 6400 ) + 'km\n\n' )
        

    # ---------------------- loading from file ----------------------------

##    print('loading from file\n')
##    
##    hl = hload('save_test.h5')
####    #hl = hload('siku-2014-01-01-12:50:46.h5')
####
####    #hl.load()
##    hl.load_fnames()
##    hl.load_mats()
##    hl.load_els()
##    print('\n')
##
##    siku.elements = hl.extract_els()
##    siku.materials = hl.extract_mats()
##          
##    hl = None

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'beaufort94_plot.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 1
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

    # name of file to load from
    #siku.settings.loadfile = 'siku-2014-01-01-12:00:00.h5'
##    siku.settings.loadfile = 'save_test.h5'

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  1, 1, 1, 1, 1 ]
        
    siku.settings.phys_consts = { 'rigidity' : 0.0,
                                  'viscosity' : 0.0,
                                  'rotatability' : 0.0,#0.75,
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' : 00000000.0,#-5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 0.0,#0.05,
                                  'tensility' : 0.0,#0.615,

                                  'anchority' : 0.,
                                  'windage':    0.0,
                                  'fastency' : 0.0, #0.5

                                  'sigma' : 0.0,        # -//- rigidity
                                  'etha' : 0.0          # -//- viscosity
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
    siku.callback.global_monitor = global_monitor

    ##
    siku.callback.presave = presave

    siku.err_test = {}
    
    return 0

def presave( t, n, ns ):
    '''no saving at all'''
    return

# --------------------------------------------------------------------------

def global_monitor( t, n, ns, Sigma ):
    siku.sigma = Sigma
    
    siku.local.Smin = -0.5*(Sigma[1] + Sigma[3])
    siku.local.Smax = -0.5*(Sigma[0] + Sigma[2])
    
    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    
    siku.local.sigmaMax = 0.
    siku.local.sigmaMin = 0.

    siku.local.sMax = 1
    siku.local.sMin = 0
    
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

    print('minimal and maximal sigma:\n' +
          str(siku.local.sigmaMin)+ ', '+str(siku.local.sigmaMax)+'\n' )
    
    print('creating .gif')
    subprocess.call( "nice convert -density 500 -delay 10 shot.eps shot.gif", \
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
        pic_name = 'shot.eps'
        print('drawing ' + str( pic_name ) )

        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    #siku.local.poly_f.close()
    return 0

# --------------------------------------------------------------------------     

def drift_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss,\
                   m, I, i, A, a_f, w_f ):
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    s = -0.5*(ss[0] + ss[1])
    if s < siku.local.sigmaMin: siku.local.sigmaMin = s
    if s > siku.local.sigmaMax: siku.local.sigmaMax = s

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        poly = siku.local.poly_f
##        if st & element.Element.f_errored: ##
##            poly.write( '> -Gred -W0.1p,red \n' ) ##                  
##        elif st & element.Element.f_special: ## elif -> if
        if st & element.Element.f_special: ## elif -> if
            poly.write( '> -Gpink -W0.1p,lightBlue \n' ) 
        elif st & element.Element.f_static:
            poly.write( '> -Gbrown -W0.1p,lightBlue \n' )
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -GlightGreen -W0.1p,lightBlue \n' )
        else:
            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
##          return 

####            d = (siku.local.Smax - siku.local.Smin)
##            d = (siku.local.sMax - siku.local.sMin)
##            if not (abs(d) > 1e-12):
##                t = 0.
##            else:
####                t = (s - siku.local.Smin) / d
##                t = (s - siku.local.sMin) / d
##                
##            poly.write( '> -G'+ siku.utils.gmt_color_int( (128, 255, 128), \
##                    (128, 128, 255), t ) +' -W0.1p,lightBlue \n' )     
            
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
