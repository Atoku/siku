'''file: poly_voronoi.py
module: poly_voronoi

Contains classes for parsing .voronoi.* files into suitable list for
scenario polygon initialization.
'''
import os
import subprocess

try:
    from siku import geocoords
    from siku import element
    from siku import geofiles
except ImportError:
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
# Class PolyVor. Provides methods for parsing files into list polygon vertices
#------------------------------------------------------------------------------
 
class PolyVor:
    '''A class for retrieving list of lists of polygon vertices from
    .voronoi.* files
    '''
    #default filters for GMT boarders forming
    default_ocean = 'gmt gmtselect temp.lli -Dl -Nk/s/s/s/s > oceanf.lli'
    default_land = 'gmt gmtselect temp.lli -Dl -Ns/k/k/k/k > landf.lli'
    
    def __init__( self, coords_f = None, seq_f = None ):
        '''Init, try to load files
        '''
        self.coords = [] #list of lists of verts` coord: [ [ (lon, lat) ] ]
        self.verts = Vert() #list of vertices` coords (x, y, z)
        self.seq = Seq() #list of lists of verts` indexes
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

    def filter_( self, minlon, maxlon, minlat, maxlat ):
        '''Excludes all polygons, that have at least one vertex, located
        outside specified region.
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

    def mark_boarders( self, Els, file_b, minlon = 0, maxlon = 360, \
                       minlat = -90, maxlat = 90 ):
        '''Marks all elements in 'Els' which contain at least one point from
        file_b as 'f_static'
        '''
        #board = Vert( file_b )
        verts = geofiles.r_lonlat( file_b )

        for v in verts:
            if v[0] < minlon or v[0] > maxlon or \
               v[1] < minlat or v[1] > maxlat:
                verts.remove( v )

        for e in Els:
            for v in verts:
                if e.does_contain( v ):
                    e.flag_state = element.Element.f_static
                    verts.remove( v )
                    break
        return 

    def get_boarder_by_gmt( self, land_filter=None, ocean_filter=None, \
                            vert_f='temp.lli', oceanf='oceanf.lli', \
                            landf='landf.lli' ):
        '''Returns list of border polygon indexes. Uses GMT for searching
        the polygons, those have both wet and dry vertices.
        '''

        tc = []
        for i in range( len ( self.coords ) ):
            p = self.coords[i]
            [ tc.append( [ v[0], v[1], i ] ) for v in p ]

        geofiles.w_lonlati( vert_f, tc )

        if ocean_filter == None:
            ocean_filter = self.default_ocean
        subprocess.call( ocean_filter, shell=True )

        if land_filter == None:
            land_filter = self.default_land
        subprocess.call( land_filter, shell=True )
                
        tc = geofiles.r_lonlati( landf )
        I = { p[2]:p[2] for p in tc }

        tc = geofiles.r_lonlati( landf )
        II = { p[2]:p[2] for p in tc }

        S = [ i for i in I if i in II ]

        os.remove( vert_f )
        os.remove( oceanf )
        os.remove( landf )
        
        return S

    def clear_the_land( self, ocean_filter=None, \
                        vert_f='temp.lli', filt_vert_f='oceanf.lli' ):
        '''Clears all polygons, that have no vertices located on water'''
        
        tc = []
        for i in range( len ( self.coords ) ):
            p = self.coords[i]
            [ tc.append( [ v[0], v[1], i ] ) for v in p ]

        geofiles.w_lonlati( vert_f, tc )

        if ocean_filter == None:
            ocean_filter = self.default_ocean
        subprocess.call( ocean_filter, shell=True )
 
        tc = geofiles.r_lonlati( filt_vert_f )

        I = { p[2]:p[2] for p in tc }

        self.coords = [ self.coords[i] for i in I ]

        os.remove( vert_f )
        os.remove( filt_vert_f )
        return
    
# ------------------------------------------------------------------------

if __name__=='__main__':
    PV = PolyVor( 'mytest.voronoi.xyz', 'mytest.voronoi.xyzf' )
    PV.filter( 150, 250, 65, 85 )
    coords = PV.coords

