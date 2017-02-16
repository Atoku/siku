'''Siku: Sea Ice Discrete Element Method Model

      The main python module that has to be imported from any
      scenario file

'''

import datetime
import mathutils

try:
    from . import bootstrap_config
    from . import earth
    from . import polygon
    from . import color_works as cworks
except:
    import bootstrap_config
    import earth
    import polygon

# ---------------------------------------------------------------------
# return status masks
# ---------------------------------------------------------------------

MASK = {
    'NONE' : 0,
    'SAVE' : 1,
    'WINDS' : 2,
    'CURRENTS' : 4,

    'EXIT' : 128
    }

# ---------------------------------------------------------------------
# misc enums
# ---------------------------------------------------------------------

CONTACT_METHODS = {
    'n2' : 0,
    'sweep' : 1
    }

CONTACT_DET_FREQ_MET = {
    'always' : 0,
    'ticks' : 1, 'tick' : 1,
    'sec' : 2, 'seconds' : 2,
    'speed' : 3, 'auto' : 3
    }

CONTACT_FORCE_MODEL = {
    'test_springs' : 0,
    'Hopkins' : 1, 'hopkins' : 1,
    'default' : 2, 'distributed_spring' : 2
    }

GRID_SOURCES = {
    'NONE' : 0,
    'TEST' : 1,
    'NMC' : 2
    }

# ---------------------------------------------------------------------
# main function: by default it is None
# ---------------------------------------------------------------------

main = None

# ---------------------------------------------------------------------
# info dictionary
# ---------------------------------------------------------------------

info = {
    'name' : bootstrap_config.NAME,
    'brief': bootstrap_config.BRIEF,
    'version': bootstrap_config.VERSION,
    'date':    bootstrap_config.DATE,
    'version numbers': [ int(x) for x in 
                         bootstrap_config.VERSION.split( '.' ) ],
    'main program name': bootstrap_config.MAINPROGRAM,
    'email':     bootstrap_config.EMAIL,
    'author':    bootstrap_config.AUTHOR,
    'copyright': bootstrap_config.COPYRIGHT }

# ---------------------------------------------------------------------
# miscellanous parameters
# ---------------------------------------------------------------------

class Settings:
    pass

settings = Settings()

settings.picname_base = 'default_' # default picture name beginning

# contact detection default method
settings.contact_method = CONTACT_METHODS['sweep']
settings.contact_freq_met = CONTACT_DET_FREQ_MET['always']
settings.contact_value = 1

settings.force_model = CONTACT_FORCE_MODEL['default']

settings.wind_source_type = GRID_SOURCES['TEST']
settings.wind_source_names = []
settings.water_source_type = GRID_SOURCES['NONE']
settings.water_source_names = []

settings.loadfile = ''

settings.borders = ''   # filename with points to mark border elements,
                        # lon-lat convension, name like 'borders.ll'
settings.border_mark = 0

## commented supposed to be outdated
settings.phys_consts = {
                        'rigidity' : 1.0,  #'bouncing' on impact
                        'viscosity' : 1.0,    #'sticking' on impact
                        'rotatability' : 1.0, #part of Force applied to rotation
                        'tangency' : 1.0,     #part of Force applied to sliding

##                        'elasticity' : 1.0,   #hardness of spring in joints
##                        'bendability' : 1.0,  #rotation intensity
                        'solidity' : 1.0,     #destruction intensity
                        'tensility' : 0.1,    #extension-without-damage cap

                        'windage' : 0.001,    #part of wind applied to force
                        'anchority' : 0.05,   #generic viscosity of water
                        'fastency' : 0.25,    #floe overlap with landfast floe
                                              #to become landfast itself

##                        'sigma' : 400000.0,   # -//- rigidity
                        'etha' : 0.001,       # -//- viscosity

                        'sigma_t' : 35e4,    #Hopkins physics method paramentes:
                        'sigma_c' : 1.285e6, #three extreme values for stress
                        'sigma_s' : 35e4,      #diagrams
                        'tan_mu' : 1.,       #and an ice friction angle

                        'hop_visc' : 0.001,        #viscosity
                        'dest_threshold' : 0.2, #desctruction threshold
                        
                        'n_integ_segments' : 10 #number of integration segments
                        }

