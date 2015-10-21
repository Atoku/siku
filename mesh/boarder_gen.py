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
        for s in shapes:
            self.contour = self.contour + s.points
        self.contour = dupl_del( self.contour )

    def add_hp_verts( self, gen_dens, fil_dens, hpDomain = None ):
        '''Generates and appends vertices using hpgrid monule'''
        if hpDomain == None:
            grid = hpgrid.Grid(  )
        else:
            grid = hpgrid.Grid( hpDomain )

        grid.points_gen( gen_dens )
        grid.points_filter( fil_dens )
        self.verts = self.verts + grid.points

    def gener_boarders( self, thick, domain = None ):
        '''Generates file 'boarders.xyz' with list of points whithin
        'thick' distance from ocean coasts (in kilometers) inside domain.
        Also saves current contours in two files:
        'contours.ll' ('ll' for 'lon-lat') and 'contours.xyz'
        '''
        gc = geocoords
        contour = [ [ gc.norm_lon( c[0] ) , gc.norm_lat( c[1] ) ] \
                    for c in self.contour ]
        
        if domain:
            contour = [ c for c in contour if is_inside( c, domain ) ]
     
        temp = geofiles.lonlat_to_xyz( contour )

        geofiles.w_xyz( 'contours.xyz', temp )
        geofiles.w_lonlat( 'contours.ll', contour ) 
        
        points = contour + geofiles.xyz_to_lonlat( self.verts )
        if domain:
            temp = [ p for p in points if is_inside( p, domain ) ]
        else:
            temp = points
                    
        geofiles.w_lonlat( 'temp.ll', temp )

        ts = str( thick )
        subprocess.call( 'gmt gmtselect temp.ll -fg -C' + ts + \
                         'k/contours.ll > boarders.ll', shell=True )

        os.remove('temp.ll')

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
