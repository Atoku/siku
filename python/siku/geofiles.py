'''File: geofiles.py
Module: geofiles (depends from siku module)

Provides reding, writing and converting files with geographic points
in different convensions. e.g. lon-lat -> x-y-z vertices convertions.

'''

import mathutils
MVec = mathutils.Vector

from siku import geocoords

# ------------------------------- READINGS ---------------------------------

def r_xyz( file ):
    '''Read [x, y, z] vertices from file into list 
    '''
    verts = []
    with open( file, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t ]
            verts.append( v )
    return verts

def r_xyzi( file ):
    '''Read [x, y, z, i] vertices from file into list where 'i' is silent index.
    '''
    verts = []
    with open( file, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t[:-1] ]
            v.append( int( t[-1] ) )
            verts.append( v )

    return verts

def r_lonlat( file ):
    '''Read (lon, lat) vertices from file into list
    '''
    verts = []
    with open( file, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t ]
            verts.append( v )
    return verts

def r_lonlati( file ):
    '''Read (lon, lat, i) vertices from file into list, where 'i' is a
    silent index
    '''
    verts = []
    with open( file, 'r') as inp:
        for l in inp:
            t = l.split()
            v = [ float(f) for f in t[:-1] ]
            v.append( int( t[-1] ) )
            verts.append( v )
    return verts

# ---------------------------- CONVERTING -------------------------------

def xyz_to_lonlat( verts ):
    '''Converts a list of (x, y, z) vertices into a list of (lon, lat)
    vertices
    '''
    cverts = [ geocoords.lonlat_deg_norm( MVec( v ) ) for v in verts ]
    return cverts

def lonlat_to_xyz( verts ):
    '''Converts a list of (lon, lat) vertices into a list of (x, y, z) 
    vertices
    '''
    cverts = [ geocoords.xyz_geographic( v[0], v[1] ) for v in verts ]
    return cverts

def xyzi_to_lonlati( verts ):
    '''Converts a list of (x, y, z, i) vertices into a list of (lon, lat, i)
    vertices, where 'i' is a silent index
    '''
    cverts = []
    for v in verts :
        t = list( geocoords.lonlat_deg_norm( MVec( v[:-1] ) ) )
        t.append( v[-1] )
        cverts.append( t )
    return cverts

def lonlati_to_xyzi( verts ):
    '''Converts a list of (lon, lat, i) vertices into a list of (x, y, z, i) 
    vertices, where 'i' is a silent index
    '''
    cverts = []
    for v in verts :
        t = list( geocoords.xyz_geographic( v[0], v[1] ) )
        t.append( v[-1] )
        cverts.append( t )
    return cverts

# ----------------------------- WRITINGS --------------------------------

def w_xyz( file, verts ):
    '''Write list of [x, y, z] vertices into file
    '''
    with open( file, 'w') as outp:
        for v in verts:
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + \
                        str( v[2] ) + '\n' )
    return

def w_xyzi( file, verts ):
    '''Write list of [x, y, z, i] vertices into file. 'i' is a sient index
    '''
    with open( file, 'w') as outp:
        for v in verts:
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + \
                        str( v[2] ) + '\t' + str( v[3] ) + '\n' )
    return

def w_lonlat( file, verts ):
    '''Write list of (lon, lat) vertices into file
    '''
    with open( file, 'w') as outp:
        for v in verts:
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + '\n' )
    return

def w_lonlati( file, verts ):
    '''Write list of (lon, lat, i) vertices into file. 'i' is a silent index
    '''
    with open( file, 'w') as outp:
        for v in verts:
            
            outp.write( str( v[0] ) + '\t' + str( v[1] ) + '\t' + \
                        str( v[2] ) + '\n' )
    return
