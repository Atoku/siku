'''File: h5load.py
Module: h5load, internal module of siku module

Synchronized with c++ Core saving. Technical module for loading .h5 snapshots
into python siku scenarios.

'''

import h5py
import numpy as np
import mathutils
mV = mathutils.Vector

import siku
from   siku import polygon
from   siku import element
from   siku import material
from   siku import geocoords
from   siku import regrid
from   siku import gmt_Plotter
from   siku import wnd

#======================== Technical classes and functions ====================

class Wind:
    ''' Technical class for loading and handling winds '''

    def __init__( self, dataset = None ):
        '''default constructor analogue'''
        self.source_type = None
    
    def load_nmc( self, file ):
        ''' Method for loading nmc wind vecdield '''

        self.source_type = file['Wind/Source type'][0]
        self.uw = wnd.NMCVar( file['Wind/Source files'][0], 'uwnd' )
        self.vw = wnd.NMCVar( file['Wind/Source files'][1], 'vwnd' )
        self.time = file['Wind/Time index'][0]
        
        self.wind = wnd.NMCSurfaceVField( self.uw, self.vw, self.time )
            
    pass

class PlainElement:
    ''' Siku: class for loading full runtime info about element.
    Has more fields then Element, but less functionality '''
    
    def __init__( self, dataset = None ):
        '''default constructor analogue'''
        self.flag = 1 # aka free
        self.mod_ind = 0
        self.con_ind = 0
        self.id = 0
        
        self.q = list( ( 0, 0, 0, 0 ) )
        self.Glob = mV( ( 0, 0, 0 ) )
        self.V = ( 0, 0, 0 )
        self.m = 0.0
        self.I = 0.0
        self.W = mV( ( 0, 0, 0 ) )
        self.F = mV( ( 0, 0, 0 ) )
        self.N = 0.0

        self.imat = 0
        self.igroup = 0
        self.i = 0.0
        self.A = 0.0
        self.sbb_rmin = 0.0
        self.gh = [ 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0 ]
        self.P = []

        if dataset:
            self.load( dataset )

    def load( self, dataset ):
        ''' Method for loding from h5 dataset '''
        self.flag = dataset[0]
        self.mon_ind = dataset[1]
        self.con_ind = dataset[2]
        self.id = dataset[3]

        # q = Workaround. Should be disscussed and fixed #list( dataset[4] ) 
        self.q = [ float(dataset[4][3]), float(dataset[4][0]),
                   float(dataset[4][1]), float(dataset[4][2]) ]
        self.Glob = mV( dataset[5] )
        self.V = tuple( dataset[6] )
        self.m = dataset[7]
        self.I = dataset[8]
        self.W = mV( dataset[9] )
        self.F = mV( dataset[10] )
        self.N = dataset[11]

        self.imat = dataset[13]
        self.igroup = dataset[13]
        self.i = dataset[14]
        self.A = dataset[15]
        self.sbb_rmin = dataset[16]
        self.gh = list( dataset[17] )

    def to_element( self, mons, cons, mats ):
        ''' Method for conversion into siku.Element.
        Arguments:
        mons - list of monitor functions` names
        cons - list of control functions names
        '''
        el = element.Element()
        
        el.A = self.A
        el.i = self.i
        el.sbb_rmin = self.sbb_rmin
        el.q = self.q
        el.verts_xyz_loc = self.P
        
        el.imat = int( self.imat )
        el.flag_state = int( self.flag )
        el.velo = self.V
        el.set_gh( self.gh, mats[ self.imat ].to_mat() )
        
        el.monitor = mons[ self.mon_ind ]
        el.control = cons[ self.con_ind ]

        return el

class Info:
    ''' Technical class for storaging various string attributes. '''

    def __init__( self ):
        '''Default constructor analogue'''
        self.brief = ''
        self.date = ''
        self.name = ''
        self.rundate = ''
        self.version = ''

    def load( self, dataset ):
        ''' Method for loding from h5 dataset '''
        self.name = dataset[0].decode()
        self.brief = dataset[1].decode()
        self.version = dataset[2].decode()
        self.date = dataset[3].decode()
        self.rundate = dataset[4].decode()

