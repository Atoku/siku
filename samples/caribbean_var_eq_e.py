'''Siku scenario

   Sea ice element simple free drift example

   Creates a (large) polygon in polar region and sets some basic winds
   in still water. No interaction with any boundaries, just a free
   float of the polygon in the area and output in KML format of its
   locations.

   Be sure that siku module is in your PYTHONPATH.

   Use python3 for checking. It is not compatible with python2.x

   (c)2014 UAF, written by Anton Kulchitsky
   GPLv3 or later license (same as siku)

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

from   siku import wnd
from   siku import noisy_grid as NG
from   siku import color_works as cworks
 
def main():

    # ---------------------------------------------------------------------
    # Define material
    # ---------------------------------------------------------------------

    ice = material.Material()       # default ice values, 10 thicknesses
    ice.name = 'ice'                # prefer to use our own name instead
                                    # of default
    
    siku.materials.append( ice )    # list of all materials

    # table of material names for convenience
    matnames = {
        'ice': 0,
    }

    # ---------------------------------------------------------------------
    #  Wind initializations (NMC grid example)
    # ---------------------------------------------------------------------

    #start time index
    st_t_ind = 2
    
    siku.uw = wnd.NMCVar( 'u2014.nc', 'uwnd' )
    siku.vw = wnd.NMCVar( 'v2014.nc', 'vwnd' )
    siku.wind = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )

######    siku.settings.wind_source_type = siku.WIND_SOURCES['NONE']
    siku.settings.wind_source_type = siku.WIND_SOURCES['NMC']
    #siku.settings.wind_source_type = siku.WIND_SOURCES['TEST']
    siku.settings.wind_source_names = [ 'u2014.nc', 'v2014.nc' ]
    w = wnd.NMCSurfaceVField( siku.uw, siku.vw, st_t_ind )
    w.make_test_field( -0.0, 0.0 )
    siku.wind = w

    # ---------------------------------------------------------------------
    # date/time settings
    # ---------------------------------------------------------------------
    
    hour = datetime.timedelta ( hours = 1 )

    #siku.time.start    = datetime.datetime  ( 2012, 3, 12, 00, 00, 00 )
    siku.time.start = siku.uw.times[st_t_ind]
    siku.time.finish   = siku.time.start + hour * 120#630#315#210#105#120

    siku.time.dt       = ( siku.time.finish - siku.time.start ) / 2400#7200#3600#2400 #60
    siku.time.dts      = datetime.timedelta ( seconds = 600 )
    siku.time.last = siku.time.start
    siku.time.last_update = siku.time.last
       
    # ---------------------------------------------------------------------
    # elements
    # ---------------------------------------------------------------------
    
    coords = []
    siku.elements = []
    n_filled = 0
## custom testing polygons for caribbeans # lon, lat convention

    x = 5
    y = 2

    NN = [ 1, 2, 4, 8 ]
##    NN = [ 2, 4, 8, 16 ]
##    NN = [ 4, 8, 16, 32 ]

    nx = [ x*i for i in NN ]
    ny = [ y*i for i in NN ]

    left_inds = []
    right_inds = []
    left_gi = {}
    right_gi = {}
    
    ###test sections
    for i in range( len( NN ) ):
        cds, links = NG.generate_plus( \
            252.0+i*20.0, -2.0, 262.0+i*20.0, 2.0, nx[i], ny[i], 0., 0. )

        coords = coords + cds
##        siku.settings.links = siku.settings.links + links

##        left_inds = left_inds + \
##                     [ n_filled+j*nx[i] for j in range(ny[i]) ]
##        right_inds = right_inds + \
##                     [ n_filled+j*nx[i]+nx[i]-1 for j in range(0, ny[i]) ]
##
        n_filled += nx[i]*ny[i]

    ###handles
    for i in range( len( NN ) ):
        cds, links = NG.generate_plus( \
            252.0+i*20.0-10.0/nx[i], -2.0, 252.0+i*20.0, 2.0, \
            1, ny[i], 0., 0. )
##            275.0-10.0/nx[i], 9.0, 275.0, 13.0, 1, ny[i], \
##            0., 0. )
        coords = coords + cds
        t = [ n_filled+j for j in range(ny[i]) ]
        left_inds = left_inds + t
        for j in t:
            left_gi[j] = i
        n_filled += ny[i]

        cds, links = NG.generate_plus( \
            262.0+i*20.0, -2.0, 262.0+i*20.0+10.0/nx[i], 2.0, \
            1, ny[i], 0., 0. )            
##            285.0, 9.0, 285.0+10.0/nx[i], 13.0, 1, ny[i], \
##            0., 0. )
        coords = coords + cds
        t = [ n_filled+j for j in range(ny[i]) ]
        right_inds = right_inds + t
        for j in t:
            right_gi[j] = i 
        n_filled += ny[i]

    siku.local.NN = NN
    siku.local.nx = nx
    siku.local.ny = ny
    siku.local.right_inds = right_inds
    siku.local.left_inds = left_inds
    siku.local.right_gi = right_gi
    siku.local.left_gi = left_gi

    # ---
    
    ### Initializing elements with polygon vertices
    for c in coords:
        siku.P.update( c )
     
        # Element declaration
        E = element.Element( polygon = siku.P, imat = matnames['ice'] )
        E.monitor = "drift_monitor"
        gh = [ 0.2, 0.2, 0.4, 0.2, 0.0, 
               0.0, 0.0, 0.0, 0.0, 0.0 ]
        E.set_gh( gh, ice )
        
        # all elements in the list
        siku.elements.append( E )

    # ---------------------------------------------------------------------
    #  Monitor function for the polygon
    # ---------------------------------------------------------------------

    ## Plotter initialization
    siku.plotter = GMT_Plotter( 'caribbean_plot_e.py' )

    ### period of picturing
    siku.diagnostics.monitor_period = 30#45#60
    siku.drift_monitor = drift_monitor
    siku.diagnostics.step_count = 0

##    siku.settings.force_model = \
##                    siku.CONTACT_FORCE_MODEL['Hopkins_Frankenstein']
    siku.settings.force_model = \
                    siku.CONTACT_FORCE_MODEL['distributed_spring']

    siku.settings.contact_method = siku.CONTACT_METHODS['sweep']
##    siku.settings.contact_method = siku.CONTACT_METHODS['n2']

##    siku.settings.phys_consts = [ 5000 , 10000000 , 0.75, -0.00003, 1, \
##                                  -10000.0, 1.00, 0.2, 0.1, \
##                                  0.01 ] #wind interaction adjuster

    siku.settings.phys_consts = { 'rigidity' : 1.0,
                                  'viscosity' : 1.0,
                                  'rotatability' : 1.0,#0.75
                                  'tangency' : -0.00003,#-0.00003
                                  
                                  'elasticity' : 500000.0,#5000000.0,
                                  'bendability' : 1.0,#1.0,
                                  'solidity' : 1.,#0.5,#0.05,
                                  'tensility' : 0.1,#10.30,#0.615,

                                  'anchority' : 0.0000001,
                                  'windage':    0.001,
                                  'fastency' : 0.250, #0.5

                                  'sigma' : 400000.,#1000000.0, #same as elasticity
                                  'etha' : 0.01#0.051    # -//- viscosity
                                  }

    # ------------------------- speed settings ----------------------------
 
    for i in left_inds:
        siku.elements[i].flag_state = element.Element.f_static

    
##    siku.settings.manual_inds = right_inds
##    amo = len(right_inds)
##    F = 800.0
##    siku.settings.manual_forces = [ (F/amo, -F/14/amo, 0.0) \
##                                    #-(i/nx)*1.0, -0.2*(i/ny/nx))
##                                    for i in right_inds ]

    for i in right_inds:
        siku.elements[i].velo = (-1.0, 0.0, 0.0)
##        siku.elements[i].velo = (-0.20, 0.0, 0.0)
        siku.elements[i].flag_state = element.Element.f_steady

    # ---------------------------------------------------------------------
    #  Callback flag-mask generator
    # ---------------------------------------------------------------------

    siku.callback.pretimestep = pretimestep
    siku.callback.aftertimestep = aftertimestep
    siku.callback.conclusions = conclusions
    siku.callback.initializations = initializations
    siku.callback.updatewind = updatewind
    siku.callback.global_monitor = global_monitor

    ##
    siku.callback.presave = presave

    return 0

def presave( t, n, ns ):
##    if n%siku.diagnostics.monitor_period ==0:
##        fname = 'siku-' + t.strftime("%Y-%m-%d-%H:%M:%S") + '.h5'
##        return fname
    return
# --------------------------------------------------------------------------

def global_monitor( t, n, ns, Sigma ):
    siku.sigma = Sigma
    
    siku.local.Smin = -0.5*(Sigma[1] + Sigma[3])
    siku.local.Smax = -0.5*(Sigma[0] + Sigma[2])
    
    return 0
# --------------------------------------------------------------------------

def initializations( siku, t ):
    subprocess.call(["gmtset", "PS_MEDIA=Custom_72cx17.5c"]) #24_20

    siku.local.sigmaMax = 0.
    siku.local.sigmaMin = 0.

    siku.local.sMax = 0.5284816499797322
    siku.local.sMin = -409396.54611198575
##    siku.local.sMax = 516386.46676500165
##    siku.local.sMin = -468575.8903352868
##    siku.local.sMax = 879877.3152675817
##    siku.local.sMin = -381127.9856318919

    siku.local.fces = []
    for i in range( len( siku.local.NN ) ):
        siku.local.fces.append( open( './forces/group' + str(i) + '.txt', 'w' ) )
        

# --------------------------------------------------------------------------

def conclusions( siku, t ):

    print('minimal and maximal sigma:\n' +
          str(siku.local.sigmaMin)+ ', '+str(siku.local.sigmaMax)+'\n' )
    
    print('creating .gif')
    subprocess.call( "convert -density 100 -delay 20 carib*.eps caribbeans.gif", \
                     shell=True )

    [ i.close() for i in siku.local.fces ]
##    subprocess.call( "gnuplot ./forces/gr.sh", shell=True )    

# --------------------------------------------------------------------------

def pretimestep( t, n, ns ):
    status = siku.MASK['NONE']
    siku.diagnostics.step_count = n

    siku.local.poly_f = open( 'Polygons.txt', 'w' )

    # primitive time stepping
##    if t > ( siku.time.last + siku.time.dt ):
##        status += siku.MASK['WINDS']
##        siku.time.last = t

    # step by NMC own time step
    if t >= siku.uw.times[siku.time.update_index + 1]: #siku.time.last: #
        status += siku.MASK['WINDS']
        siku.time.last = t #siku.time.finish#

    #accumulated force vectr and stress
    siku.local.FF = [ mathutils.Vector( (0.0,0.0,0.0) )\
                      for i in siku.local.NN ]
    siku.local.ff = [ 0 for i in siku.local.NN ]

    return status

# --------------------------------------------------------------------------

def updatewind( siku, t ):
##    siku.time.update_index += 1
##    siku.time.last_update = t
##    siku.wind = \
##              wnd.NMCSurfaceVField( siku.uw, siku.vw, siku.time.update_index )
    pass

# --------------------------------------------------------------------------

def aftertimestep( t, n, ns ):
    siku.local.poly_f.close()
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        pic_name = 'carib%03d.eps' % \
            (siku.diagnostics.step_count / siku.diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        
        siku.plotter.plot( pic_name, siku.time.update_index, siku.wind )

    for i in range( len( siku.local.FF ) ):
        f = siku.local.FF[ i ]
        s = siku.local.ff[ i ]
        t = f.length
        siku.local.fces[i].write( \
            str(n) + '\t' + str(s) + '\t' + str(t) + '\t' + \
            str(f[0]) + '\t' + str(f[1]) + '\t' + str(f[2]) + '\n' )

    #siku.local.poly_f.close()
    return 0

# --------------------------------------------------------------------------

##drift_monitor = siku.callback.default_monitor

def drift_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss, \
                   m, I, i, A, a_f, w_f ):
    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    s = -0.5*(ss[0] + ss[1])
    sxy = -0.5*(ss[2]+ss[3])
##    if ss[2] != -ss[3]:
####        print( abs(abs(ss[2])-abs(ss[3])) / (max(abs(ss[2]), abs(ss[3]))) )
##        print( abs(abs(ss[2])-abs(ss[3])) / (0.5*(abs(ss[2])+abs(ss[3]))) )
####        input(str(ss[2])+'  '+str(ss[3]))
##    s = -ss[3]  #@####
    if s < siku.local.sigmaMin: siku.local.sigmaMin = s
    if s > siku.local.sigmaMax: siku.local.sigmaMax = s

    #forces monitor
    if index in siku.local.right_gi:
        siku.local.FF[ siku.local.right_gi[ index ] ] += \
                       mathutils.Vector( F )
        siku.local.ff[ siku.local.right_gi[ index ] ] += \
                       mathutils.Vector( F ).length

    # appending vertices to plotting list
    if siku.diagnostics.step_count % siku.diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

##        col1 = (128, 255, 127) #light green
##        col2 = (128, 127, 255) #light blue
        col1 = (255, 128, 127) #light red
        col2 = (128, 127, 255) #light blue
        col = cworks.gmt_color_hsv_scale( col2, col1, s, 1e5 )

        poly = siku.local.poly_f
##        if st & element.Element.f_errored: ##
##            poly.write( '> -Gred -W0.1p,red \n' ) ##
                                
##        elif
        if st & element.Element.f_special: ## elif -> if
            poly.write( '> -Gpink -W0.4p,'+col+' \n' )
        elif st & 256: ## elif -> if
            poly.write( '> -Gpurple -W0.5p,'+col+' \n' )
        elif st & 512: ## elif -> if
            poly.write( '> -Gmagenta -W0.5p,'+col+' \n' ) 
        elif st & element.Element.f_static:
##            poly.write( '> -Gbrown -W0.1p,lightBlue \n' )
            poly.write( '> -G'+ col +' -W0.2p,black \n' )
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -G'+ col +' -W0.2p,white \n' )
##            poly.write( '> -GlightGreen -W0.1p,lightBlue \n' )
        else:
##            poly.write( '> -GlightCyan -W0.1p,lightBlue \n' )\
            poly.write( '> -G'+ col +' -W0.1p,'+ col +' \n' ) 
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return

# --------------------------------------------------------------------------

def winds_diag( t, winds ):

    mesh = siku.diagnostics.meshes[0]
    ez = mathutils.Vector( (0,0,1) )

    siku.diagnostics.wind_counter += 1

    return

# ---------------------------------------------------------------------
# Calling main function at the end
# ---------------------------------------------------------------------

siku.main = main()

if __name__ == '__main__':
    sys.exit( siku.main )
