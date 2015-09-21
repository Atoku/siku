'''File: polygon.py, contains Polygon class for calculation of polygon
parameters

'''

import mathutils                       # blender module
from . import geocoords                # geocoordinates

class Polygon:
    '''Siku: Polygon class

    Class that calculates the parameters for polygons such as area,
    moment of inertia (divided by mass), center of mass, quaternion
    coordinates, local coordinates

    All these functions use the convention where polygons are almost
    flat (side << R). Do not use it for too large polygons comparable
    with the whole sphere.

    Public methods:

      update -- update the object with new polygon, forgets the old one
                and calculate all the parameters

    '''

    def __init__( self ):
        '''Creates empty object Polygon and precomputes some important values

        '''
        return

    def update( self, poly ):
        '''Updates object Polygon using list of geographic coordinates,
        calculates all main parameters of the polygon.

        All Cartesian coordinates are given on a Unit sphere

        Arguments:
        poly (list) -- list of tuples (lon,lat) with geographic
                       coordinates of vertices in degrees. No
                       duplicate vertices should be included! All
                       polygons are assumed to be closed with last
                       vertex connected to the first vertex.

        Updates the state:
        self.A (float) -- Area of polygon
        self.I (float) -- Geom. moment of inertia
        self.P (mathutils.Vector) -- Cartesian coordinates of CM
        self.q (mathutils.Quaternion) -- Quaternion representing
                                         transformation from local
                                         frame to global frame.
        self.poly_xyz (list of mathutils.Vector) -- Cartesian
                                                    coordinates of all
                                                    vertices

        self.poly_xyz_loc (list of mathutils.Vector) -- Cartesian
                                                        local
                                                        coordinates of
                                                        all vertices

        '''
        self.poly = poly
        self.A, \
            self.i, \
            self.C, \
            self.q, \
            self.poly_xyz, \
            self.poly_xyz_loc, \
            = self.parameters_calculation()

        return

# ------------------------ PRIVATE METHODS: --------------------------------

    def restore_poly_xyz( self ):
        '''Calculates the list of Cartesian coordinates on unit sphres for
        each vertex in self.poly_xyz. Should not be used by end user.

        Returns:
        poly_xyz (list of mathutils.Vector) -- Cartesian coordinates
                                               of vertices

        '''

        poly_xyz = [ mathutils.Vector( geocoords.xyz_geographic( *vertex ) )
                     for vertex in self.poly ]

        return poly_xyz

    def parameters_calculation( self ):
        '''Calculates the actual center of mass (cm) in assumption of flat
        polygon as well as total area and geometric moment of inertia.

        Returns: A, I, C, Q, poly_xyz, poly_xyz_loc
        A (float) -- Area of polygon
        I (float) -- Geom. moment of inertia (e.g. div by mass: I/m)
        C (mathutils.Vector) -- Cartesian coordinates of center of mass
        Q (mathutils.Quaternion) -- rotation quaternion for polygon
        poly_xyz (list of mathutils.Vector) -- global Cartesian
                                               coordinates
        poly_xyz_loc (list of mathutils.Vector) -- local Cartesian
                                                   coordinates on unit
                                                   sphere

        '''
        
        # what we calculate
        C = mathutils.Vector( (0.0,0.0,0.0) ) # center of mass
        A = 0.0                 # area
        I = 0.0                 # geom. moment of inertia (divided by mass)
        poly_xyz = self.restore_poly_xyz()

        # geom pseudo-center
        P = mathutils.Vector( (0.0,0.0,0.0) )
        for p in poly_xyz: P += p
        P /= len( poly_xyz )

        # for speed we calculate all vectors Pi
        Pi = [ U - P for U in poly_xyz ]

        # Areas and Centers of triangles

        Ai = []
        Ci = []
        for i in range( len(Pi) ):
            j = ( i + 1 ) % len(Pi)
            Ai.append( Pi[i].cross(Pi[j]).length )
            Ci.append( poly_xyz[i] + poly_xyz[j] + P )
        A = sum( Ai )
        
        # Center of mass
        for i in range( len(Pi) ):
            C += Ai[i] * Ci[i]
        C /= ( 3 * A )
        
        # and moment of inertia
        # (http://en.wikipedia.org/wiki/List_of_moments_of_inertia)
        # checked manually for triangles by integration

        Pi = [ U - C for U in poly_xyz ] # new Pi related to CM now
        for i in range( 1, len(Pi) ):
            j = ( i + 1 ) % len(Pi)
            I += Ai[i] * ( Pi[j].length_squared +
                           Pi[i].dot(Pi[j]) +
                           Pi[i].length_squared )
            
        I /= ( 6.0 * A )

        # Calculate the initial quaternion for the position
        Q = geocoords.quat0( C )
##        #print(Q)
##        print( 'polyquat ' + str( Q.w ) + ' ' + str( Q.x ) + ' ' + \
##               str( Q.y ) + ' ' + str( Q.z ) )

        # Calculate local Cartesian unit coordinates
        qconj = Q.conjugated()
        R = qconj.to_matrix()
        poly_local_xyz = [ R * poly for poly in poly_xyz ]

        return A, I, C, Q, poly_xyz, poly_local_xyz

    pass