class PlainMat:
    ''' Technical class for loading materials. '''

    def __init__( self, dataset = None ):
        '''Default constructor analogue'''
        self.name = ''
        self.E = 0.0
        self.nu = 0.0
        self.layers = []

        if dataset:
            self.load( dataset )

    def load( self, dataset ):
        ''' Method for loding from h5 dataset '''
        self.name = dataset[0].decode()
        self.E = dataset[1]
        self.nu = dataset[2]
        self.layers = dataset[3]

    def to_mat( self ):
        ''' Method for converting into siku.Material '''
        mat = material.Material()
        mat.name = self.name
        mat.E = self.E
        mat.nu = self.nu

        mat.thickness_intervals = []
        mat.rho = []
        mat.sigma_t = []
        mat.sigma_c = []

        for l in self.layers:
            mat.thickness_intervals.append( l[0] )
            mat.rho.append( l[1] )
            mat.sigma_t.append( l[2] )
            mat.sigma_c.append( l[3] )

        return mat

    pass 

def load_vertex( dataset ):
    ''' Technical function for loading vertex from .h5 dataset '''
    return ( dataset[0], tuple( dataset[1] ) )

#================================== Main Class ===============================

class Loader:
    ''' Siku: class for loading .h5 snapshots. Provides high-level input
    functionality '''

    def __init__( self, filename = None ):
        ''' Default initializer '''
        self.filename = filename
        self.file = None
        self.info = Info()
        self.mats = []
        self.els = []
        self.verts = []
        self.mons = []
        self.cons = []
        self.conts = []
        
        self.wind = None
        self.wind_source = None

        # preopen file if name is given
        if self.filename:
            self.file = h5py.File( self.filename, 'r' )

    def load( self, filename = None ):
        ''' Main high-level method. Calls numerous other methods and
        functions. '''
        if filename:
            self.filename = filename

        # optional opening file
        if self.filename and self.file == None:
            self.file = h5py.File( self.filename, 'r' )

        # bad input check
        if self.filename == None and self.file == None:
            return

        # loading info
        self.info.load( self.file['Info/Info'][0] )
        
        # loading  stuff
        self.load_mats()  # materials
        self.load_els()  # elements and vertices
        self.load_fnames()  # monitor and control functions` names
        self.load_conts()  # contacts
        self.load_wind()  # wind field (type autodetection)

    def load_els( self ):
        ''' Load elements from previously opened file '''
        # bad input check
        if self.file == None:
            return
        
        self.els = [ PlainElement( d ) for d in self.file['Elements/Elements'] ]
        self.verts = [ load_vertex( d ) for d in
                       self.file['Elements/Vertices'] ]
        [ self.els[ v[0] ].P.append( v[1] )  for v in self.verts ]

    def load_mats( self ):
        ''' Load materials from previously opened file '''
        # bad input check
        if self.file == None:
            return

        self.mats = [ PlainMat( d ) for d in self.file['Materials/Materials'] ]

    def load_fnames( self ):
        ''' Load monitoring and control functions` names from previously
        opened file '''
        # bad input check
        if self.file == None:
            return

        self.mons = [ d.decode() for d in self.file['Monitor functions'] ]
        self.cons = [ d.decode() for d in self.file['Control functions'] ]

    def load_conts( self ):
        ''' Load contacts list from previously opened file '''
        # bad input check
        if self.file == None:
            return

        self.conts = [ ( d[0], d[1], d[2] ) for d in
                       self.file['Contacts/Contacts'] ]

    def load_wind( self ):
        ''' Load wind grid from previously opened file '''
        # bad input check
        if self.file == None:
            return

        # different sources
        self.wind_source_type = self.file['Wind/Source type'][0]

        if self.wind_source == 2:  #nmc wind grid
            self.wind = Wind()
            self.wind.load_nmc( self.file )

#-----------------------------------------------------------------------------        

    def extract_els( self ):
        ''' Output method - returns list of elements. Require loaded monitor
        and control functions '''
        return [ e.to_element( self.mons, self.cons, self.mats )
                 for e in self.els ]

    def extract_mats( self ):
        ''' Output method - returns list of materials '''
        return [ m.to_mat() for m in self.mats ]

    def extract_mons( self ):
        ''' Output method - returns list of monitor functions` names '''
        return self.mons[:]

    def extract_cons( self ):
        ''' Output method - returns list of control functions` names '''
        return self.cons[:]

    def extract_wind( self ):
        ''' Output method - returns reference to Wind handler object '''
        return self.Wind

#==================================== Main ===================================

if __name__ == '__main__':
    ''' Testing function '''
    l = Loader('save_test.h5')
    l.load()
    print(l.mats[0].layers)
    print('\n')

    print(l.els[0].F)
    print(l.els[0].q)
    print('\n')
    
    print(l.conts)
