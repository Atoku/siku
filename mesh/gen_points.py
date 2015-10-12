#!/usr/bin/env python

'''

   gen_points.py

   Siku Project

   Generation of x,y,z triplets on unit sphere for generation of
   Voronoi diagram later on.

'''

from    math   import sin,cos,pow,ceil,floor,pi,sqrt,acos

import random
import sys
import types

# --------------------------------------------------------------------
def trivial( x ): return x

# --------------------------------------------------------------------
def cmp_key( x ): return x[1]

# --------------------------------------------------------------------
class sph_domain:
    '''Class sph_domain for building domains and random points coverage

    '''

    def __init__( self, phi=(0,2*pi), theta=(0,pi) ):
        '''
        Arguments (defining the region):
        phi   (tuple=(float,float)) - interval in phi spherical coordinate
        theta (tuple=(float,float)) - interval in theta s.c.

        Default: full range, no arguments = the whole sphere
 
        '''

        # intervals to define the region
        self.phi = ( phi[0], phi[1] )
        self.theta = ( theta[0], theta[1] )
        self.Dphi  = phi[1] - phi[0]
        self.Dtheta = theta[1] - theta[0]

        # list of points for the mesh
        self.mesh = []

    def point_count( self, psi ):
        '''Counts number of points for resolution psi into the domain

        Arguments:
        psi (float) -- resolution angle in degrees
        
        Returns:
        N (int) -- number of points inside the domain
        '''

        # degrees into radians
        psi = pi * psi / 180.0 

        Ntheta = int( ceil( self.Dtheta / psi ) )
        dtheta = self.Dtheta / Ntheta

        N = 0
        for j in range( Ntheta ):
            
            theta = j * dtheta
            
            Nphi = 1 + int( abs( floor( self.Dphi * sin( theta ) / dtheta ) ) )
            N += Nphi

        return N

    def make_points_random2( self, psi ):
        '''Genertion of the points. Returns a list of (x,y,z) triplets on unit
        sphere.
        
        '''
        res_list = []

        k = 0.5 / pi

        p0 = k * self.phi[0]
        p1 = k * self.phi[1]

        t0 = 0.5* ( 1 + cos( self.theta[1] ) )
        t1 = 0.5* ( 1 + cos( self.theta[0] ) )

        N = self.point_count( psi )
        
        # arccos method:
        for j in range( N ):
            
            U = random.uniform( p0, p1 )
            V = random.uniform( t0, t1 )

            phi = 2*pi* U
            theta = acos( 2*V - 1 )

            x,y,z = sph_coordinates( phi, theta )
            res_list.append( (x,y,z) )

        return res_list

    def filter_points( self, lst, dmin, key = trivial ):
        '''Filters the list by the distance criteria: each point cannot be
        closer than the distance d. The method is described in the
        documentation.

        Arguments:
        lst (list)     -- list to be filtered (not disturbed in place)
        dmin (float or function) 
                       -- distance (can be angle or actual distance)
        key (function) -- function to apply to the key to receive actual
                          distance in 3D space
        
        Returns: nst, new list with blue noise points
        
        '''

        mst = sorted( lst, key = cmp_key ) # sort by x

        nst = [ mst[0] ]            # resulting list: first point is
                                    # always there

        btype_dmin_func = isinstance( dmin, types.FunctionType )

        for i, m  in enumerate( mst[1:] ):

            good_point = True

            k = len( nst ) - 1      # index of the last element in nst

            if btype_dmin_func:
                d = key( dmin( *m ) )   # using dmin function to calculate
            else:
                d = key( dmin )

            while ( k > 0 and abs( cmp_key( m ) - cmp_key( nst[k] ) ) < d ):
                if ( sph_dist( m, nst[k] ) < d ):
                    good_point = False
                    break
                k -= 1

            if good_point: nst.append( m )

        return nst


# --------------------------------------------------------------------
def std_dist ( x, y, z ):
    return 1

# --------------------------------------------------------------------
def sph_coordinates( phi, theta ):
    x = cos( phi ) * sin( theta )
    y = sin( phi ) * sin( theta )
    z = cos( theta )
    return x,y,z


# --------------------------------------------------------------------
def make_points_polar( psi, dpsi ):
    '''Genertion of the points. Returns a list of (x,y,z) triplets on unit
    sphere

    '''
    psi = pi * psi / 180.0 # degrees into radians
    
    Ntheta = int( ceil( 2 * pi / psi ) )
    dtheta = 2 * pi / Ntheta

    res_list = []

    for j in range( Ntheta ):

        theta = j * dtheta

        Nphi = 1 + int( abs(floor( 2 * pi * sin( theta ) / dtheta ) ) )
        dphi = 2 * pi / Nphi

        for i in range ( Nphi ):
            phi =  i * dphi
            x, y, z = sph_coordinates( phi +
                                       random.uniform(-dpsi,dpsi), 
                                       theta +
                                       random.uniform(-dpsi,dpsi) )
            res_list.append( (x,y,z) )

    return res_list


# --------------------------------------------------------------------
def make_points_random( psi ):
    '''Genertion of the points. Returns a list of (x,y,z) triplets on unit
    sphere. The fastest method by Marsaglia.

    '''
    res_list = []

    # number of points with this angle psi?

    psi = pi * psi / 180.0 # degrees into radians
    Ntheta = int( ceil( 2 * pi / psi ) )
    dtheta = 2 * pi / Ntheta
    N = 0
    for j in range( Ntheta ):

        theta = j * dtheta

        Nphi = 1 + int( abs( floor( 2 * pi * sin( theta ) / dtheta ) ) )
        N += Nphi

    # Marsaglia method:

    for j in range( N ):

        X = 0
        Y = 0
        S = 2
        while S > 1: 
            X = random.uniform(-1.0,1.0)
            Y = random.uniform(-1.0,1.0)
            S = X*X + Y*Y
        
        M = sqrt( 1 - S )
        x = 2*X*M
        y = 2*Y*M
        z = 1 - 2*S
        res_list.append( (x,y,z) )

    return res_list

