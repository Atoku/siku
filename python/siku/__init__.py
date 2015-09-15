'''Siku: Sea Ice Discrete Element Method Model

      The main python module that has to be imported from any
      scenario file

'''
try:
    from . import bootstrap_config
    from . import earth
except:
    import bootstrap_config
    import earth
#import wnd

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
# planet default parameters
# ---------------------------------------------------------------------

planet = earth.Earth()

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

callback.presave = presave

## ### moved to freedrift.py
##def pretimestep( t, n, ns):
##    status = MASK['NONE']
##    print('test')
##    # some specific checks should be placed.
##    if t > ( time.last + time.dt ):
##        status += MASK['WINDS']
##        time.last = t
##
##    return status
##
##callback.pretimestep = pretimestep

def updatewind( siku, t ):
    for i in range(len(uw.times)):
        if t < uw.times[i] and i > 0:
            i -=1
            break

    #this print is synchronized with siku.cc print
    print( str( uw.times[i] ) + '\n' )
    
    time.last_update = uw.times[i]
    time.update_index = i
    siku.wind = wnd.NMCSurfaceVField( uw, vw, i )
    pass

callback.updatewind = updatewind

# ---------------------------------------------------------------------
# Diagnostics
# ---------------------------------------------------------------------

class Diagnostics:
    pass

diagnostics = Diagnostics()

# registered meshes: to use in monitor functions
diagnostics.meshes = []

# wind monitoring is a list of tuples ( func_name, grid ). This
# functions will be called with the grids values
diagnostics.wind = []

# ---------------------------------------------------------------------
# Surface wind grid (NMC)
# ---------------------------------------------------------------------

wind = None


