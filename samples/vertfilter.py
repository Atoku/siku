import os
import subprocess

import siku
try:
    from siku import polygon
    from siku import geocoords
    from siku import element
    from siku import geofiles
except ImportError:
    import polygon
    import geocoords
    import element
    import geofiles

import mathutils

Vec = mathutils.Vector
latlon = geocoords.lonlat_deg_norm
norm = geocoords.norm_deg

#------------------------------------------------------------------------------
# Class Vert. Loads and contains vertices coords
#------------------------------------------------------------------------------

class Vert:
    '''Loads and contains vertices coords from .voronoi.xyz
    '''
    def __init__( self, file = None ):
        '''Init, try to load
        '''
        self.coords = [] #list of verts [x, y, z]
        if file:
            self.load( file )
        return

    def load( self, file ):
        '''Load from .voronoi.xyz'''
        with open( file, 'r+') as inp:
            t = []
            for line in inp:
                t = line.split()
                self.coords.append( [ float(f) for f in t ] );
        return 

#------------------------------------------------------------------------------
# Class Seq. Loads and contains vertices indexes
#------------------------------------------------------------------------------

class Seq:
    '''Loads and contains vertices indexes from .voronoi.xyzf
    '''
    def __init__( self, file = None ):
        '''Init, try to load
        '''
        self.inds = [] #list of lists of vertices` indexes
        if file:
            self.load( file )
        return

    def load( self, file ):
        '''Load from .voronoi.xyzf'''
        with open( file, 'r+') as inp:
            t = []
            for line in inp:
                t = line.split()
                self.inds.append( [ int(i) for i in t[:-1] ] );
        return 

#------------------------------------------------------------------------------
# 
#------------------------------------------------------------------------------
 
class VorFilter:

    def __init__( self, coords_f = None, seq_f = None ):
        '''Init, try to load files
        '''
        self.cf = coords_f
        self.sf = seq_f
        self.P = polygon.Polygon()
        
        self.coords = [] #list of lists of verts` coord: [ [ (lon, lat) ] ]
        self.verts = Vert() #list of vertices` coords (x, y, z)
        self.seq = Seq() #list of lists of verts` indexes
        self.control = {} #dict of tuples: key = point index in self.Vert
                          # first val = index of poly in self.coord
                          # second val = index of vertice in poly
        self.coordI = [] #list of indexes [in self.vert] of points in coord
        
        if coords_f and seq_f:
            self.load( coords_f, seq_f )
        return

    def load( self, coords_f, seq_f ):
        '''Loads vertices from files
        '''
        self.verts.load( coords_f )
        self.seq.load( seq_f )

        for i in range(len(self.seq.inds)):
            l = self.seq.inds[i]

            t = []
            tt= []
            for j in range(len( l )):
                I = l[j] - 1
                t.append( latlon( Vec( self.verts.coords[ I ] ) ) )
                tt.append( I )
                
                try:
                    self.control[I].append( ( i, j ) )
                except:
                    self.control[I] = [ ( i, j ) ]

            self.P.update( t )
            ttt = [ (k[0], k[1]) for k in self.P.poly_xyz_loc ]
            
            self.coords.append( ttt )
            self.coordI.append( tt )
            
        return

    def save( self, coords_f = None, seq_f = None ):
        if (coords_f == None and self.cf == None) or \
           (seq_f == None and self.sf == None):
            print( 'no filenames! can`t save!' )
            return
        
        if coords_f == None:
            coords_f = self.cf

        if seq_f == None:
            seq_f = self.sf

        with open( coords_f, 'w' ) as xyz:
            pass
            ##UNDONE!
        
        with open( seq_f, 'w' ) as xyzf:
            pass
            ##UNDONE!
        
        return

    def filter_( self ):

        r = []

        for i in range(len(self.coords)):
            c = self.coords[i]

            t = is_convex( c )
            if t:
                I = self.coordI[i][t]
                tex = 'polygon: ' + str( i ) + \
                      ', vert (global index): ' + str( I ) + \
                      ', also in polygons: \n'
                for a in self.control[I]:
                    tex = tex + str( a[0] ) + '\n'
                r.append( tex )       
        
        return r
    
#------------------------------- private utils -------------------------

def is_convex( poly, tolerance = 0 ):
    ''' check if the polygon is convex.
    if is - return None
    if not - return index of first vertex that is 'pushed inside'
    the poly
    '''

    if len(poly) < 3:
        return None

    l = len(poly)

    tv1 = subs( poly[1], poly[0] )
    tv2 = subs( poly[2], poly[1] )
    td1 = cross( tv1, tv2 )
    td2 = 0

    for i in range(l):
        tv1 = tv2
        tv2 = subs( poly[(i+2)%l], poly[(i+1)%l] )

        td2 = cross( tv1, tv2 )
        if td2*td1 < tolerance:
            return i
        td1 = td2
    
    return

def subs( p1, p2 ):
    return ( p1[0]-p2[0], p1[1]-p2[1] )

def cross( p1, p2 ):
    return p1[0]*p2[1] - p1[1]*p2[0]

###==========================================================================

def main():
    v = VorFilter( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )            
    tex = v.filter_()
    print( 'bad polygons: ' + str(len( tex )) +'\nprint meta? (y/n): ' )
    s = input()

    if s == 'y':
        for t in tex:
            print( t )
        print('''\n!!!!if there are two identic polygon indexes -
the vertex is first-final in that polygon!!!!''' )

if __name__ == '__main__':
    main()
 
