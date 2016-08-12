'''Siku: Sea Ice Discrete Element Method Model

      The main python module that has to be imported from any
      scenario file

'''

import datetime

try:
    from . import bootstrap_config
    from . import earth
    from . import polygon
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
    'default' : 0, 'test_springs' : 0,
    'Hopkins_Frankenstein' : 1,
    'distributed_spring' : 2
    }

WIND_SOURCES = {
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

# contact detection default method
settings.contact_method = CONTACT_METHODS['sweep']
settings.contact_freq_met = CONTACT_DET_FREQ_MET['always']
settings.contact_value = 1

settings.force_model = CONTACT_FORCE_MODEL['default']

settings.wind_source_type = WIND_SOURCES['TEST']
settings.wind_source_names = []

settings.loadfile = ''

settings.borders = ''   # filename with points to mark border elements,
                        # lon-lat convension, name like 'borders.ll'
settings.border_mark = 0

##settings.phys_consts = [ 1, 1, 1, 1, 1,\
##                         1, 1, 1, 1, 1 ] # yet senseless
### YET IS FOR TEST
settings.phys_consts = { 'rigidity' : 1.0,  #'bouncing' on impact
                      'viscosity' : 1.0,    #'sticking' on impact
                      'rotatability' : 1.0, #part of Force applied to rotation
                      'tangency' : 1.0,     #part of Force applied to sliding
                      
                      'elasticity' : 1.0,   #hardness of spring in joints
                      'bendability' : 1.0,  #prt f sprng frc ap to rotation
                      'solidity' : 1.0,     #part of extension ap to damage
                      'tensility' : 1.0,    #extension-without-damage cap

                      'windage' : 1.0,      #part of wind applied to force
                      'anchority' : 1.0,    #generic viscosity of water
                      'fastency' : 0.8,     #floe overlap with landfast floe
                                            #to become landfast itself

                      'sigma' : 1.0,        # -//- rigidity
                      'etha' : 1.0          # -//- viscosity
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
    fname = 'siku-' + t.strftime("%Y-%m-%d-%H:%M:%S") + '.h5'
    return fname

def pretimestep( t, n, ns):
    status = MASK['NONE']
    diagnostics.step_count = n
    print("Step " + str( diagnostics.step_count ) + " has started")

    return status

def global_monitor( t, n, ns, Sigma ):
    sigma = Sigma

    local.Smin = -0.5*(Sigma[1] + Sigma[3])
    local.Smax = -0.5*(Sigma[0] + Sigma[2])
    
    return 0

def updatewind( siku, t ):
    print("Your advertisement could be here")
    pass

def aftertimestep( t, n, ns ):
    print("Step " + str( diagnostics.step_count ) + " has ended")
    return 0

def initializations( siku, t ):
    local.sigmaMax = 0
    local.sigmaMin = 0
    print('Hello earth!')

def conclusions( siku, t ):
    print('Good buy!')

callback.presave = presave
callback.pretimestep = pretimestep
callback.global_monitor = global_monitor
callback.updatewind = updatewind
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

def gmt_color_int( col1, col2, t ):
    '''Interpolates color from col1 to col2 (RGB-tuples) by parameter t,
    
    Return: string 'R/G/B', where R,G,B - integers from 0 to 255
    '''
    t = max( t, 0. )
    t = min( t , 1. )
    
    col = ( int( col1[0] + t*(col2[0] - col1[0]) ), \
            int( col1[1] + t*(col2[1] - col1[1]) ), \
            int( col1[2] + t*(col2[2] - col1[2]) ) )

    return str(col[0])+'/'+str(col[1])+'/'+str(col[2])

utils.gmt_color_int = gmt_color_int

# ---------------------------------------------------------------------
# Surface wind grid (NMC)
# ---------------------------------------------------------------------

wind = None


