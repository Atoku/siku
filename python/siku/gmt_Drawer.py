'''GMT_Drawer: class for using GMT scripts to create .eps files
'''
from Dict import Dictionary
import os

class GMT_Drawer:
    '''Class for drawing GMT staff
    '''
    def __init__( self, dictFile = None, configFile = None ):
        '''Init, try to load dictionary and config file
        '''
        self.Dict = Dictionary(dictFile, '\t')
        self.config_header = ''
        self.AUTOSPACING = True
        
        if configFile:
            self.load_config(configFile)

    def load_dict( self, dictFile = 'dict.txt' ):
        '''(re)loads dictionary from specified file
        '''
        self.Dict = Dictionary(dictFile, '\t')

    def expand_dict( self, dictFile):
        '''expand (or load if empty) a dictionary from specified file
        '''
        self.Dict.load( dictFile, '\t' )

    def load_config( self, configFile = 'draw_config.txt' ):
        '''load drawing configuration from specified file
        '''
        self.config = open(configFile)
        self.config_header = self.read_config()

    def read_config( self ):
        '''read and split one line from config ignoring empty \
        and commented lines
        '''
        temp = self.config.readline()
        while temp and (temp[0] == '#' or temp[0] == ' ' or temp[0] == '\n'):
            temp = self.config.readline()
        if not temp:
            self.config.close()
        return temp.split()

    #-------------------------- drawing ----------------------------------

    def draw( self, configFile = None ):
        '''perform GMT drawing with optional specification of \
        drawing configuration file
        '''
        if configFile:
            self.load_config(configFile)

        self.start_drawing()
        #for i in range(int(self.config_header[1])):
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
        print(command)
        os.system(command)

    def make_GMT_command( self, begin_str, string, end_str ):
        '''inner usage function
        should be 'protected' in C++ manner,
        but there is no 'protected' in python...
        '''
        if not string:
            return None

        command = begin_str
        for word in string:
            if word in self.Dict:
                command = command + self.Dict[word]
            elif self.AUTOSPACING:
                command = command + ' ' + word
            else:
                command = command + word
        command = command + end_str
        return command
	

#---------------------------------- MAIN -----------------------------------

if __name__=='__main__':
    D = GMT_Drawer('dict.txt')
    D.AUTOSPACING = True
    D.draw('draw_config.txt')
