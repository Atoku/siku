'''File: wnd.py

Contains classes for wind grid storaging and methods for translation wind
speed data into Cartesian coordinates array.
'''

import geocoords
from nmc import NMC

#------------------------------------------------------------------------------
#   LATITUDE-LONGITUDE
#------------------------------------------------------------------------------

        
class Wind(NMC):
    '''
    A class for storaging and processing wind data for siku
    '''
    def open( self, filename, wind_value='uwnd' ):
        '''Opens file and reads grid and time information
        '''
        NMC.open( self, filename )
        self.read_wnd_( wind_value )

        return

    def is_header_match( self, nmc ):
        '''Checks if the header of another NMC object matches the current one

        '''
        return ( NMC.is_header_match( nmc ) and \
                 (self.wnd == nmc.wnd).all() )

    def reinit_( self ):
        NMC.reinit_( self )
        self.wnd = []
        return

    def read_wnd_( self, wind_value ):
        '''Reads uwnd data from file
        '''
        self.wnd = self.f1.variables[wind_value][:][:][:]
        return

    pass

#------------------------------------------------------------------------------
# CARTESIAN
#------------------------------------------------------------------------------

class CartesianWind: #actually more like a template for now
    '''A class for storaging NMC wind data in Cartesian 3D coords

    Wind data is storaged as 2D lists:
    first dimension is time
    second dimension is an index of coords list

    Coords are stored as 1D list of [x, y, z] lists
    '''
    def __init__( self, wind=None ):

        #in case of immidiate initialization
        if wind:
            self.make_cartesian_( wind )

        return
    
    def make_cartesian_( self, wind ): #processes coords automaticly
        '''Converts data into Cartesian grid
        '''
        self.times = wind.times_raw
        self.coords = [[geocoords.xyz_geographic(wind.lat[la],wind.lon[lo]) \
            for lo in range(len(wind.lon)) for la in range(len(wind.lat))] \
            for t in range(3)]#len(self.times))] #too much data!!
        self.wind = [[wind.wnd[t][la][lo] \
            for lo in range(len(wind.lon)) for la in range(len(wind.lat))] \
            for t in range(3)]#len(self.times))] #too much data!!

## Attempt to process all in 1 cycle. Failed due to bad pack/unpack procedure
##        (self.wind, self.coords) = \
##                     [[(wind.wnd[t][la][lo],\
##                     geocoords.xyz_geographic(wind.lat[la],wind.lon[lo])) \
##                     for lo in range(len(wind.lon)) \
##                     for la in range(len(wind.lat))] \
##                     for t in range(3)]#len(self.times))]
     
        return
    
    def test_save_( self, filename ):
        '''saves data into file
        '''
        with open(filename, mode='w') as output:
            ##print('test')
            output.write(str(self.wind[0][0]))
        return

#------------------------------------------------------------------------------
#   MAIN
#------------------------------------------------------------------------------

if __name__ == '__main__':
    
    uw = Wind('2014.nc')

    carw = CartesianWind ( uw )
    
    carw.test_save_('test.txt')