# --------------------------------------------------------------------
def make_points_random2( psi ):
    '''Genertion of the points. Returns a list of (x,y,z) triplets on unit
    sphere.

    '''
    res_list = []

    # number of points with this angle psi?

    psi = pi * psi / 180.0 # degrees into radians
    Ntheta = int( ceil( 2 * pi / psi ) )
    dtheta = 2 * pi / Ntheta
    N = 0
    for j in range( Ntheta ):

        theta = j * dtheta

        Nphi = 1 + int( abs( floor( 2 * pi * sin( theta ) / dtheta ) ) )
        N += Nphi

    # arccos method:

    for j in range( N ):

        U = random.random()
        V = random.random()

        phi = 2*pi* U
        theta = acos( 2*V - 1 )

        x,y,z = sph_coordinates( phi, theta )
        res_list.append( (x,y,z) )

    return res_list


# --------------------------------------------------------------------
def sph_dist( x0, x1 ):
    '''Distance on sphere

    '''
    dprod = x0[0]*x1[0] + x0[1]*x1[1] + x0[2]*x1[2]
    return acos( dprod )

# --------------------------------------------------------------------
def sph_dist_arc( x0, x1 ):
    '''Distance on sphere

    '''
    return x0[0]*x1[0] + x0[1]*x1[1] + x0[2]*x1[2]

# --------------------------------------------------------------------
def arc_deg_to_distance( arc ):
    '''Returns actual shortcut distance between two points that are
    separated by a specific arc set in degrees

    '''
    return sqrt( 2.0* ( 1 - cos( pi * arc / 180.0 ) ) )

# --------------------------------------------------------------------
def filter_points( lst, dmin, key = trivial ):
    '''Filters the list by the distance criteria: each point cannot be
    closer than the distance d. The method is described in the
    documentation.

    Arguments:
    lst (list)     -- list to be filtered (not disturbed in place)
    dmin (float)   -- distance (can be angle or actual distance)
    key (function) -- function to apply to the key to receive actual
           distance in 3D space
    
    Returns: nst, new list with blue noise points

    '''

    d = key( dmin )

    mst = sorted( lst, key = cmp_key ) # sort by x

    nst = [ mst[0] ]            # resulting list: first point is
                                # always there

    for i, m  in enumerate( mst[1:] ):

        good_point = True

        k = len( nst ) - 1      # index of the last element in nst

        while ( k > 0 and abs( cmp_key( m ) - cmp_key( nst[k] ) ) < d ):
            if ( sph_dist( m, nst[k] ) < d ):
                good_point = False
                break
            k -= 1

        if good_point: nst.append( m )

    return nst

# --------------------------------------------------------------------
def filter_points_func( lst, dmin, key = trivial ):
    '''Filters the list by the distance criteria: each point cannot be
    closer than the distance d. The method is described in the
    documentation.

    Arguments:
    lst (list)     -- list to be filtered (not disturbed in place)
    dmin (function)-- distance (can be angle or actual distance)
    key (function) -- function to apply to the key to receive actual
           distance in 3D space
    
    Returns: nst, new list with blue noise points

    '''

    mst = sorted( lst, key = cmp_key ) # sort by x

    nst = [ mst[0] ]            # resulting list: first point is
                                # always there

    for i, m  in enumerate( mst[1:] ):

        good_point = True

        k = len( nst ) - 1      # index of the last element in nst

        d = key( dmin( *m ) )   # using dmin function to calculate

        while ( k > 0 and abs( cmp_key( m ) - cmp_key( nst[k] ) ) < d ):
            if ( sph_dist( m, nst[k] ) < d ):
                good_point = False
                break
            k -= 1

        if good_point: nst.append( m )

    return nst

# --------------------------------------------------------------------
def north_south( x, y, z ):
    rval = 1.0
    if z < 0: rval = 10.0
    return rval

# --------------------------------------------------------------------
def small_spot( x, y, z ):
    rval = 5.0
    if x < 0.02:
        rval = 0.01              # 1km

    return rval

# --------------------------------------------------------------------
def main():

    random.seed()
    nst = []

    f = open ( 'mytest.xyz', 'w' )

    S0 = sph_domain()
    #S1 = sph_domain( phi = (0,pi/3), theta = (pi/2, 2*pi/3 ) )
    #beaufort-chukchi domain
##    S1 = sph_domain( phi = (2.62, 4.36), theta = (0.09, 0.44) )
    S2 = sph_domain( theta = (0, 0.44 ) )

    lst1 = S0.make_points_random2( 5.0 )
    nst1 = S0.filter_points( lst1, 4.0, key = arc_deg_to_distance )

#    lst2 = S1.make_points_random2( 0.5 )
#    nst2 = S1.filter_points( lst2, 0.9, key = arc_deg_to_distance  )

##    lst3 = S2.make_points_random2( 0.2 )
##    nst3 = S2.filter_points( lst3, 0.25, key = arc_deg_to_distance  )
    lst3 = S2.make_points_random2( 0.5 )
    nst3 = S2.filter_points( lst3, 0.9, key = arc_deg_to_distance  )

    
##    nst = nst1 + nst2 + nst3
    nst = nst1 + nst3
##    nst = nst1 + nst2

    for l in nst:
        f.write( '%f %f %f\n' % l )

    f.close()

    print "# orig points: ",  len( lst1 )
    print "# filtered points: ",  len( nst )
    
    return 0


# --------------------------------------------------------------------

if __name__ == '__main__':
    sys.exit( main() )
