'''Generator of random 2D points in range

Generates pairs of values packed in tuples
Can use own methed or hpgrid generation

'''
import random
import math

import hpgrid
from hpgrid import DEGREES,RADIANS

class RandVecGenerator:
    '''Class for generating random 2d vectors'''
    def __init__( self, min_x, max_x, min_y, max_y, seed = None ):
        '''Initializer with range specification'''
        self.reinit( min_x, max_x, min_y, max_y, seed )

    def reinit( self, min_x, max_x, min_y, max_y, seed = None ):
        '''Respecification of range'''
        self.min_x = min_x
        self.max_x = max_x
        self.min_y = min_y
        self.max_y = max_y
        self.Domain = hpgrid.Domain((min_x,max_x),(min_y,max_y), DEGREES)
        self.Grid = hpgrid.Grid( self.Domain )
        random.seed(seed)  
        
    def generate( self ):
        '''Generation of random tuple ( x, y )'''
        return ( random.uniform(self.min_x, self.max_x), \
                 random.uniform(self.min_y, self.max_y) )

    def hp_generate( self, psi, units=DEGREES, verbose = True ):
        '''Generating hpgrid point set'''
        self.Grid.points_gen( psi, units, verbose )
        self.Grid.points_filter( psi, units, verbose )

def polar( filename = 'invecs.txt', Generator = None, amo=1000):
    '''For testing and generating vectors in polar coords AROUND THE POLE '''
    RV = Generator
    if not RV:
        RV = RandVecGenerator( -10, 10, -10, 10 )
    with open('invecs.txt','w') as f:
        for i in range(amo):
            temp = RV.generate()
            vec = (math.sqrt(temp[0]*temp[0] + temp[1]*temp[1]), \
                   math.atan2(temp[1],temp[0]) )
            f.write(str(90-vec[0])+'    '+str( math.degrees(vec[1]))+'\n')


if __name__=='__main__':
    pass
    
