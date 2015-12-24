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

    siku.defaults.wind_source_type = siku.WIND_SOURCES['NMC']
    siku.defaults.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]
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
    siku.time.finish = siku.uw.times[st_t_ind] + 6* hour
    #siku.time.dt = datetime.timedelta ( milliseconds = 1 )
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 143
   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []
#### custom testing polygons for caribbeans # lon, lat convention
##   
##    coords.append( [ (267.0, 12.0),      
##                    (269.0, 12.0),
##                    (269.0, 14.0),
##                    (267.0, 14.0) ] )
##    coords.append( [ (270.0, 12.0), #2
##                     (272.0, 12.0),
##                     (272.0, 14.0),
##                     (270.0, 14.0) ] )
##    
##    coords.append( [ (265.0, 20.0), #3
##                     (267.0, 18.0),
##                     (269.0, 20.0),
##                     (267.0, 22.0) ] )
##    coords.append( [ (268.0, 18.0), #4
##                     (270.0, 16.0),
##                     (272.0, 18.0),
##                     (270.0, 20.0) ] )
##    
##    coords.append( [ (265.0, 28.0), #5
##                     (266.0, 26.5),
##                     (267.0, 28.0),
##                     (266.0, 29.5) ] )
##    coords.append( [ (267.0, 25.5), #6
##                     (268.0, 24.0),
##                     (269.0, 25.5),
##                     (268.0, 27.0) ] )
##    # ---
##    
##    coords.append( [ (281.0, 10.0), #7
##                     (282.0, 10.0),
##                     (282.0, 16.0),
##                     (281.0, 16.0) ] )
##    coords.append( [ (282.5, 10.0), #8
##                     (283.5, 10.0),
##                     (283.5, 16.0),
##                     (282.5, 16.0) ] )
##    
##    coords.append( [ (278.0, 19.5), #9
##                     (282.0, 19.5),
##                     (282.0, 20.5),
##                     (278.0, 20.5) ] )
##    coords.append( [ (281.0, 18.0), #10
##                     (285.0, 18.0),
##                     (285.0, 19.0),
##                     (281.0, 19.0) ] )
##    
##    coords.append( [ (278.0, 24.0), #11
##                     (280.0, 24.0),
##                     (280.0, 28.0),
##                     (278.0, 28.0) ] )
##    coords.append( [ (282.0, 28.0), #12
##                     (283.0, 26.0),
##                     (285.0, 26.0),
##                     (284.0, 28.0) ] )
##    coords.append( [ (282.0, 24.0), #13
##                     (284.0, 24.0),
##                     (285.0, 26.0),
##                     (283.0, 26.0) ] )
##    # --- 
##
##    coords.append( [ (293.0, 12.0), #14
##                     (294.0, 12.0),
##                     (294.0, 13.0),
##                     (293.0, 13.0) ] )
##    coords.append( [ (295.0, 10.0), #15
##                     (296.0, 10.0),
##                     (296.0, 15.0),
##                     (295.0, 15.0) ] )
##    
##    coords.append( [ (293.0, 21.0), #16
##                     (294.0, 21.0),
##                     (294.0, 22.0),
##                     (293.0, 22.0) ] )
##    coords.append( [ (295.0, 17.0), #17
##                     (296.0, 17.0),
##                     (296.0, 22.0),
##                     (295.0, 22.0) ] )
##    
##    coords.append( [ (291.0, 24.0), #18
##                     (292.0, 24.0),
##                     (292.0, 29.0),
##                     (291.0, 29.0) ] )
##    coords.append( [ (293.0, 26.0), #19
##                     (294.0, 26.0),
##                     (294.0, 27.0),
##                     (293.0, 27.0) ] )

    # ---------------------- voronoi initialization ------------------------
##    print('\nLoading polygons')
##    ## North cap
##    PV = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )
##    ## Channel (handmade)
##    PC = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )
##    
##    PV.filter_( 0, 360, 60, 90 )
##    PC.filter_( 179, 187, 54, 60 )
##    
##    print('Deleting land polygons')
##    PV.clear_the_land()
##
##    coords = PV.coords
##    coords = coords + PC.coords

    ### Initializing elements with polygon vertices
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

    ## Core will mark polygons, those contain at leas one point from next
    ## file as 'static'
