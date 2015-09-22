'''File: element.py, contains Element class

'''

import copy
import sys

class Element:
    '''Siku: Element class contains all the information necessary to
    initialize a single element

    '''

    # "static" flag values for state flag: must coincide with values
    # in element.hh

    f_free   = 0x1
    f_steady = 0x2
    f_static = 0x4

    def __init__( self, polygon, imat ):

        # copy all necessary data from polygon
        self.A = polygon.A
        self.i = polygon.i
        self.sbb_rmin = polygon.sbb_rmin
        self.q = [ polygon.q[0],  polygon.q[1],  
                   polygon.q[2],  polygon.q[3] ]

        self.verts_xyz_loc = [ (P[0],P[1],P[2]) for P in
                               polygon.poly_xyz_loc ]

        # search the name in materials
        self.imat = imat

        # default is free
        self.flag_state = self.f_free

        # monitor and control function names
        self.monitor = None
        self.control = None

        # thickness distribution by layers
        self.gh = None

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
    
    pass
