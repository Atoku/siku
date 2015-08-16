'''File: geocoords.py
Module: geocoords, internal module of siku module

Makes transformation between geographic coordinate system and Cartesian
coordinate system with center in the center of sphere. Assuming that
everything is on a sphere.

'''

from   math import sin,cos,acos,atan,pi,atan2
import mathutils

ratio180 = pi / 180.0        # antidegree constant
ratioPI  = 180.0 / pi        # antiradian constant

def xyz_geographic( lon, lat ):
    '''Cartesian extrinsic normalized coordinates calculated by geographic
    coordinates in degrees
    
    Arguments:
    lon (float) -- longitude in degrees
    lat (float) -- latitude in degrees
    
    Returns: x,y,z
    x,y,z (float) -- Cartesian coordinates of a point on a unit sphere

    '''
    theta = ratio180 * ( 90.0 - lat )
    phi   = ratio180 * lon
    
    Stheta = sin( theta )
    Ctheta = cos( theta )
    Sphi   = sin( phi )
    Cphi   = cos( phi )
    
    return Stheta * Cphi, Stheta * Sphi, Ctheta

def xyz_spherical( theta, phi ):
    '''Cartesian extrinsic normalized coordinates calculated by spherical
coordinates in radians
    
    Arguments:
    theta (float) -- theta in radians
    phi (float)   -- longitude (phi) in radians
    
    Returns: x,y,z
    x,y,z (float) -- Cartesian coordinates of a point on a unit sphere

    '''
    Stheta = sin( theta )
    Ctheta = cos( theta )
    Sphi   = sin( phi )
    Cphi   = cos( phi )
    
    return Stheta * Cphi, Stheta * Sphi, Ctheta

    
def lonlat_rad( u ):
    '''Restores longitude and latitude by x,y,z coordinates in radians
    
    Arguments:
    u (mathutils.Vector) -- Decart coordinates on a sphere
    
    Returns: lon,lat
    lon, lat (float) -- Longitude and latitude in radians
    
    '''
    r = u.length
    
    theta = acos( u.z / r )
    phi = atan2( u.y, u.x ) # atan2 is a special function that
    # always works, see python math
    # documentation
    
    return phi, 0.5*pi - theta
    
def lonlat_deg( u ):
    '''Restores longitude and latitude by x,y,z coordinates in degrees
    
    Arguments:

    u (mathutils.Vector) -- Decart coordinates on a sphere
    
    Returns: lon,lat

    lon, lat -- Longitude and latitude in degrees
    
    '''
    lon_rad, lat_rad = lonlat_rad( u )
    return ratioPI * lon_rad, ratioPI * lat_rad

def quat0( u ):
    '''Calculates the quaternion-0 for a point relative to the North
    pole. As there are the whole set of possible quaternions, the
    calculated is the shortest quaternion

    Arguments:

    u (mathutils.Vector) -- Decart coordinates on a sphere

    Returns: q0

    q0 (mathutils.Quaternion) -- q0, a shortest path rotation to move
                                 u to ez.

    '''
    theta = acos( u.z / u.length )
    ez = mathutils.Vector( (0.0,0.0,1.0) )

    # normalization of the axis is necessary: checked!
    return mathutils.Quaternion( u.cross(ez).normalized(), - theta )