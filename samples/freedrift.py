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
    #  Wind initializations
    # ---------------------------------------------------------------------

    ###### TESTING!
    try:
        siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
        siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )
        siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, -1 )
        siku.time.start = siku.uw.times[1]
        siku.time.last = siku.uw.times[1]
        siku.time.finish = siku.uw.times[5]
        siku.time.dt = ( siku.time.finish - siku.time.start ) / 20
    except:
        print('wnd works bad')

    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------

    ## time inits are temporary moved to 'wind' section
    #siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    #siku.time.finish   = datetime.datetime  ( 2012, 3, 13 )
    #siku.time.finish   = datetime.datetime  ( 2012, 3, 12, 00, 00, 10 )
    #siku.time.dt       = datetime.timedelta ( seconds = 1 )
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    #siku.time.last = siku.time.start
    
    # ---------------------------------------------------------------------
    # Polygon initialization
    # ---------------------------------------------------------------------
    
    P = polygon.Polygon()    # need to be done only once for all polygons,
                             # elements will be initialized using polygons
    
    # ---------------------------------------------------------------------
    # elements. In our case a single polygon
    # ---------------------------------------------------------------------

    # coords = [ (37.61, 55.75),      # lon, lat convention
    #            (13.38, 52.52),
    #            (12.50, 41.90),
    #            (32.86, 39.93),
    #            (48.04, 46.32) ]
    
    coords = [ (190.00, 70.7),      # lon, lat convention
               (195.00, 71.0),
               (196.00, 72.7),
               (192.00, 72.5),
               (190.00, 71.7) ]
    
    P.update( coords )
    
    # Element declaration
    E = element.Element( polygon = P, imat = matnames['ice'] )
    E.monitor = "drift_monitor"
    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
           0.0, 0.0, 0.0, 0.0, 0.0 ]
    E.set_gh( gh, ice )
    
    # all elements in the list
    siku.elements = [ E ]
    
    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    siku.drift_monitor = drift_monitor

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

    return 0

# --------------------------------------------------------------------------
def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    # !!! some valid checks should be placed.
    # NMC time comparison is not clear.
    if t > ( siku.time.last + siku.time.dt ):
        print( "Time comparison" )
        status = siku.MASK['WINDS']

    # and change the winds here

    # and save the current time in a structure
    
    # test:
    print ( "We are in pretimestep, time:", t, status )

    return status

siku.callback.pretimestep = pretimestep

# --------------------------------------------------------------------------

def drift_monitor( t, Q, Ps ):

    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C
    lon, lat = geocoords.lonlat_deg( c )

#    print( lon, lat )

#    for p in Ps:
#        print (p)

    return

# --------------------------------------------------------------------------

def winds_diag( t, winds ):

    mesh = siku.diagnostics.meshes[0]
    ez = mathutils.Vector( (0,0,1) )

    fp = open( 'winds-%02d.txt' % (siku.diagnostics.wind_counter), 'w' )

    for i, w in enumerate( winds ):
        x = mathutils.Vector( mesh[i] )
        u = mathutils.Vector( w )
        uval = u.length
        lon, lat = geocoords.lonlat_deg( x )
        a = ez - x
        
        mdl = a.length * uval
        if ( mdl != 0 ):
            azimuth = 180 * math.acos( (a*u) / mdl ) / math.pi
            fp.write( "%f %f %f %f %f\n" % \
                      ( lon, lat, 0.25*uval, azimuth, 0.7*uval ) )
            

    fp.close()
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