settings.manual_inds = []
settings.manual_forces = []

settings.initial_freeze = 1
settings.links = []

planet = earth.Earth()

P = polygon.Polygon()    # need to be done only once for all polygons,
                         # elements will be initialized using polygons

elements = []

class Local:
    pass

local = Local()

# ---------------------------------------------------------------------
# ModelTime class for setting model time and such
# ---------------------------------------------------------------------

class ModelTime:
    pass

time = ModelTime()
time.update_index = 0

# ---------------------------------------------------------------------
# material list
# ---------------------------------------------------------------------

materials = []                  # must be filled in the actual list

# ---------------------------------------------------------------------
# Callback functions
# ---------------------------------------------------------------------

class Callback:
    pass

callback = Callback()

def presave( t, n, ns ):
    'No saving - no name'
    return

def pretimestep( t, n, ns):
    status = MASK['NONE']
    diagnostics.step_count = n

    print("\r Step: " + str(n) + "\t", end = "", flush=True)
          
    local.poly_f = open( 'Polygons.txt', 'w' )

    return status

def global_monitor( t, n, ns, Sigma ):
    return 0

def updatewind( siku, t ):
    print("Your advertisement could be here")
    pass

def updatewater( siku, t ):
    print("Yet no water")
    pass

def aftertimestep( t, n, ns ):
    
    local.poly_f.close()
    
    if diagnostics.step_count % diagnostics.monitor_period == 0:
        pic_name = settings.picname_base + '%03d.eps' % \
            (diagnostics.step_count / diagnostics.monitor_period)
        print('drawing ' + str( pic_name ) )
        plotter.plot( pic_name )
        
    return 0

def initializations( siku, t ):
    pass

def conclusions( siku, t ):
    
    print('Good buy!')
    pass

def default_monitor( t, n, Q, Ps, st, index, ID, W, F, N, ss, \
                     m, I, i, A, a_f, w_f ):

    # create actual quaternion
    q = mathutils.Quaternion( Q )
    C = mathutils.Vector( (0,0,1) )

    # get latitude and longitude of center of mass (0,0,1)
    R = q.to_matrix()
    c = R * C

    # appending vertices to plotting list
    if diagnostics.step_count % diagnostics.monitor_period == 0:
        Pglob = [ R*mathutils.Vector( p ) for p in Ps ]
        vert = [ geocoords.lonlat_deg(mathutils.Vector( p ) ) for p in Pglob ]

        poly = local.poly_f
        if st & element.Element.f_special:
            poly.write( '> -Gpink -W0.4p,lightBlue \n' ) 
        elif st & element.Element.f_static:
            poly.write( '> -Gbrown -W0.2p,lightBlue \n' )
        elif st & element.Element.f_steady:
            poly.write( '> -Ggreen -W0.2p,lightBlue \n' )
        else:
            poly.write( '> -Gwhite -W0.05p,lightBlue \n' ) 
            
        for v in vert:
            poly.write( str( geocoords.norm_lon(v[0]) )+'\t'+ \
                        str( v[1] )+'\n' )

    return

callback.default_monitor = default_monitor
drift_monitor = default_monitor
callback.presave = presave
callback.pretimestep = pretimestep
callback.global_monitor = global_monitor
callback.updatewind = updatewind
callback.updatewater = updatewater
callback.aftertimestep = aftertimestep
callback.conclusions = conclusions
callback.initializations = initializations

# ---------------------------------------------------------------------
# Diagnostics
# ---------------------------------------------------------------------

class Diagnostics:
    pass

diagnostics = Diagnostics()
diagnostics.step_count = 0
diagnostics.monitor_period = 1

# registered meshes: to use in monitor functions
diagnostics.meshes = []

# wind monitoring is a list of tuples ( func_name, grid ). This
# functions will be called with the grids values
diagnostics.wind = []

# ---------------------------------------------------------------------
# Utils
# ---------------------------------------------------------------------

class Utils:
    pass

utils = Utils()

# ---------------------------------------------------------------------
# Surface wind grid (NMC)
# ---------------------------------------------------------------------

wind = None


