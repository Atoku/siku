'''File: boarder_gen.py
Module: boarder_gen

Class: Boarder

Provides functionality for generating lists and files of land boarders` vertices
based upon shapefiles and GMT filtering inside specified domain.
'''

import sys
import os
import subprocess
import shapefile
import mathutils

import siku
from siku import hpgrid
from siku import geocoords
from siku import geofiles
GC = geocoords

# -------------------------------------------------------------------------
# In this module all fields named exactly as 'domain' have to be 4-tuple in
# convension (minlon, maxlon, minlat, maxlat), or None if it`s indefinite
# -------------------------------------------------------------------------

class Boarder:
    '''A class that provides functionality for generating lists and files of
    land boarders` vertices based upon shapefiles and GMT filtering inside
    specified domain.
    '''
    def __init__( self, sfile = None ):
        '''Init and try to load shapefile'''
        self.sf = None
        self.shapes = None
        self.verts = [] #in (x, y, z)
        self.contour = [] #in (lon, lat)
        self.boarders = [] #in (x, y, z)
        
        if sfile:
            self.load( sfile )
        return

    def load( self, sfile ):
        '''Load vertices from shapefile'''
        self.sf = shapefile.Reader( sfile )
        self.shapes = self.sf.shapes()
        return

    def make_contour( self, shapes = None ):
        '''Try to make cotour from shapes. Self.shapes by default'''
        if shapes == None:
            shapes = self.shapes

        self.contour = []
        self.add_shapes_contour( shapes )

    def add_shapes_contour( self, shapes ):
        '''Add specific shapes list to self.contour'''
        for s in shapes:
            self.contour = self.contour + s.points
        self.contour = dupl_del( self.contour )

    def add_contour( self, file ):
        '''Adds contour points from specific table file in (lon, lat) convension
        '''
        with open( file, 'r' ) as inp:
            for l in inp:
                self.contour.append( [ float( w ) for w in l.split() ] )

    def filter_contour( self, dens, domain=None ):
        '''Filters contour vertices (only inside domain, if one is given)
        for lowering resolution.'''
        verts = []
##        contour = [ [ GC.norm_lon( c[0] ) , GC.norm_lat( c[1] ) ] \
##            for c in self.contour ]
        if domain:
            for c in self.contour:
                if is_inside( c, domain ):
                    verts.append( c )
                    self.contour.remove( c )
        else:
            verts = self.contour[:]
            self.contour= []

        verts = geofiles.lonlat_to_xyz( verts )
        g = hpgrid.Grid()
        g.points = verts
        g.points_filter( dens )
        verts = geofiles.xyz_to_lonlat( g.points )

        self.contour = self.contour + verts

    def add_hp_verts( self, gen_dens, fil_dens, domain = None ):
        '''Generates and appends vertices using hpgrid monule'''
        if domain == None:
            grid = hpgrid.Grid(  )
        else:
            hpDomain = hpgrid.Domain( \
                phi = ( GC.deg_to_rad_ll( domain[0], domain[1] ) ), \
                theta = ( GC.ratio180 * ( 90 - GC.norm_lat( domain[3] ) ), \
                          GC.ratio180 * ( 90 - GC.norm_lat( domain[2] ) ) ) )
            grid = hpgrid.Grid( hpDomain )

        grid.points_gen( gen_dens )
        grid.points_filter( fil_dens )
        self.verts = self.verts + grid.points

    def gener_boarders( self, thick, dens = None, domain = None, \
                        file_b = 'boarders.ll', file_s = 'shapes' ):
        '''Generates files:
        -'file_b.ll' (lon-lat convension) list of points filtered by
        optional 'dens' density (hpgrid method) and located whithin 'thick'
        distance from ocean coasts (in kilometers) inside domain.
        -'file_s[.ll,.xyz]' all inner vertices filtered by optional 'dens'
        density.
        -'contours[.ll,.xyz]' current contours inside domain.
        '''
        contour = [ [ GC.norm_lon( c[0] ) , GC.norm_lat( c[1] ) ] \
                    for c in self.contour ]

        #selecting contour in domain
        if domain:
            contour = [ c for c in contour if is_inside( c, domain ) ]

        temp = geofiles.lonlat_to_xyz( contour )

        #outputting contours
        geofiles.w_xyz( 'contours.xyz', temp )
        geofiles.w_lonlat( 'contours.ll', contour )

        #merging contours and verts inside domain
        if domain:
            contour = [ p for p in contour[:] if is_inside( p, domain ) ]
        else:
            pass
        points = contour + geofiles.xyz_to_lonlat( self.verts )

        points = geofiles.lonlat_to_xyz( points )

        #filtering by 'dens'
        if dens:
            tg = hpgrid.Grid()
            tg.points = points
            tg.points_filter( dens )
            points = tg.points

        #outputting shapes
        geofiles.w_xyz( file_s + '.xyz', points )
        points = geofiles.xyz_to_lonlat( points )
        geofiles.w_lonlat( file_s + '.ll', points )

        #creating boarders with GMT
        ts = str( thick )
        subprocess.call( 'gmt gmtselect ' + file_s + '.ll -fg -C' + ts + \
                         'k/contours.ll > ' + file_b + '.ll', shell=True )

        return
        

# ----------------------- technical (private) methods --------------------
    
def dupl_del( verts ):
    '''Deletion of duplicated vertices'''
    sverts = sorted ( verts ) #sorted collection

    p = sverts[0] #previous vertex
    fverts = [ p ] #filtered collection

    for i, m in enumerate( sverts[1:] ):
        if m[0] == p[0] and m[1] == p[1]:
            continue
        else:
            fverts.append( m )
            p= m
    return fverts

def is_inside( vert, domain ):
    '''Checks if vertex given as (lon, lat) is inside the region given as
    (minlon, maxlon, minlat, maxlat) in degrees'''
    if vert[0] > domain[0] and vert[0] < domain[1] and \
       vert[1] > domain[2] and vert[1] < domain[3]:
        return True
    else:
        return False
