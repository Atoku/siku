'''GMT_Drawer: class for using GMT scripts to create .eps files

Should be used by GMT_Plotter module
'''

import os
import pickle

class GMT_Drawer:
    '''Class for drawing GMT staff
    '''
    def __init__( self, configFile = None ):
        '''Init, try to load dictionary and config file
        dictionary is made for predefining script options
        and fast access to them

        '''

        ## old idea to save configure strings into file and
        ## load when required. It does work yet no examples provided.
        self.dict = {}
        try:
            with open( 'draw_dict', 'rb' ) as inp:
                self.dict = pickle.load( inp )
        except:
            pass
        
        
        self.config_header = ''
        self.AUTOSPACING = True
        
        if configFile:
            self.load_config(configFile)

    def __del__( self ):
        '''Automatic saving dictionary configuration'''
        if len(self.dict) > 0:
            with open( 'draw_dict', 'wb' ) as out:
                pickle.dump( self.dict, out )
    
    def load_config( self, configFile = 'draw_config.txt' ):
        '''Load drawing configuration from specified file
        '''
        self.config = open(configFile)
        self.config_header = self.read_config()

    def read_config( self ):
        '''Read and split one line from config ignoring empty \
        and commented lines
        '''
        temp = self.config.readline()
        while temp and (temp[0] == '#' or temp[0] == ' ' or temp[0] == '\n'):
            temp = self.config.readline()
        if not temp:
            self.config.close()
        return temp.split()

    #-------------------------- drawing ----------------------------------

    def draw( self, configFile = None, verbose = True ):
        '''Perform GMT drawing with optional specification of \
        drawing configuration file
        '''
        if configFile:
            self.load_config(configFile)

        self.verbose = verbose
        self.start_drawing()
        while not self.config.closed:
            self.draw_overlay()
        self.finish_drawing()

    def start_drawing( self ):
        '''GMT create output file with header
        impement projection and region options
        '''
        if not self.config:
            raise Error('no draw config loaded')

        command = self.make_GMT_command('gmt psbasemap ', \
                    self.read_config(), ' -K > '+self.config_header[0])

        if command:
            if self.verbose:
                print(command)
            os.system(command)

    def draw_overlay( self ):
        '''GMT draw medium layer
        (for example set of vectors or some borders)
        '''
        if not self.config:
            raise Error('no draw config loaded')

        command = self.make_GMT_command('gmt', self.read_config(), \
                                    ' -K -O >> '+self.config_header[0])
        
        if command:
            if self.verbose:
                print(command)
            os.system(command)

    def finish_drawing( self ):
        '''GMT final layer
        implements script`s closing part
        '''
        if not self.config:
            raise Error('no draw config loaded')
        
        command = 'gmt psxy -R -J -T -O >> '+self.config_header[0]
        if not self.config.closed:
            self.config.close()
        if self.verbose:
            print(command)
        os.system(command)

    def make_GMT_command( self, begin_str, string, end_str ):
        '''Inner usage function
        should be 'protected' in C++ manner,
        but there is no 'protected' in python...
        '''
        if not string:
            return None

        command = begin_str
        for word in string:
            if word in self.dict:
                command = command + self.dict[word]
            elif self.AUTOSPACING:
                command = command + ' ' + word
            else:
                command = command + word
        command = command + end_str
        return command
	

#---------------------------------- MAIN -----------------------------------

if __name__=='__main__':
    D = GMT_Drawer()
    D.AUTOSPACING = True
    D.draw('draw_config.txt')
