'''File: wnd.py

Contains classes for wind grid storaging and methods for translation wind
speed data into Cartesian coordinates array.
'''

import geocoords
from nmc import NMC

#------------------------------------------------------------------------------
#   LATITUDE-LONGITUDE
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
# CARTESIAN
#------------------------------------------------------------------------------

class NMCWind:
    '''A class for storaging and processing NMC wind data

    Wind data is storaged as 2D lists:
    first dimension is time
    second dimension is an index of coords list

    Coords are stored as 1D list of [x, y, z] lists
    '''
    def __init__( self, uwind=None, vwind=None, time=None):
        '''Inits the class instance
        '''
        self.clear_()

        #in case of immidiate initialization
        if uwind and vwind:
            self.load( uwind, vwind, time )
            #self.make_cartesian_( uvind, vwind, time )

        return
    
    def load( self, uwind, vwind, time=None ):
        '''Loads 1 timestep data from two wind variable class instances
        '''
        if not time: #default timestep is 'last'
            time = len(uwind.times)-1

        self.lat = uwind.lat
        self.lon = uwind.lon
        self.time = time
        self.load_wind( uwind, vwind, time )

        return

    def clear_( self ):
        '''Clears inner data
        '''
        self.time = None
        self.lat = []
        self.lon = []
        self.wind = [[]]
        self.cart_wind = []
        self.cart_coords = []
        return

    def load_wind( self, uwind, vwind, time ):
        '''Loads only wind data from two wind variable class instances
        '''
        self.wind = [[(uwind.val[time][la][lo], vwind.val[time][la][lo]) \
                for lo in range(len(self.lon))] for la in range(len(self.lat))]

        return

    def make_cartesian_( self ):
        '''Generetes inner data in Cartesian coords

        creates two 1D arrays for (uwnd, vwnd) and (x, y, z) tuples
        '''
        if not self.wind:
            raise RuntimeError('no wind data assigned')
        self.cart_coords = []

        for la in range(len(self.lat)):
            for lo in range(len(self.lon)):
                self.cart_coords.append( \
                    geocoords.xyz_geographic(self.lon[lo],self.lat[la]))
                self.cart_wind.append(self.wind[la][lo])
        
        return
    
    def grid_save_( self, filename ):
        '''saves grided data into file for further plotting
        '''
        with open(filename, mode='w') as output:
            for la in range(len(self.lat)):
                for lo in range(len(self.lon)):
                    output.write(str(self.lon[lo]) + ' ' + \
                                 str(self.lat[la]) + ' ' + \
                                 str(self.wind[la][lo][0]) + ' ' + \
                                 str(self.wind[la][lo][1]) + ' ' + \
                                 ' 0 0 0 \n')
        return

#------------------------------------------------------------------------------
#   MAIN
#------------------------------------------------------------------------------

if __name__ == '__main__':
    
    uw = NMCVar('2014.nc', 'uwnd')
    vw = NMCVar('2014.nc', 'uwnd')

    carw = NMCWind ( uw, vw, 1)

    carw.make_cartesian_()
    
    carw.grid_save_('test.txt')
