'''Module for interpolating and plotting wind data.

!!!! Ruqeires discussion and planning for further development
Right now it`s plotting random generated test data with 3 kind of different
aproximation methods on one single plot. 
'''
from gmt_Drawer import GMT_Drawer
import wnd
from  interpolate import Interpolator
import rand_vec
from Dict import Dictionary
from math import sqrt
from hpgrid import DEGREES,RADIANS
from geocoords import norm_delta

class GMT_Plotter:
    def __init__( self, dictFile, configFile ):
        '''Init, try to load dictionary and config file
        '''
        self.dictFile = dictFile
        self.Dict = Dictionary(dictFile, '\t')
        self.load_config(configFile)

    def load_dict( self, dictFile = 'dict.txt' ):
        '''(re)loads dictionary from specified file
        '''
        self.Dict = Dictionary(dictFile, '\t')

    def expand_dict( self, dictFile):
        '''expand (or load if empty) a dictionary from specified file
        '''
        self.Dict.load( dictFile, '\t' )

    def load_config( self, configFile ):
        '''load drawing configuration from specified file
        '''
        self.config = open(configFile)
        self.uwnd_file_name = self.read_config()
        self.vwnd_file_name = self.read_config()
        self.time_index = int(self.read_config())
        self.grid_step_lat = float(self.read_config())
        self.grid_step_lon = float(self.read_config())
        self.pic_file_name = self.read_config()
        self.domain = tuple( self.read_config().split() )
        self.view = self.read_config()
        self.psi = float(self.read_config())
        #self.config_header = self.read_config()
        
    def read_config( self ):
        '''read and split one line from config ignoring empty \
        and commented lines
        '''
        if self.config.closed:
            raise ValueError('Corrupted config file')
        temp = self.config.readline()
        while temp and (temp[0] == '#' or temp[0] == ' ' or temp[0] == '\n'):
            temp = self.config.readline()
        if not temp:
            self.config.close()
            return ''
        t, c = temp.split('#',1)
        if t in self.Dict:
            return self.Dict[t]
        return t

##-----------------------------------------------------------------------------
##--------------------------------- PLOTTING ----------------------------------
##-----------------------------------------------------------------------------
    
    def plot( self ):
        print('plotter start plotting')
        UW = wnd.NMCVar(self.uwnd_file_name, 'uwnd')
        VW = wnd.NMCVar(self.vwnd_file_name, 'vwnd')
        W = wnd.NMCWind(UW, VW, self.time_index)
        Inter = Interpolator( W, self.grid_step_lat, self.grid_step_lon)
        W.grid_save_('grid.txt') #saving base gird

##----------------------- generating interpolation grid -----------------------

        print('generating inter_grid')
        RV = rand_vec.RandVecGenerator(float(self.domain[0]), \
            float(self.domain[1]), float(self.domain[2])+90, \
            float(self.domain[3])+90 )
        dphi = float(self.domain[1]) - float(self.domain[0])
        RV.hp_generate( self.psi )
        vecs = RV.Grid.points_angular

##-------------------------- making interpolations ----------------------
        print('interpolating')
        max_wind = 0
        with open('interpolated_vectors.txt','w') as outp:
            for v in vecs:
                temp = Inter.interpolate_simple(v[1],v[0])
                if sqrt( temp[0]*temp[0] + temp[1]*temp[1]) > max_wind:
                    max_wind = sqrt( temp[0]*temp[0] + temp[1]*temp[1])
                outp.write(str(v[0])+' '+str(v[1])+' '+ \
                           str(temp[0])+' '+str(temp[1])+' 0 0 0 \n')


##------------------------- preparing draw_config.txt -------------------------
        print('preparing draw_config')
        psi = self.psi
##        if dphi > 90:
##            psi /=4
        with open('draw_config.txt','w') as dc:
            dc.write('''#this is config file fog GMT_Drawer\n
#next line is header with output_file name\n''')
            dc.write(str(self.pic_file_name) + '\n')
            dc.write('\n#draw configuration\n#Region and projection\n')
            dc.write(str(self.view)+'-G255/226/164 -P\n')
            dc.write('''#Coasts\npscoast -R -J -B -I1/0.25p,70/150/255\
 -N1/0.15p,110/80/0 -I2/0.1p,70/150/255 -G187/142/46 -S109/202/255\n#winds
psvelo interpolated_vectors.txt -R -J -W0.25p,blue -L -A3.0p+e+gblue\
 -Se''')

            dc.write(str( (psi * 20)/(max_wind * dphi) )+'/1/1\n' )
            dc.write('psvelo grid.txt -R -J -W0.5p,white -L -A3.0p+e+gwhite -S')
            dc.write(str( (psi * 20)/(max_wind * dphi) )+'/1/1\n' )

            dc.write('\n#other overlays\n')
            while not self.config.closed:
                dc.write(self.read_config()+'\n')

##-------------------------------- drawing ------------------------------
        print('drawing')

        #starting drawer with created config
        D = GMT_Drawer(self.dictFile,'draw_config.txt') 
        D.draw() #at last - plotting

    pass

if __name__ == '__main__':
    G = GMT_Plotter( 'dict.txt', 'plot_config.txt')
    G.plot()
