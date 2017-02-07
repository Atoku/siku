'''
The fullest example. Demonstrates basic way to run the modelling process.
'''

import subprocess
import os
import math
import sys
import datetime
import mathutils
import numpy

import siku
from   siku import polygon
from   siku import element
from   siku import material
from   siku import geocoords
from   siku import regrid
from   siku import gmt_Plotter
GMT_Plotter = gmt_Plotter.GMT_Plotter
from   siku import poly_voronoi
PolyVor = poly_voronoi.PolyVor
from   siku import h5load
hload = h5load.Loader

from   siku import wnd
from   siku import color_works as cworks
from   siku import ice_nc

# ---------------------------------------------------------------------

import standard_callbacks as scb
scb.siku = siku
 
def main():

    # ---------------------------------------------------------------------
    # Define material
    # ---------------------------------------------------------------------

    ice = material.Material()       # default ice values, 10 thicknesses
    ice.name = 'ice'                # prefer to use our own name instead
                                    # of default
    ice.E = 4e5
    
    siku.materials.append( ice )    # list of all materials

    # table of material names for convenience
    matnames = {
        'ice': 0,
    }
    
    # ---------------------------------------------------------------------
    #  Wind and current initializations (NMC grid example)
    # ---------------------------------------------------------------------
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )

    ### approximate start time
    start =  datetime.datetime  ( 2014, 2, 2, 10, 10, 30 )
    ### find closest time point in .nc file
    for i in range(len( siku.uw.times )):
        if siku.uw.times[i] >= start:
            break
    st_t_ind = i
    siku.time.update_index = i - 1
    print( 'start time: ' + str( start ) + ' at position: ' + str( i ) + \
           ' of ' + str( len( siku.uw.times ) ) + '\n\n' )
    
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

    siku.settings.wind_source_type = siku.GRID_SOURCES['NMC']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]

#### Uncomment this to fill wind grid with test vectors (east-north components
#### in make_test_field)
##    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
##    w.make_test_field( 0.,0. )
##    siku.wind = w
   
    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------

    hour = datetime.timedelta ( minutes = 60 )

    ## time inits by NMC grid times
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.last_update = siku.time.start
    siku.time.finish = siku.time.start + 24 * hour
    siku.time.dt = (siku.time.finish - siku.time.start) / 3601
    siku.time.dts      = 4 * hour

   
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []

    # ---------------------- voronoi initialization ------------------------

    print('\nLoading polygons')
    PV = PolyVor( 'shapes.voronoi.xyz', 'shapes.voronoi.xyzf' )        
    PV.filter_( 0, 360, 60, 90 )

    print('Deleting land polygons')
    PV.clear_the_land()

    coords = PV.coords

    gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
           0.0, 0.0, 0.0, 0.0, 0.0 ]
    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor" 
        E.set_gh( gh, ice )
        
        siku.elements.append( E )

    # --------------------------- applying mask ---------------------------

    ## using grid data as some ice-state mask. In this case - setting
    ## some landfast ice as 'steady' 'special' elements.
    print('Applying masks')
    Mask = ice_nc.InterGrid('init_Beaufort.nc')
    Mask1 = ice_nc.InterGrid('init_Chukchi.nc')

    q2ll = geocoords.quat_to_ll

    eln = []
    for e in siku.elements:
        
        ll = q2ll( mathutils.Quaternion( e.q ) )
        v = Mask.get( ll[0], ll[1] )
        v1 = Mask1.get( ll[0], ll[1] )

        if v:
            v = abs(int(v))
        if v1:
            v1 = abs(int(v1))

        ## refine the values! int-uint conversions are plausible
        if (v == 255 or v == 128 or v == 1) or\
           (v1 == 255 or v1 == 128 or v1 == 1):
            e.flag_state = element.Element.f_special | \
                           element.Element.f_static
            
        if not ((v == 0 or v == 64) and (v1 == 0 or v1 == 64)):
            eln.append( e )
    siku.elements = eln
    print('Done')

    # ---------------------------------------------------------------------

    print('Marking borders with GMT')
    bor = PV.get_border_by_gmt()
    for b in bor:
        siku.elements[ b ].flag_state = element.Element.f_static
    print('Done\n\n')

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'complete_config.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 60
##    siku.drift_monitor = drift_monitor
    siku.drift_monitor = scb.stress_monitor
    siku.settings.picname_base = 'C_'

    siku.settings.phys_consts['windage'] = 0.001
    siku.settings.phys_consts['anchority'] = 0.01
    siku.settings.phys_consts['tensility'] = 0.5
    siku.settings.phys_consts['etha'] = 0.001    

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

