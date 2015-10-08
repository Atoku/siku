'''file: poly_voronoi.py
module: poly_voronoi

Contains classes for parsing .voronoi.* files into suitable list for
scenario polygon initialization.
'''
try:
    from siku import geocoords
except ImportError:
    import geocoords

import mathutils

Vec = mathutils.Vector
latlon = geocoords.lonlat_deg_norm

#------------------------------------------------------------------------------
# Class Vert. Loads and contains vertices coords
#------------------------------------------------------------------------------

class Vert:
    '''Loads and contains vertices coords from .voronoi.xyz
    '''
    def __init__( self, file = None ):
        '''Init, try to load
        '''
        self.coords = []
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
        self.inds = []
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
# Class PolyVor. Provides methods for parsing files into list polygon vertices
#------------------------------------------------------------------------------
 
class PolyVor:
    '''A class for retrieving list of lists of polygon vertices from
    .voronoi.* files
    '''
    def __init__( self, coords_f = None, seq_f = None ):
        '''Init, try to load files
        '''
        self.coords = []
        self.verts = Vert()
        self.seq = Seq()
        if coords_f and seq_f:
            self.load( coords_f, seq_f )
        return

    def load( self, coords_f, seq_f ):
        '''Loads vertices from files
        '''
        self.verts.load( coords_f )
        self.seq.load( seq_f )

        for l in self.seq.inds:
            self.coords.append(
                [ latlon( Vec( self.verts.coords[ i-1 ] ) ) for i in l ]
                )
        return

    def filter( self, minlon, maxlon, minlat, maxlat ):
        '''Excludes all polygons, that have at least one vertex, located
        outside specified refion.
        '''
        temp = []
        
        for l in self.coords:
            flag = True
            for i in l:
                if i[0] < minlon or i[0] > maxlon or \
                   i[1] < minlat or i[1] > maxlat:
                    flag = False
                    break
            if flag:
                temp.append( l )
                
        self.coords = temp
        return

if __name__=='__main__':
    PV = PolyVor( 'mytest.voronoi.xyz', 'mytest.voronoi.xyzf' )
    PV.filter( 150, 250, 65, 85 )
    coords = PV.coords

