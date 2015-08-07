'''File: regrid.py, contains Regrid class for calculation of Regular Grids

'''

import math
import numpy
import mathutils                       # blender module

from . import geocoords                # geocoordinates

class Regrid:
    '''Siku: Regrid class

    Class for creating regular grids that can be used for monitoring
    of vector or scalar fields in some regions on the Globe.

    '''

    def __init__( self ):
        '''Creates empty object'''
        return

    def globe_coverage( self, resolution ):
        '''Returns a grid with full Globe coverage. Nodes are separated by
        resolution angle set in degrees (approximately).

        NOTE: Current implementation is pretty slow. It is suggested
        to use grid generation only on initialization step.

        Arguments:

        resolution - distance between neighbooring nodes in degrees of
        plane angle.

        '''

        result = []             # resulting list of coordinates (x,y,z)
        
        dphi_rad = math.pi * resolution / 180.0
        N_theta = math.ceil( math.pi / dphi_rad )

        dtheta = math.pi / N_theta;

        for j in range( N_theta ):
            theta = j * dtheta
            N_phi = 1 + math.floor( 2 * math.pi * math.sin( theta ) / dtheta )
            dphi_loc = 2 * math.pi / N_phi

            for i in range( N_phi ):
                phi = i * dphi_loc;
                x = geocoords.xyz_spherical( theta, phi ) # x is tuple
                result.append( x )

        return result

    pass