##    siku.callback.pretimestep = pretimestep
    siku.callback.pretimestep = scb.grid_pretimestep
##    siku.callback.aftertimestep = aftertimestep
    siku.callback.aftertimestep = scb.basic_aftertimestep
##    siku.callback.conclusions = conclusions
    siku.callback.conclusions = scb.basic_conclusions
    siku.callback.initializations = initializations
##    siku.callback.updatewind = updatewind
    siku.callback.updatewind = scb.basic_updatewind
##    siku.callback.global_monitor = global_monitor

    siku.err_test = {}
    
    return 0

# --------------------------------------------------------------------------

##def global_monitor( t, n, ns, Sigma ):
##    siku.sigma = Sigma
##        
##    return 0

# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_17cx15c"])
    pass
    

# --------------------------------------------------------------------------

##def conclusions( siku, t ):
##        
##    print('creating .gif')
##    subprocess.call( "convert -density 300 -delay 10 _C*.eps C.gif", \
##                     shell=True )
##    pass

# --------------------------------------------------------------------------

##def pretimestep( t, n, ns ):
##    status = siku.MASK['NONE']
##    siku.diagnostics.step_count = n
##
##    print("\r Step: " + str(n) + "\t", end = "", flush=True)
##
##    siku.local.poly_f = open( 'Polygons.txt', 'w' )
##
##    # step by NMC own time step
##    if t >= siku.uw.times[siku.time.update_index + 1]:
##        status += siku.MASK['WINDS']
##
##    return status

# --------------------------------------------------------------------------

##def updatewind( siku, t ):
##    print( "updating winds: ", end = "" )
##    siku.time.update_index += 1
##    siku.time.last_update = t
##    siku.wind = \
##              wnd.NMCSurfaceVField( siku.uw, siku.vw, siku.time.update_index )
##    print( str( t ) )
##    pass

# --------------------------------------------------------------------------

##def aftertimestep( t, n, ns ):
##    siku.local.poly_f.close()
##    if siku.diagnostics.step_count == 0 or \
##       (siku.diagnostics.step_count+1) % siku.diagnostics.monitor_period == 0:
##        pic_name = '_C%03d.eps' % \
##            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
##        print('drawing ' + str( pic_name ) )
##
##        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )
##
##    return 0

# --------------------------------------------------------------------------

##def drift_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss,\
##                   m, I, i, A, a_f, w_f ):
##
##    # create actual quaternion
##    q = mathutils.Quaternion( Q )
##    C = mathutils.Vector( (0,0,1) )
##
##    # get latitude and longitude of center of mass (0,0,1)
##    R = q.to_matrix()
##    c = R * C
##
##    s = -0.5*(ss[0] + ss[1])
##
##    # appending vertices to plotting list
##    if siku.diagnostics.step_count == 0 or \
##       (siku.diagnostics.step_count+1) % siku.diagnostics.monitor_period == 0:
##
##        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
##        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]
##        poly = siku.local.poly_f
##
##        ## color interpolation to paint the elements according to applied
##        ## stress
##        col2 = (255, 128, 127) #red
##        col1 = ( 127, 128, 255) # blue
##        col = cworks.gmt_color_hsv_scale( col1, col2, s, 1e5 )
##        
##        if st & element.Element.f_special:       
##            poly.write( '> -G'+ col +' -W0.1p,purple \n' ) 
##        elif st & element.Element.f_static:
##            poly.write( '> -G'+ col +' -W0.1p,brown \n' )
##        elif st & element.Element.f_steady:
##            poly.write( '> -GlightGreen -W0.1p,lightBlue \n' )
##        else:
##            poly.write( '> -G'+ col +' -W0.1p,white \n' )        
##            
##        for v in vert:
##            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
##                        str( v[1] )+'\n' )
##
##    return

# --------------------------------------------------------------------------

##def winds_diag( t, winds ):
##    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