##    siku.defaults.boarder_mark = 1
##    siku.defaults.boarders = 'contours.ll'
##
##    print('Marking boarders with GMT')
##    bor = PV.get_boarder_by_gmt()
##    for b in bor:
##        siku.elements[ b ].flag_state = element.Element.f_static
##    print('Done\n\n')

    # ---------------------- loading from file ----------------------------

    print('file start atempt\n')
    
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


    # ------------------------- speed sattings ----------------------------

######    Works with caribbean polygons after initialization
##    siku.elements[0].velo = ( 20, 0, 0 )
####    siku.elements[1].flag_state = element.Element.f_static
##    siku.elements[2].velo = ( 20, 0, 0 )
####    siku.elements[3].flag_state = element.Element.f_static
##    siku.elements[4].velo = ( 20, 0, 0 )
####    siku.elements[5].flag_state = element.Element.f_static
##
##    siku.elements[6].velo = ( 0, 0, -0.0001 )
####    siku.elements[7].flag_state = element.Element.f_static
##    siku.elements[8].velo = ( 0, 0, -0.0001 )
####    siku.elements[9].flag_state = element.Element.f_steady
##    siku.elements[10].velo = ( 20, 4, 0 )
####    siku.elements[11].flag_state = element.Element.f_steady
##
##    siku.elements[13].velo = ( 20, 7, 0 )
####    siku.elements[14].flag_state = element.Element.f_steady
##    siku.elements[15].velo = ( 20, 5, 0 )
####    siku.elements[16].flag_state = element.Element.f_steady
##    siku.elements[17].velo = ( 20, 6.5, 0 )
####    siku.elements[18].flag_state = element.Element.f_steady
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'plot_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 6
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.defaults.contact_method = siku.CONTACT_METHODS['sweep']

    # name of file to load from
    #siku.defaults.loadfile = 'siku-2014-01-01-12:00:00.h5'
    siku.defaults.loadfile = 'save_test.h5'

    # ---------------------------------------------------------------------
    #  Diagnostics function for the winds
    # ---------------------------------------------------------------------
    
##    # We create a grid and append it to monitor grids
##    siku.diagnostics.wind_counter = 0
##    rg = regrid.Regrid()
##    mesh_01 = rg.globe_coverage( 5.0 )
##    siku.diagnostics.meshes.append( mesh_01 )
##    siku.diagnostics.wind.append( 
##        ( winds_diag, 0, siku.time.start, 2*siku.time.dt ) )

    # ---------------------------------------------------------------------
    #  Defaults
    # ---------------------------------------------------------------------

##    # without this - no wind will be added to calculations
##    siku.defaults.wind_source = siku.WIND_SOURCES['NMC']

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
    ###old version
    #os.system("gmtset PS_MEDIA=Custom_24cx20c ")

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    subprocess.call( "nice convert -density 300 -delay 10 drift*.eps drift.gif", \
                     shell=True )
    ###old version
    #os.system("convert -density 300 -delay 10 drift*.eps drift.gif")

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
    if t >= siku.time.last: #siku.uw.times[siku.time.update_index + 1]:
        status += siku.MASK['WINDS']
        siku.time.last = siku.time.finish#t

    # and change the winds here
    # ~!wind is changed with another call

    # and save the current time in a structure
    # ~!current time is saved in siku.time.last
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
##    #static polygons (generally shores) may be simply passed
##    if st & element.Element.f_static:
##        return
    
##    if st & element.Element.f_errored:
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
        if st & element.Element.f_errored: ##
            poly.write( '> -Gred -W0.1p,red \n' ) ##

##            #errored export lon-lat:
##            with open("errored"+str(i)+".txt", 'w') as erf:
##                for v in vert:
##                    erf.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
##                        str( v[1] )+'\n' )
            
        elif st & element.Element.f_special: ## elif -> if
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
