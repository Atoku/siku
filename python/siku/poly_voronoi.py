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
    from siku import polygon
except ImportError:
    import geocoords
    import element
    import geofiles
    import polygon

import mathutils
import math

Vec = mathutils.Vector
latlon = geocoords.lonlat_deg_norm
norm = geocoords.norm_deg
Poly = polygon.Polygon
Quat = mathutils.Quaternion

#-----------------------------------------------------------------------------

def make_pairs( lst ):
    '''Utility: generates sorted list of pairs of indexes from raw list'''
    l = len(lst)
    res = []
    for i in range( l-1 ):
        for j in range(i+1, l):
            res.append( ( min(lst[i], lst[j]), max(lst[i], lst[j]) ) )
    res.sort()
    
    return res

def find_h( rc, r1, r2 ):
    '''Utility: find the altitude of triangle''' 
    d1 = (rc - r1).length
    d2 = (rc - r2).length
    d = (r2 - r1).length
    
    s = (d+d1+d2)*0.5
    sq = s*(s-d)*(s-d1)*(s-d2)
    
    if d == 0.0 or sq <= 0.0:
        return d1

    return 2 * math.sqrt( sq ) / d
    
def find_delta( poly ):
    '''Utility: calculates the radius of largest inscribed circle'''
    l = len(poly.poly_xyz)
    
    r = find_h( poly.C, poly.poly_xyz[-1], poly.poly_xyz[0] )
    for i in range(1, l):
        tr = find_h( poly.C, poly.poly_xyz[i-1], poly.poly_xyz[i] )
        if tr < r:
            r = tr
    return r

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
    #default filters for GMT borders forming
    default_ocean = 'gmt gmtselect temp.lli -Dl -Nk/s/s/s/s > oceanf.lli'
    default_land = 'gmt gmtselect temp.lli -Dl -Ns/k/k/k/k > landf.lli'
    
    def __init__( self, coords_f = None, seq_f = None ):
        '''Init, try to load files
        '''
        self.coords = [] #list of lists of verts` coord: [ [ (lon, lat) ] ]
        self.verts = Vert() #list of vertices` coords (x, y, z)
        self.seq = Seq() #list of lists of verts` indexes
        self.links = []
        self.init_links = [] #PRIVATE
        self.init_inds = [] #PRIVATE
        self.delta = None #PRIVATE
        if coords_f and seq_f:
            self.load( coords_f, seq_f )
        return

    def load( self, coords_f, seq_f ):
        '''Loads vertices from files
        '''
        self.verts.load( coords_f )
        self.seq.load( seq_f )

        ViP = [ [] for i in range(len(self.verts.coords)) ]
        c = 0

        for l in self.seq.inds:
            self.coords.append(
                [ latlon( Vec( self.verts.coords[ i-1 ] ) ) for i in l ]
                )

            ###all next is for iniital links          
            for j in l:           
                ViP[j-1].append(c)  ## adding poly index to each point of it
            c += 1

        #generating initial links       
        temp = { i:{} for i in range(len(self.coords)) } #preparing dict of
                                                         #dicts - polygon and
                                                         #it`s neighbours
        for v in ViP:
            l = make_pairs( v ) #raw pairs of contacting polygons
            for i in l:
                temp[i[0]][i[1]] = i[1] #temp =
                                        #= { poly1ID: { poly2ID: poly2ID } } 

        p = Poly()
        R = []
        IL = []
        for i1 in temp:
            p.update( self.coords[i1][:] ) #calculating polygon props
            R.append( find_delta( p ) ) #accumulating radiuses of 'inscribed'
                                        #circles

            IL.append( ( p.C, temp[i1] ) ) # [ (Vec, { p2ID:p2ID } ) ]

        self.delta = min( R ) #delta = minimum circle
        self.init_links = IL
        
        return

    def generate_links( self, Els ):
        '''Generates links (based upon init_links) from Els - list of
        Elements'''
        links = []

        reindex = {} #new indexes of old polygons (aftes all filters)

        for i in range(len(Els)):
            #props of new polygon
            c = Quat(Els[i].q).to_matrix() * Vec( (0.0, 0.0, 1.0) )

            #serching for 'new' polygons matching to 'old' ones
            for j in range(len(self.init_links)):
                l = self.init_links[j]
                if (c - l[0]).length < self.delta:
                    reindex[j] = i

        #reindexing Elements` links
        for i in range(len(Els)):
            l = self.init_links[i]
            for o in l[1]:
                t = reindex.get( o, None )
                if t != None:
                    links.append( (min(i, t), max(i, t)) )

        links = list(set(links)) #getting unique pairs (for sure)
        links.sort()
        
        self.links = links
        return links

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

    def mark_borders( self, Els, file_b, minlon = 0, maxlon = 360, \
                       minlat = -90, maxlat = 90 ):
        '''Marks all elements in 'Els' which contain at least one point from
        file_b as 'f_static'
        '''
        #bord = Vert( file_b )
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

    def get_border_by_gmt( self, Els = None, \
                           land_filter=None, ocean_filter=None, \
                            vert_f='temp.lli', oceanf='oceanf.lli', \
                            landf='landf.lli', verb = False):
        '''Returns list of border polygon indexes. Uses GMT for searching
        the polygons, those have both wet and dry vertices.
        '''

        if verb: print('-preparing file')
        tc = []
        if Els == None:
            for i in range( len ( self.coords ) ):
                p = self.coords[i]
                [ tc.append( [ v[0], v[1], i ] ) for v in p ]
        else:
            for i in range( len ( Els ) ):
                p = Els[i].verts_xyz
                for v in p:
                    t = geocoords.lonlat_deg_norm(\
                        mathutils.Vector( (v[0], v[1], v[2]) ) )
                    tc.append( [t[0], t[1], i] )
                    #print()
                #[ tc.append( [ v[0], v[1], i ] ) for v in p ]

        geofiles.w_lonlati( vert_f, tc )

        if verb: print('-filtering ocean')
        if ocean_filter == None:
            ocean_filter = self.default_ocean
        subprocess.call( ocean_filter, shell=True )  # most time spent here

        if verb: print('-filtering land')
        if land_filter == None:
            land_filter = self.default_land
        subprocess.call( land_filter, shell=True )  # most time spent here

        if verb: print('-finilizing')
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
        subprocess.call( ocean_filter, shell=True )  # most time spent here

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

