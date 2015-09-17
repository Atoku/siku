'''interpol - module for interpolation of wind data for input coordinates list
uses NMCWind as a datasource
'''
import math

from siku import geocoords

try:
    from geocoords import norm_lat,norm_lon,norm_delta,norm_delta_rad
except:
    norm_lat = geocoords.norm_lat
    norm_lon = geocoords.norm_lon
    norm_delta = geocoords.norm_delta
    norm_delta_rad = geocoords.norm_delta_rad
from siku import nmc
from siku import wnd

class Interpolator:
    '''Class interpolator provides several methods for simple interpolation
    of wind data by regular lat-lon grid from 'wnd' module
    '''
    def __init__( self, wind_data, grid_step_lat, grid_step_lon ):
        '''Generates interpolation grid
        parameters:
        wnd - wind_data
        angle steps of grid by lat and lot
        '''
        self.reinit( wind_data, grid_step_lat, grid_step_lon)

    def reinit( self, wind_data, grid_step_lat, grid_step_lon):
        '''Generates new interpolation grid with new input data'''
        self.wind = wind_data
        self.step_la = grid_step_lat
        self.step_lo = grid_step_lon
        self.gener_grid()

    def gener_grid( self ):
        '''Technical function for generation the grid'''
        self.wind_grid = { self.wind.lat[la] : { self.wind.lon[lo] : \
            self.wind.vec[la][lo] for lo in range(len(self.wind.lon)) } \
            for la in range(len(self.wind.lat)) }

    def interpolate( self, in_lat, in_lon ):
        '''Interpolation method of wind at specified position.
        Uses bilinear aproximation between the vectors.
        Proportion is linear scaling of angle and linear scaling of length
        '''
        in_lat = norm_lat(in_lat)
        in_lon = norm_lon(in_lon)
        
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
        RB = self.wind_grid[bottom][norm_lon(right)]
        RT = self.wind_grid[top][norm_lon(right)]

        top_vec = self.proport( LT, RT, (in_lon - left) / (right - left) )
        bot_vec = self.proport( LB, RB, (in_lon - left) / (right - left) )

        return self.proport( bot_vec, top_vec, \
                             (in_lat - bottom) / (top - bottom) )

    def proport( self, vec1, vec2, t ):
        '''Prorotion between two vectors is linear scaling of azimuth and
        linear scaling of length
        '''
        f1 = math.atan2( vec1[1], vec1[0] )
        f2 = math.atan2( vec2[1], vec2[0] )
        r1 = math.sqrt( vec1[0]*vec1[0] + vec1[1]*vec1[1] )
        r2 = math.sqrt( vec2[0]*vec2[0] + vec2[1]*vec2[1] )

        f = f1 + norm_delta_rad( f2 - f1 ) * t
        r = r1 + (r2 - r1) * t
            
        return ( r * math.cos(f), r * math.sin(f) )

##    def norm_lat( self, lat ):
##        '''Thechnical function for normalizing latitude value'''
##        while lat < -180:
##            lat += 90
##        while lat > 180:
##            lat -= 90
##        return lat
##
##    def norm_lon( self, lon ):
##        '''Technical function for normalizing longitude value'''
##        while lon < 0:
##            lon += 360
##        while lon >= 360:
##            lon -= 360
##        return lon
##
##    def norm_ang_diff( self, diff ):
##        '''Technical function for normalizing angle differance in radians'''
##        while diff > math.pi:
##            diff -= ( math.pi*2 )
##        while diff < -math.pi:
##            diff += ( math.pi*2 )
##        return diff

