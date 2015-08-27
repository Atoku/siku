'''File: hpgrid.py
Module: hpgrid, internal module of siku module

Hierarchical points grid. A tree structure of points used to generate
Voronoi tesselations with appropriate resolutions.

'''

from    math   import sin,cos,pow,ceil,floor,pi,sqrt,acos, atan2
import random

import geocoords
from geocoords import ratio180,ratioPI,norm_lat,norm_lon,norm_delta

# --------------------------------------------------------------------
# --------------------------------------------------------------------

DEGREES = 0
RADIANS = 1

# --------------------------------------------------------------------
# --------------------------------------------------------------------

class Domain:
    '''Class: Domain
    A rectangular in spherical coordinates domain on the unit sphere
    '''

    def __init__( self, phi=(0,2*pi), theta=(0,pi), units=RADIANS):
        '''Arguments (defining the region):
        phi   (tuple=(float,float)) - interval in phi spherical coordinate
        theta (tuple=(float,float)) - interval in theta s.c.

        Default: full range, no arguments = the whole sphere
 
        '''
        if units == DEGREES:
            phi = ( phi[0]*ratio180, phi[1]*ratio180 )
            theta  = (theta[0]*ratio180, theta[1]*ratio180 )
        
        #assert( phi[0] < phi[1] )
        assert( theta[0] < theta[1] )

        # intervals to define the region
        self.phi = ( phi[0], phi[1] )
        self.theta = ( theta[0], theta[1] )
        self.Dphi  = ( phi[1] - phi[0] )
        self.Dtheta = theta[1] - theta[0]

    def point_count( self, psi, units=DEGREES ):
        '''Counts number of points for resolution psi into the domain. The
        result is how many uniformly distributed points would be
        necessary to create a point net of psi resolution.

        Arguments:
        psi (float) -- resolution angle in degrees
        units (DEGREES/RADIANS) -- how psi is set (default: DEGREES)
        
        Returns:
        N (int) -- number of points inside the domain

        '''

        # degrees into radians if necessary
        if units == DEGREES:
            psi = psi*ratio180 

        Ntheta = int( ceil( self.Dtheta / psi ) )
        dtheta = self.Dtheta / Ntheta

        N = 0
        for j in range( Ntheta ):
            
            theta = j * dtheta
            
            Nphi = 1 + int( abs( floor( self.Dphi * sin( theta ) / dtheta ) ) )
            N += Nphi

        return N

    def includes( self, domain2 ):
        '''Compares domains
    
        Arguments: 
        domain2 (Domain) -- domain that is checked on being included
        into calling object domain

        Returns:
        True if domain2 is inside calling domain
        False otherwise

        '''
        return ( domain2.phi[0] >= self.phi[0] 
                 and
                 domain2.phi[1] <= self.phi[1]
                 and
                 domain2.theta[0] >= self.theta[0]
                 and
                 domain2.theta[1] >= self.theta[1] )

# --------------------------------------------------------------------
# --------------------------------------------------------------------
def phi_theta( x, y, z ):
    r = sqrt( x*x + y*y + z*z )
    theta = atan2( y, x )*ratioPI
    phi = acos( z / r )*ratioPI-90.0
    phi = norm_lon( phi )
    theta = norm_lat( theta )
    return ( phi, theta )

class Grid:
    '''
    A node Grid, a subgrid of the large tree
    '''

    def __init__( self, domain = None, parent=None ):
        self.parent = parent
        self.children = []
        self.domain = domain
        self.points = []
        self.points_angular = []

        if not self.domain:
            self.domain = Domain()

        return

    def points_gen( self, psi, units=DEGREES ):
        '''Genertion of the points randomly distributed with *average*
        resolution of psi.

        Arguments:
        psi (float) -- resolution
        units (DEGREES/RADIANS) -- how psi is set (default: DEGREES)

        '''
        k = 0.5 / pi

        p0 = k * self.domain.phi[0]
        p1 = k * self.domain.phi[1]

        t0 = 0.5* ( 1 + cos( self.domain.theta[1] ) )
        t1 = 0.5* ( 1 + cos( self.domain.theta[0] ) )

        N = self.domain.point_count( psi, units )
        print("points amount to generate " + str(N))
        self.points_clear()
        # arccos method:
        for j in range( N ):
            
            U = random.uniform( p0, p1 )
            V = random.uniform( t0, t1 )

            phi = 2*pi* U
            theta = acos( 2*V - 1 )

            x,y,z = geocoords.xyz_spherical( theta, phi )

            self.points.append( (x,y,z) )
            self.points_angular.append( (phi*ratioPI, theta*ratioPI) )

        return

    def points_filter( self, psi, units = DEGREES ):
        '''Filters the list of poinst (in place) by the distance criteria:
        each point cannot be closer than the angle psi. The method is
        described in the documentation.

        Arguments:
        psi (float)    -- angular max allowed distance between points
        units (DEGREES/RADIANS) -- how psi is set (default: DEGREES)

        '''

        mst = sorted( self.points, key = lambda x: x[1] ) # sort by x

        nst = [ mst[0] ]            # resulting list: first point is
                                    # always there 
        nst_ang = [ phi_theta( mst[0][0], mst[0][1], mst[0][2]) ]

        # degrees into radians if necessary
        if units == DEGREES:
            psi = psi*ratio180 

        Cpsi = cos( psi )

        for i, m  in enumerate( mst[1:] ):

            good_point = True

            k = len( nst ) - 1      # index of the last element in nst

            while ( k > 0 ):
                if ( m[0]*nst[k][0] + m[1]*nst[k][1] + m[2]*nst[k][2] > Cpsi ):
                    good_point = False
                    break
                k -= 1

            if good_point:
                nst.append( m )
                nst_ang.append(phi_theta( nst[-1][0], nst[-1][1], nst[-1][2]))

        self.points = nst
        self.points_angular = nst_ang
        print('points left ' + str(len(self.points)))
        return

    def points_clear( self ):
        '''Clear the list of points
        '''
        self.points = []
        self.points_angular = []

# --------------------------------------------------------------------
# --------------------------------------------------------------------

# class Hpgrid:
#     '''
#     Class: Hpgrid

#     Hierarhical point grid, a tree of point grids to cover the globe

#     '''

#     def __init__( self, root=None ):
#         self.root = root
        
#     def create_subgrid( self, 
#                         resolution, 
#                         resolution_min,
#                         domain = None, 
#                         parent = None,
#                         units = DEGREES ):
        
#         grid = Grid.grid( domain, parent )

#         if not parent: 
#             assert( not self.root )
#             self.root = grid
#         else:
#             grid.parent.children.append( grid )
            
        

#         pass
