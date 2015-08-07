'''File: nmc.py, A class to read NMC reanalysis data of different kind
but mostly wind data to use in siku model.

'''

import os
import numpy
import netCDF4
import math
import datetime
import bisect

from . import geocoords                # geocoordinates

class NMC:
    '''Siku: NMC class

    A class to read NMC reanalysis data of different kind but mostly
    wind data to use in siku model.

    '''

    # Variables to convert NMC time to datetime structure
    TSTART = datetime.datetime( 1800, 1, 1, 0, 0, 0 )
    HOUR = datetime.timedelta( hours = 1 )

    def __init__( self, filename=False, arrayname=False ):
        '''Creates object to read and store reanalysis data
        filename -- string, what file to read
        arrayname -- string, main array to read
        '''

        self.reinit_()

        # special case when we set file name immidiately
        if filename:
            self.open( filename )

        if arrayname:
            self.arrayname = arrayname
        
        return

    def open( self, filename ):
        '''Opens file and reads grid and time information

        '''
        if self.f1:
            raise ( "Error: using open on opened file" )

        # opening and restoring time and grid
        self.filename = filename
        self.f1 = netCDF4.Dataset( filename )
        self.read_time_raw_()
        self.read_latlon_()
        self.convert_time_()

        return

    def read_data( self, time ):
        '''Reads the data from arrayname. This is the latest data that not
        exceeds time (in datetime format).
        '''
        return self.read_array_( self.arrayname, time )

    def close( self ):
        '''Closes the file and reinitializes the file pointer for future use

        '''
        self.f1.close()

        self.reinit_()

        return

    def is_header_match( self, nmc ):
        '''Checks if the header of another NMC object matches the current one

        '''
        return ( ( self.times_raw == nmc.times_raw ).all() and \
                 ( self.lat == nmc.lat ).all() and \
                 ( self.lon == nmc.lon ).all() )

    def __del__( self ):
        '''Safe destructor just in case

        '''
        if self.f1:
            self.f1.close()
        return

    def read_array_( self, name, time ):
        '''Returns array for a particular time (in datetime format)

        '''
        # find index by time in sorted times array
        ind = bisect.bisect_left( self.times, time )
        return self.f1.variables[name][:][ind]

    # ---------------------------------------------------------------
    # low level interface
    # ---------------------------------------------------------------

    def reinit_( self ):
        self.f1 = None
        self.lon = []           # longitude grid
        self.lat = []           # latitude grid
        self.times_raw = []     # raw time data (in floats)
        self.times = []         # data structure converted
        self.filename = None
        self.arrayname = None   # main array name
        return

    def read_latlon_( self ):
        '''Reads latitude and longitude arrays from the file

        '''
        self.lon = self.f1.variables['lon'][:]
        self.lat = self.f1.variables['lat'][:]
        return

    def read_time_raw_( self ):
        '''Reads raw time data from the file

        '''
        self.times_raw = self.f1.variables['time'][:]
        return

    def convert_time_( self ):
        '''Converts NMC time: hours from 1800-01-01 into datetime format and
        stores it into self.times

        '''
        self.times = [ self.TSTART + t*self.HOUR for t in self.times_raw ]
        return

    pass
