'''File: gmt.py, A class to prepare and run shell scripts for GMT5 to
produce all different graphics for siku

'''

import os

import math
from . import geocoords                # geocoordinates


class GMT:
    '''Siku: GMT class

    A class to prepare and run shell scripts for GMT5 to produce all
    different graphics for siku.

    '''

    # List of all templates to use in the class
    TPLS = {
        'header': '{tpldir}/gmt-header.sh.tpl',
        'footer': '{tpldir}/gmt-footer.sh.tpl',
        'vfield': '{tpldir}/gmt-vfield.sh.tpl',
    }

    class STD:
        pass

    def __init__( self ):
        '''Creates object, reads most of the templates'''

        # standard data to use

        self.std = self.STD()
        self.std.region = {}
        self.std.region['whole_planet'] = '0/360/-90/90'
        self.std.region['beaufort_chukchi'] = '172/243/62/80'

        self.std.proj = {}
        self.std.proj['glob_azimuthal_equator'] = 'A0/0/4i'
        self.std.proj['alaska_albers'] = "B-154/50/55/65/16c"

        # directory where we are

        self.module_dir = os.path.dirname( __file__ )

        # default settings 

        self.settings = {}
        self.settings['projection'] \
            = self.std.proj['glob_azimuthal_equator']
        self.settings['region'] = self.std.region['whole_planet']

        self.settings['input'] = 'data.txt'
        self.settings['output'] = 'tmp.eps'
        self.settings['fileroot'] = 'gmt-temp-file'

        return

    def header( self ):
        '''Returns header for the script to generate the map'''
        fname = self.TPLS['header'].format( tpldir=self.module_dir )
        s = open( fname ).read()
        return s.format( **self.settings )

    def footer( self ):
        '''Returns footer for the script to generate the map'''
        fname = self.TPLS['footer'].format( tpldir=self.module_dir )
        s = open( fname ).read()
        return s.format( **self.settings )

    def vfield( self, fname_input=False ):
        '''Returns line for output of vector field'''
        fname = self.TPLS['vfield'].format( tpldir=self.module_dir )
        
        tmp_fname = self.settings['input']
        if ( fname_input ):
            self.settings['input'] = fname_input

        s = open( fname ).read().format( **self.settings )
        self.settings['input'] = tmp_fname

        return s
        

    pass
