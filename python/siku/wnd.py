'''file: wnd.py
module: wnd

Contains classes for wind grid storaging and methods for translation wind
speed data into Cartesian coordinates array.
'''
try:
    from siku import geocoords
    from siku import nmc
except ImportError:
    import nmc
    import geocoords

NMC = nmc.NMC

#------------------------------------------------------------------------------
# VARIABLE STORAGING (IN LATITUDE-LONGITUDE COORDINATES)
#------------------------------------------------------------------------------
        
class NMCVar(NMC):
    '''A class for storaging NMC specified variable data
    '''
    def __init__( self, filename = None, var_name = ''):
        '''Inits the class instance
        '''
        self.reinit_()
        if filename:
            self.open( filename, var_name)
        return        
    
    def open( self, filename, var_name='' ):
        '''Opens file and reads grid and time information
        '''
        NMC.open( self, filename )
        if var_name:
            self.read_var_( var_name )

        return

    def is_header_match( self, nmc ):
        '''Checks if the header of another NMC object matches the current one
        '''
        return ( NMC.is_header_match( nmc ) and \
                 (self.val == nmc.val).all() )

    def reinit_( self ):
        '''Reinits tha class instance
        '''
        NMC.reinit_( self )
        self.val = []
        return

    def read_var_( self, var_name ):
        '''Reads variable data from file
        '''
        self.val = self.f1.variables[var_name][:][:][:]
        return

    pass


#------------------------------------------------------------------------------
# SURFACE VECTOR FIELDS (WITH CARTESIAN INTERPRATATION)
#------------------------------------------------------------------------------

class NMCSurfaceVField:
    '''A class for storaging and processing NMC surface vector fields.

    Loads vector field from two NMCVar instances, that are considered to be
    'east' and 'north' components of resulting vector.

    Basically made for NMC wind data.
    
    Methods for transforming surface data into cartesian coords are included.
    '''
    def __init__( self, ucomp=None, vcomp=None, time=None):
        '''Inits the class instance
        '''
        self.clear_()

        #in case of immidiate initialization
        if ucomp and vcomp:
            self.load( ucomp, vcomp, time )
            #self.make_cartesian_( uvind, vwind, time )

        return
    
    def load( self, ucomp, vcomp, time=None ):
        '''Loads 1 timestep data from two wind variable class instances
        '''
        if not time: #default timestep is 'last'
            time = -1

        self.lat = list(ucomp.lat)
        self.lon = list(ucomp.lon)
        self.time = time
        self.load_vec( ucomp, vcomp, time )

        return

    def clear_( self ):
        '''Clears inner data
        '''
        self.time = None
        self.lat = []
        self.lon = []
        self.vec = [[]]
        self.cart_vec = []
        self.cart_coords = []
        return

    def load_vec( self, ucomp, vcomp, time ):
        '''Loads only wind data from two wind variable class instances
        '''
        self.vec = [[(ucomp.val[time][la][lo], vcomp.val[time][la][lo]) \
                for lo in range(len(self.lon))] for la in range(len(self.lat))]

        return

    def make_cartesian_( self ):
        '''Generetes inner data in Cartesian coords

        creates two 1D arrays for (uwnd, vwnd) and (x, y, z) tuples
        '''
        if not self.vec:
            raise RuntimeError('no wind data assigned')
        self.cart_coords = []

        for la in range(len(self.lat)):
            for lo in range(len(self.lon)):
                self.cart_coords.append( \
                    geocoords.xyz_geographic(self.lon[lo],self.lat[la]))
                self.cart_vec.append(self.vec[la][lo])
        
        return
    
    def grid_save_( self, filename ):
        '''saves grided data into file for further plotting
        '''
        with open(filename, mode='w') as output:
            for la in range(len(self.lat)):
                for lo in range(len(self.lon)):
                    output.write(str(self.lon[lo]) + ' ' + \
                                 str(self.lat[la]) + ' ' + \
                                 str(self.vec[la][lo][0]) + ' ' + \
                                 str(self.vec[la][lo][1]) + ' ' + \
                                 ' 0 0 0 \n')
        return
    pass
#------------------------------------------------------------------------------
#   MAIN
#------------------------------------------------------------------------------
def main():
    #if __name__ == '__main__':
    uw = NMCVar('u2014.nc', 'uwnd')
    vw = NMCVar('v2014.nc', 'vwnd')

    carw = NMCSurfaceVField ( uw, vw )

    carw.make_cartesian_()
    #print(NMCVar)
    #carw.grid_save_('test.txt')

if __name__ == '__main__':
    main()
