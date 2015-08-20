''' Class for loading a dictionary from text file
'''

class Dictionary(dict):
    '''Simply the class for loading and storaging dictionaries
    '''
    def __init__( self, filename = 'dict.txt', splitter = None):
        '''Init and try to load dictionary
        '''
        self.clear()
        if filename:
            self.load( filename, splitter )
            
    def load( self, filename = 'dict.txt', splitter = None):
        '''try to load dictionary from file
        '''
        f = open(filename, 'r+')
        while not f.closed:
            line = f.readline()
            while line and (line[0] == '#' or line[0] == ' ' or \
                            line[0] == '\n'):
                line = f.readline()
            if not line:
                f.close()
                return
            temp = line.split(splitter, 2)
            self[temp[0]] = temp[1]
            
    pass


#--------------------------------- MAIN ------------------------------

if __name__== '__main__':
    d = Dictionary()
    print (d)
    
