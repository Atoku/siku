'''File: color_works.py
Module: color_works.

Provides several functions for easy color interpolations and parametrizations
in order to generate nice output graphics. Made for working with GMT.

'''

import math
from math import sin, cos, tan, atan, pi, sqrt, tanh, log, copysign

rad2deg = 180. / pi
deg2rad = pi / 180.

def param_scale( t, scale = 1e5 ):
    '''Transform input paramenter from [-inf, inf] range to [0, 1]
    by specific function (atan by default) and characteristic scale
    (10^5 by default) which is the value of argument at 0.5 of max amplitude.
    '''
    
    ##t = 0.5 + 0.5 * log(abs(t)/scale+1.)*copysign(1, t)/log(5.)
    t = 0.5 + math.atan( t/scale ) / pi #/ 2.8
    
    t = max( t, 0. )
    t = min( t , 1. )
    
    return t

def gmt_color_lerp( col1, col2, t ):
    '''Interpolates color from col1 to col2 (RGB-tuples) by parameter t,
    
    Return: string 'R/G/B', where R,G,B - integers from 0 to 255
    '''
    t = max( t, 0. )
    t = min( t , 1. )
    
    col = ( int( col1[0] + t*(col2[0] - col1[0]) ), \
            int( col1[1] + t*(col2[1] - col1[1]) ), \
            int( col1[2] + t*(col2[2] - col1[2]) ) )

    return str(col[0])+'/'+str(col[1])+'/'+str(col[2])

def gmt_color_lerp_scale( col1, col2, t, scale = 1e5 ):
    '''Interpolates color from col1 to col2 (RGB-tuples) by parameter t
    with internal smooth scaling by additional parameter 'scale'
    
    Return: string 'R/G/B', where R,G,B - integers from 0 to 255
    '''
    return gmt_color_lerp( col1, col2, param_scale( t, scale ) )

def rgb_to_hsv( col ):
    '''Convert color in RGB encoding into HSV (H in radians)
    http://www.rapidtables.com/convert/color/rgb-to-hsv.htm
    '''
    r = col[0]/255.
    g = col[1]/255.
    b = col[2]/255.

    Cmax = max(r, g, b)
    Cmin = min(r, g, b)
    d = Cmax - Cmin
    
    H = 0
    if( d == 0 ):
        pass
    elif( Cmax == r ):
        H = pi/3. * ( (g - b)/d )%6
    elif( Cmax == g ):
        H = pi/3. * ( (b - r)/d + 2. )
    elif( Cmax == b ):
        H = pi/3. * ( (r - g)/d + 4. )

    S = 0. if Cmax == 0. else d/Cmax

    V = Cmax
    
    return ( H, S, V )

def hsv_to_rgb( col ):
    '''Convert SHV triplet to RGB (considering H in radians and >0)
    http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
    '''
    H = col[0]
    S = col[1]
    V = col[2]

    while( H < 0. ): H += 2.*pi
    while( H >= 2.*pi ): H -= 2*pi
    
    p3 = pi/3. #aka 60 degrees
    C = S * V
    X = C * (1. - abs( (H/p3)%2 - 1. ))
    m = V - C

    if( H < p3 ):
        c = (C, X, 0,)
    elif( H < 2.*p3 ):
        c = (X, C, 0.)
    elif( H < 3.*p3 ):
        c = (0., C, X)
    elif( H < 4.*p3 ):
        c = (0., X, C)
    elif( H < 5.*p3 ):
        c = (X, 0., C)
    else:
        c = (C, 0., X)

    return ( int((c[0]+m)*255.), int((c[1]+m)*255.), int((c[2]+m)*255.) )
    
def gmt_color_hsv( col1, col2, t ):
    '''Smoothly but slowly interpolate color with parameter t by
    temporally converting RGB to SHV.

    Return: string 'R/G/B', where R,G,B - integers from 0 to 255
    '''
    t = max( t, 0. )
    t = min( t , 1. )

    C1 = rgb_to_hsv( col1 )
    C2 = rgb_to_hsv( col2 )
    
    C = ( C1[0] + t*(C2[0] - C1[0]), \
          C1[1] + t*(C2[1] - C1[1]), \
          C1[2] + t*(C2[2] - C1[2]) )

    col = hsv_to_rgb( C )

    return str(col[0])+'/'+str(col[1])+'/'+str(col[2])

def gmt_color_hsv_scale( col1, col2, t, scale = 1e5 ):
    '''Smoothly but slowly interpolate color with parameter t by
    with internal smooth scaling by additional parameter 'scale'
    and temporally converting RGB to SHV.

    Return: string 'R/G/B', where R,G,B - integers from 0 to 255
    '''
    return gmt_color_hsv( col1, col2, param_scale( t, scale ) )
