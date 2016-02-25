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
    'Hopkins_Frankenstein' : 1
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
# miscellanous default parameters
# ---------------------------------------------------------------------

class Defaults:
    pass

defaults = Defaults()

# contact detection default method
defaults.contact_method = CONTACT_METHODS['sweep']
defaults.contact_freq_met = CONTACT_DET_FREQ_MET['always']
defaults.contact_value = 1

defaults.force_model = CONTACT_FORCE_MODEL['default']

defaults.wind_source_type = WIND_SOURCES['TEST']
defaults.wind_source_names = []

defaults.loadfile = ''

defaults.boarders = 'boarders.ll'
defaults.boarder_mark = 0

defaults.phys_consts = [ 1, 1, 1, 1, 1,\
                         1, 1, 1, 1, 1 ] # yet senseless

defaults.manual_inds = []
defaults.manual_forces = []

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

def updatewind( siku, t ):
    print("Your advertisement could be here")
    pass

def aftertimestep( t, n, ns ):
    print("Step " + str( diagnostics.step_count ) + " has ended")
    return 0

def initializations( siku, t ):
    print('Hello earth!')

def conclusions( siku, t ):
    print('Good buy!')

callback.presave = presave
callback.pretimestep = pretimestep
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
# Surface wind grid (NMC)
# ---------------------------------------------------------------------

wind = None


