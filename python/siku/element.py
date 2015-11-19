'''File: element.py, contains Element class

'''

import copy
import sys
import mathutils
mV = mathutils.Vector
import random

try:
    from siku import geocoords
except ImportError:
    import geocoords

class Element:
    '''Siku: Element class contains all the information necessary to
    initialize a single element

    '''

    # "static" flag values for state flag: must coincide with values
    # in element.hh

    f_free   = 0x1
    f_steady = 0x2
    f_static = 0x4
    f_special = 0x20

    def __init__( self, polygon = None, imat = None, st_loc_velo = None ):
        ''' Inits element. May init from polygon and imat. '''
        # copy all necessary data from polygon
        if polygon:
            self.A = polygon.A
            self.i = polygon.i
            self.sbb_rmin = polygon.sbb_rmin
            self.q = [ polygon.q[0],  polygon.q[1],  
                       polygon.q[2],  polygon.q[3] ]

            self.verts_xyz_loc = [ (P[0],P[1],P[2]) for P in
                                   polygon.poly_xyz_loc ]
        else:
            self.A = 0.0
            self.i = 0.0
            self.sbb_rmin = 0.0
            self.q = []
            self.verts_xyz_loc = []

        # search the name in materials
        self.imat = imat

        # default is free
        self.flag_state = self.f_free

        # monitor and control function names
        self.monitor = None
        self.control = None

        # thickness distribution by layers
        self.gh = None

        # default velocity and rotation in local coords ( vx, vy, rot )
        if st_loc_velo:
            self.velo = st_loc_velo
        else:
            self.velo = ( 0, 0, 0 )

        return

    def set_gh( self, gh, material ):
        '''Setting ice thickness distribution function. It also checks the
        correctness on material. It deep copies gh, so no need to
        preserve it from updates.

        gh -- ice thickness distribution g(h) by layers
        material -- material the element is built from

        '''
        # correctness check
        if len( gh ) != len( material.thickness_intervals ):
            print( "Error: set_gh,",
                   "length of gh must be same as length of h" )
            sys.exit( 1 )

        if sum( gh ) > 1.0:
            print( "Error: set_gh,",
                   "sum( g(h) ) must be 1.0 but it is not" )
            sys.exit( 1 )

        self.gh = copy.deepcopy( gh )

    def does_contain( self, point ):
        '''Checks whether the 'point' (in geographic lon-lat terms) is inside
        the polygon.

        Uses 2D vector multiplication and comparing the result with zero.
        '''
        # calculate point in local coords
        P = mathutils.Vector( geocoords.xyz_geographic( *point ) )
        q = mathutils.Quaternion( self.q )
        qc = q.conjugated()
        R = qc.to_matrix()
        p = R * P 
        pv = mV( p )

        l = len( self.verts_xyz_loc )
        
        # make edges list
        EL = [ mV( self.verts_xyz_loc[ (i + 1) % l ] )
               - mV( self.verts_xyz_loc[ i ] ) for i in range( l ) ]

        # checking cross prod sign - if all are positive - must be inside
        for i in range( len( self.verts_xyz_loc ) ):
            if cross2d( EL[i], pv - mV( self.verts_xyz_loc[i] ) ) < 0:
        
                return False      
        return True
    
    pass

# ---------------------------------------------------------------------
# external functions
# ---------------------------------------------------------------------

def cross2d( v1, v2 ):
    '''Returns the result of cross product of two 2D vectors, those are
    constructed like (v1.x, v1.y) and (v2.x, v2.y)
    '''
    return v1.x * v2.y - v2.x * v1.y
