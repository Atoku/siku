'''interpol - module for interpolation of wind data for input coordinates list
uses NMCWind as a datasource
'''

import geocoords
import nmc
import wnd
import math

class Interpolator:
    def __init__( self, wind_data, grid_step_lat, grid_step_lon ):
        self.wind = wind_data
        self.step_la = grid_step_lat
        self.step_lo = grid_step_lon
        self.gener_grid()

    def gener_grid( self ):
        self.wind_grid = { self.wind.lat[la] : { self.wind.lon[lo] : \
            self.wind.wind[la][lo] for lo in range(len(self.wind.lon)) } \
            for la in range(len(self.wind.lat)) }

    def interpolate( self, in_lat, in_lon ):
        in_lat = self.norm_lat(in_lat)
        in_lon = self.norm_lon(in_lon)
        
        lat_ind = in_lat // self.step_la
        if in_lat == 90:
            lat_ind -= 1

        lon_ind = in_lon // self.step_lo
        if in_lon == 360:
            lon_ind = 0

        left = lon_ind * self.step_lo
        right = ( lon_ind + 1 ) * self.step_lo
        bottom = lat_ind * self.step_la
        top = ( lat_ind + 1 ) * self.step_la

        LB = self.wind_grid[bottom][left]
        LT = self.wind_grid[top][left]
        RB = self.wind_grid[bottom][self.norm_lon(right)]
        RT = self.wind_grid[top][self.norm_lon(right)]

        top_vec = self.proport( LT, RT, (in_lon - left) / (right - left) )
        bot_vec = self.proport( LB, RB, (in_lon - left) / (right - left) )

        return self.proport( bot_vec, top_vec, \
                             (in_lat - bottom) / (top - bottom) )

    def proport( self, vec1, vec2, t ):

##        X = vec1[0]+ (vec2[0]-vec1[0])*t
##        Y = vec1[1]+ (vec2[1]-vec1[1])*t
##        return (X, Y)

        def prop( v1, v2, t):
            return (v1 + (v2 - v1) * t)
        f1 = math.atan2( vec1[1], vec1[0] )
        f2 = math.atan2( vec2[1], vec2[0] )
        r1 = math.sqrt( vec1[0]*vec1[0] + vec1[1]*vec1[1] )
        r2 = math.sqrt( vec2[0]*vec2[0] + vec2[1]*vec2[1] )

        f = f1 + self.norm_ang_diff(f2 - f1) * t
        r = r1 + (r2 - r1) * t
            
        return ( r * math.cos(f), r * math.sin(f) )


    def norm_lat( self, lat ):
        while lat < -180:
            lat += 90
        while lat > 180:
            lat -= 90
        return lat

    def norm_lon( self, lon ):
        while lon < 0:
            lon += 360
        while lon >= 360:
            lon -= 360
        return lon

    def norm_ang_diff( self, diff ):
        while diff > math.pi:
            diff -= ( math.pi*2 )
        while diff < -math.pi:
            diff += ( math.pi*2 )
        return diff
