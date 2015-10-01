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

######
from siku import gmt_Plotter
try:
    from gmt_Plotter import GMT_Plotter
except:
    GMT_Plotter = gmt_Plotter.GMT_Plotter


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

    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, -1 )
##    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, -1 )
##    w.make_test_field()
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

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[0]
    siku.time.last = siku.uw.times[0]
    siku.time.last_update = siku.time.last
    siku.time.finish = siku.uw.times[220]
    siku.time.dt = ( siku.time.finish - siku.time.start ) / 256

    # ---------------------------------------------------------------------
    # Polygon initialization
    # ---------------------------------------------------------------------
    
    P = polygon.Polygon()    # need to be done only once for all polygons,
                             # elements will be initialized using polygons
    
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------

    # coords = [ (37.61, 55.75),      # lon, lat convention
    #            (13.38, 52.52),
    #            (12.50, 41.90),
    #            (32.86, 39.93),
    #            (48.04, 46.32) ]

    coords = []
    siku.elements = []
    coords.append( [ (190.00, 70.7),      # lon, lat convention
               (195.00, 71.0),
               (196.00, 72.7),
               (192.00, 72.5),
               (190.00, 71.7) ] )
    coords.append( [ (190.00, 78.3),      # lon, lat convention
               (192.00, 77.0),
               (190.00, 75.7),
               (188.00, 77.0) ] )
    coords.append( [ (226.3, 73.3),      # lon, lat convention
               (226.3, 70.7),
               (223.7, 70.7),
               (223.7, 73.3) ] )
 
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
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'plot_config.py' )
    
    siku.diagnostics.monitor_freq = 5
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

    return 0

# --------------------------------------------------------------------------

def conclusions( siku, t ):
    print('creating .gif')
    os.system("convert -density 100 -delay 30 drift*.eps drift.gif")

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n
    # some specific checks should be placed.

    # primitive time stepping
##    if t > ( siku.time.last + siku.time.dt ):
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # step by NMC own time step
##    if t >= siku.uw.times[siku.time.update_index + 1]:
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # and change the winds here
    # ~!wind is changed with another call

    # and save the current time in a structure
    # ~!current time is saved in siku.time.last
    return status

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    if siku.diagnostics.step_count % siku.diagnostics.monitor_freq == 0:
        pic_name = 'drift%02d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_freq)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )
    return 0

# --------------------------------------------------------------------------

def drift_monitor( t, Q, Ps, i ):
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    ## plotting current frame (not each one) into .eps picture
    if siku.diagnostics.step_count % siku.diagnostics.monitor_freq == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        poly_name = 'Poly{n}.txt'.format( n = i )
        with open( poly_name , 'w' ) as poly:
            for v in vert:
                poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+str( v[1] )+'\n' )

#    print( lon, lat )

#    for p in Ps:
#        print (p)

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