#----------------------------- for comparation ---------------------------
    def proport_simple( self, vec1, vec2, t ):
        '''Prorotion between two vectors is linear scaling of both
        vector components
        '''
        X = vec1[0]+ (vec2[0]-vec1[0])*t
        Y = vec1[1]+ (vec2[1]-vec1[1])*t
        return (X, Y)

    def interpolate_simple( self, in_lat, in_lon ):
        '''Simple interpolation method of wind at specified position.
        Uses bilinear aproximation between the vectors.
        Proportion is linear scaling of vector components
        '''
        in_lat = norm_lat(in_lat)
        in_lon = norm_lon(in_lon)
        
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
        RB = self.wind_grid[bottom][norm_lon(right)]
        RT = self.wind_grid[top][norm_lon(right)]

        top_vec = self.proport_simple( LT, RT, (in_lon - left)/(right - left) )
        bot_vec = self.proport_simple( LB, RB, (in_lon - left)/(right - left) )

        return self.proport_simple( bot_vec, top_vec, \
                             (in_lat - bottom) / (top - bottom) )

#------------------------------ slerp ----------------------------------
    def slerp( self, vec1, vec2, t ):
        '''Prorotion between two vectors is 'slerp'
        '''
        r1 = math.sqrt(vec1[0]*vec1[0] + vec1[1]*vec1[1])
        r2 = math.sqrt(vec2[0]*vec2[0] + vec2[1]*vec2[1])
        cosO = (vec1[0]*vec2[0] + vec1[1]*vec2[1])/(r1*r2)
        if cosO > 0.99:
            return ( vec1[0]*(1 - t) + vec2[0]*t, vec1[1]*(1 - t) + vec2[1]*t )
        else:
            omega = math.acos( cosO )
            
            sinO = math.sqrt( 1 - cosO*cosO )
            sinT = math.sin( t*omega )
            sin1T = math.sin( (1 - t)*omega )
            
            return ( (vec1[0]*sin1T + vec2[0]*sinT)/sinO, \
                     (vec1[1]*sin1T + vec2[1]*sinT)/sinO )

    def interpolate_slerp( self, in_lat, in_lon ):
        '''Slerp interpolation method of wind at specified position.
        Uses bilinear aproximation between the vectors.
        Proportion is 'slerp'
        '''
        in_lat = norm_lat(in_lat)
        in_lon = norm_lon(in_lon)
        
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
        RB = self.wind_grid[bottom][norm_lon(right)]
        RT = self.wind_grid[top][norm_lon(right)]

        top_vec = self.slerp( LT, RT, (in_lon - left) / (right - left) )
        bot_vec = self.slerp( LB, RB, (in_lon - left) / (right - left) )

        return self.slerp( bot_vec, top_vec, \
                             (in_lat - bottom) / (top - bottom) )  
#--------------------------------- naive ------------------------------------
    def interpolate_naive( self, in_lat, in_lon ):
        '''A bit stupid methon
        Resulting vector is proportional to how it`s close to \
        corner base grid vectors
        '''
        in_lat = norm_lat(in_lat)
        in_lon = norm_lon(in_lon)
        
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
        RB = self.wind_grid[bottom][norm_lon(right)]
        RT = self.wind_grid[top][norm_lon(right)]

        ltx = math.sqrt( \
            (in_lat-top)*(in_lat-top) + (in_lon-left)*(in_lon-left) )
        lbx = math.sqrt( \
            (in_lat-bottom)*(in_lat-bottom) + (in_lon-left)*(in_lon-left) )
        rtx = math.sqrt( \
            (in_lat-top)*(in_lat-top) + (in_lon-right)*(in_lon-right) )
        rbx = math.sqrt( \
            (in_lat-bottom)*(in_lat-bottom) + (in_lon-right)*(in_lon-right) )
        norma = ltx + lbx + rtx + rbx

        return ( ((norma-ltx)*LT[0]/norma + (norma-lbx)*LB[0]/norma + \
                 (norma-rtx)*RT[0]/norma + (norma-rbx)*RB[0]/norma)/3 , \
                 ((norma-ltx)*LT[1]/norma + (norma-lbx)*LB[1]/norma + \
                 (norma-rtx)*RT[1]/norma + (norma-rbx)*RB[1]/norma)/3 )
