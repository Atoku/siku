'''Siku: Sea Ice Discrete Element Method Model
   File: material.py
   Basic template class for material data

'''

class Material:

    class Default:
        '''Default material. 

        Data for thickness (10 layers) are used based on Jia Wang
        et.al. report "Sea Ice-Ocean-Oilspill Modeling System (SIOMS)
        for the Nearshore Beaufort and Chukchi Seas: Parameterization
        and Improvement (Phase II). Final Report, OCS Study MMS
        2008-021. February 2010.

        sigma_t from Hopkins 2003. sigma_c also.

        '''

        def __init__( self ):
            self.name = 'Default ice'
            self.thickness_intervals = [ 0.2, 0.5, 1.0, 1.5, 2.0, 
                                         3.0, 4.0, 5.0, 6.0, 7.0 ] # m
            self.rho = [910.0]*len( self.thickness_intervals )     # kg/m^3
            self.sigma_t = [ 50.0,  150.0, 200.0, 250.0, 350.0,    # kPa
                             450.0, 550.0, 650.0, 750.0, 850.0 ]
            self.sigma_c = [ 1285*0.5 ]*len( self.thickness_intervals ) # kPa
            self.E  = 1.8       # GPa
            self.nu = 0.7       # 1
            return

        pass

    default = Default()         # default class

    def __init__( self ):
        '''Set all default values, no copying, so update wisely: replace the
        whole lists, not components.

        '''
        self.name    = self.default.name
        self.thickness_intervals = self.default.thickness_intervals
        self.rho     = self.default.rho
        self.sigma_t = self.default.sigma_t
        self.sigma_c = self.default.sigma_c
        self.E       = self.default.E
        self.nu      = self.default.nu
        return

    pass
