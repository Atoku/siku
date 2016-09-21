'''File: ice_nc.py, Simple module for loading and initializing ice coverage
with masked data by .nc file.
'''

import netCDF4
import math

from . import geocoords

# -------------------------------------------------------------
# Math utility
# -------------------------------------------------------------

def interpolate( Region, Values, Point ):
    '''Interpolates value by region boundaries (given as tuple of
    min and max lon and lat values), values in region corners
    (given as tuple of values) and point of interpolation (given
    as lot-lat pair)

    Units = DEGREES!!

    return: interpolated value
    '''

    # left-bot, right-bot, left-top and right-top values
    LB = Values[0]
    RB = Values[1]
    LT = Values[2]
    RT = Values[3]

    # interpolation parameters (should be between 0 and 1)
    to = (Point[0] - Region[0]) / (Region[1] - Region[0])
    ta = (Point[1] - Region[2]) / (Region[3] - Region[2])

    # semi-interpolated values vertically and horizontally
    L = LB + ta*( LT - LB )
    R = RB + ta*( RT - RB )
    B = LB + to*( RB - LB )
    T = LT + to*( RT - LT )

    # interpolated values (vertically and horizontally)
    LR = L + to*( R - L )
    BT = B + ta*( T - B )

    # mean value result
    return 0.5*( LR + BT )

def interpolate_disc( Region, Values, Point ):
    '''Discretely interpolates value by region boundaries (given
    as tuple of min and max lon and lat values), values in region
    corners (given as tuple of values) and point of interpolation
    (given as lot-lat pair)

    Units = DEGREES!!

    return: interpolated value
    '''
    
    # left-bot, right-bot, left-top and right-top values
    LB = Values[0]
    RB = Values[1]
    LT = Values[2]
    RT = Values[3]
    
    # interpolation parameters (should be between 0 and 1)
    to = (Point[0] - Region[0]) / (Region[1] - Region[0])
    ta = (Point[1] - Region[2]) / (Region[3] - Region[2])

    if to < 0.5:
        if ta < 0.5:    return LB
        else:           return LT
    else:
        if ta < 0.5:    return RB
        else:           return RT

    return None            

def get_ind( Ar, v ):
    '''Fast search of index for value insertion within some
    sorted collection.

    Return: index of element that is less (or equal) than 'v', while
        the next element is greater than 'v'.
        None is v < Ar[0], or v > Ar[-1]
    '''

    I = None
    N = len(Ar)-1;
    L = 0
    R = N
    
    i = L
    while L < R:
        i = L + (R-L) // 2
        
        if Ar[i] <= v and Ar[i+1] > v:
            return i;
        elif Ar[i] > v:
            R = i
        elif Ar[i+1] <= v:
            L = i+1

    return None

# -------------------------------------------------------------
# The data is being read into some interpolation grid
# -------------------------------------------------------------

class InterGrid:
    '''Simple class for storaging and processing initial data.
    Provides high-level interface.

    Units = DEGREES!!
    '''

    def __init__( self, filename = None ):
        '''Initializing class and loading if filename is given'''
        
        self.data = None
        self.lons = []
        self.lats = []
        self.grid = [[]]
        if filename:         
            self.load( filename )

    def load( self, filename, loN = 'lon', laN = 'lat', grN = 'Band1' ):
        '''Loading the dataset'''

        # just-in-case clearing
        if self.data:
            self.data.close()
            self.lons = []
            self.lats = []
            self.grid = [[]]

        # actually loading
        self.data = netCDF4.Dataset( filename )
        self.lons = self.data.variables[loN][:]
        self.lats = self.data.variables[laN][:]
        self.grid = self.data.variables[grN][:][:]

        # normilizing
        self.lons = [ geocoords.norm_lon( l ) for l in self.lons ]

        # min, max longitude and 'longitude width' for nice date-line
        # interpolation
        self.lo_m = self.lons[0] #min longitude
        self.lo_M = self.lons[-1] #max longitude
        
        self.lo_w = self.lo_M - self.lo_m #longitude width
        if self.lo_w < 0: self.lo_w += 360.

        # generating utility longitudes array for nice date-line interpolation
        self.lons_ = [ self.lons[0] ]
        for i in range(len(self.lons) - 1):
            if self.lons[i+1] < self.lons[i]:
                self.lons_.append( self.lons[i+1] + 360. )
            else:
                self.lons_.append( self.lons[i+1] )

    def get( self, lon, lat ):
        '''Return discretely interpolated value in point given as
        longitude and latitude.

        Units = DEGREES!!

        If lon-lat out of grid bounds - return None
        '''
        lo_m = self.lo_m
        lo_M = self.lo_M
        lo_w = self.lo_w

        # out-of-bounds check
        if self.lons[0] < self.lons[-1]:
            if lon < self.lons[0] or lon > self.lons[-1] or \
               lat < self.lats[0] or lat > self.lats[-1]:
                return None
        else:
            if (lon < self.lons[0] and lon > self.lons[-1]) or \
                lat < self.lats[0] or  lat > self.lats[-1]:
                return None
        
        # adjusting longitude range
        lon = geocoords.norm_lon( lon )
        lo = lon - lo_m
        if lo < 0: lo += 360.
        lon = lo + lo_m

        # searching longitude of cell
        oi = get_ind( self.lons_, lon )
##        oi = None
##        for i in range( len(self.lons_)-1 ):            
##            if self.lons_[i] < lon and self.lons_[i+1] > lon:
##                oi = i
##                break

        # searching latitude of cell
        ai = get_ind( self.lats, lat )        
##        ai = None
##        for i in range( len(self.lats)-1 ):
##            if self.lats[i] < lat and self.lats[i+1] > lat:
##                ai = i
##                break

        # additional safe check
        if oi == None:
            print( 'oi - None' )
            return None
        if ai == None:
            print( 'ai - None' )
            return None   

        # actual indexes
        li = oi
        ri = oi + 1
        bi = ai
        ti = ai + 1

        # interpolating
        return interpolate_disc( \
##        return interpolate( \
            (li, ri, bi, ti), \
            (self.grid[bi][li], self.grid[bi][ri], \
            self.grid[ti][li], self.grid[ti][ri]), \
            (lon, lat) )
