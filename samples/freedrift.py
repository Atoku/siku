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

##import sys
##import os
##try:
##    os.chdir('./python/siku/')
##    sys.path.append(os.getcwd())
##    os.chdir('./../../')
##except:
##    pass

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
    
    # ---------------------------------------------------------------------
    #  Wind initializations (NMC grid example)
    # ---------------------------------------------------------------------

    #start time index
    st_t_ind = 1
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
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
    siku.time.finish = siku.uw.times[st_t_ind] + 72* hour 
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 480

    # ---------------------------------------------------------------------
    # Polygon initialization
    # ---------------------------------------------------------------------
    
    P = polygon.Polygon()    # need to be done only once for all polygons,
                             # elements will be initialized using polygons
    
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []
##############
    #fan
##    coords.append( [ (190.00, 70.7),      # lon, lat convention
####               (195.70, 70.5),#
##               #(195.0, 71.0),#
####               (196.00, 72.7),
##               (194.50, 71.0),
##               (195.50, 72.7),
##               (192.00, 72.5),
##               (190.00, 71.7) ] )
##    #~rhomb
##    coords.append( [ (190.00, 70.7),      # lon, lat convention
##               (194.0, 70.0),
##               (198.0, 70.0),
##               (195.00, 71.0) ] )
##    #~rect
##    coords.append( [ (195.00, 71.0),      # lon, lat convention
##               (198.0, 70.0),
##               (198.3, 72.7),
##               (196.0, 72.7) ] )

##    #~rhomb2
##    coords.append( [ (183.00, 71.5),      # lon, lat convention
##               (187.0, 70.5),
##               (193.0, 71.5),
##               (187.90, 71.5) ] )
##    #~rhomb
##    coords.append( [ (190.00, 70.7),      # lon, lat convention
##               (194.0, 70.0),
##               (198.0, 70.0),
##               (195.00, 71.0) ] )

##############
    # ---------------------- voronoi initialization ------------------------
    PV = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )
    PV.filter( 0, 360, 65, 90 )
##    PV.filter( 150, 250, 65, 85 )
    coords = PV.coords
 
    for c in coords:
        P.update( c )
     
        # Element declaration
        E = element.Element( polygon = P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
               0.0, 0.0, 0.0, 0.0, 0.0 ]
        E.set_gh( gh, ice )
        
        # all elements in the list
        siku.elements.append( E )

    print("preparing boarders")
    PV.mark_boarders( siku.elements, 'boarders.txt', 0, 360, 60, 90 )
    print("boarders are ready")
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'plot_config.py' )
    
    siku.diagnostics.monitor_period = 4
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

    siku.defaults.contact_method = siku.CONTACT_METHODS['sweep']

    # ---------------------------------------------------------------------
    #  Diagnostics function for the winds
    # ---------------------------------------------------------------------
    
    # We create a grid and append it to monitor grids
    siku.diagnostics.wind_counter = 0
    rg = regrid.Regrid()
    mesh_01 = rg.globe_coverage( 5.0 )
    siku.diagnostics.meshes.append( mesh_01 )
    siku.diagnostics.wind.append( 
        ( winds_diag, 0, siku.time.start, 2*siku.time.dt ) )

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
    os.system("gmtset PS_MEDIA=Custom_24cx20c ")

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    os.system("convert -density 300 -delay 10 drift*.eps drift.gif")

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    # cleaning polygons file for further filling
    try:
        os.remove( 'Polygons.txt' )
    except:
        pass

    # some specific checks should be placed.

    # primitive time stepping
##    if t > ( siku.time.last + siku.time.dt ):
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # step by NMC own time step
    if t >= siku.uw.times[siku.time.update_index + 1]:
        status += siku.MASK['WINDS']
        siku.time.last = t

    # and change the winds here
    # ~!wind is changed with another call

    # and save the current time in a structure
    # ~!current time is saved in siku.time.last
    return status

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'drift%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )
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

        with open( 'Polygons.txt', 'a' ) as poly:
            if st == element.Element.f_static:
                poly.write( '> -Gbrown -W0.1p,lightBlue \n' )
            elif st == element.Element.f_steady:
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

## Commented to stop that file breeding while other modules are being tested
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
# velocity field settings: sources for velocities, where to get 
# ---------------------------------------------------------------------



# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
