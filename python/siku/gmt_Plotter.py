'''Module for interpolating and plotting wind data.
Right now it`s plotting random generated test data plot. 
'''

from math import sqrt
from subprocess import call 

from siku import gmt_Drawer
from siku import wnd
from siku import interpolate
from siku import rand_vec
from siku import hpgrid
from siku import geocoords

try:
    from gmt_Drawer import GMT_Drawer
    from  interpolate import Interpolator
    from hpgrid import DEGREES,RADIANS
    from geocoords import norm_delta
except:
    GMT_Drawer = gmt_Drawer.GMT_Drawer
    Interpolator = interpolate.Interpolator
    DEGREES = hpgrid.DEGREES
    RADIANS = hpgrid.RADIANS
    norm_delta = geocoords.norm_delta

##-----------------------------------------------------------------------------
##-------------------------------- THE PLOTTER --------------------------------
##-----------------------------------------------------------------------------

class GMT_Plotter:
    '''High level class for plotting GMT winds to .eps files

    Depends on gmt_Drawer module

    Creates draw_config.txt files that are executed by GMT_Drawer
    '''
    
    #template string for further fulfilling
    draw_prep_str = \
'''#this is config file for GMT_Drawer
#next line is header with output_file name
{pic_file_name}\n
#draw configuration
#Region and projection
{view} -G{ground_colr} -P -V{verb}\n
#Coasts
{coasts} -V{verb}

#Underlays\n'''
    #template string for further fulfilling
    draw_wind_str = \
'''
#Winds
{inter_wind}{inter_scale}/1/1 -V{verb}
{grid_wind}{grid_scale}/1/1 -V{verb}

#Other overlays\n'''

    #dafault values dictionary for template string completion
    deft_conf = {
        'vector_scaling' : 1.0,
        'coasts' : 'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0\
-I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 ',
        'inter_wind' : 'psvelo interpolated_vectors.txt -R -J \
-W0.25p,lightGreen -L -A3.0p+e+glightGreen -Se',
        'grid_wind' : 'psvelo grid.txt -R -J -W0.5p,blue -L -A3.0p+e+gblue -Se',
        'underlays' : ['psxy Poly*.txt -R -J -B -Gred -L ']
    }

    def __init__( self, configFile ):
        '''Init, try to load config file
        '''
        self.config = {}
        self.W = None
        self.import_config(configFile)

    def import_config( self, configFile ):
        '''Import configuration from specified file'''
        exec( open( configFile ).read(), globals(), self.config )
                    
        self.verbose = 'q'
        if self.config.get('verbose') == True:
             self.verbose = 'd'

        self.uwf = self.config.get( 'uwind_file', None )
        self.vwf = self.config.get( 'vwind_file', None )
        self.domain = self.config.get( 'inter_domain', (0, 360, -90, 90) )

##-----------------------------------------------------------------------------
##--------------------------------- PLOTTING ----------------------------------
##-----------------------------------------------------------------------------
    
    def plot( self, out_file = 'map.eps', time = None, nmc_grid = None ):
        '''Main function:
        performs plotting by loading input, generating random positions grid,
        calling interpolation, preparing draw_config.txt and finaly executing
        DMT_Drawer
        '''
    
        if out_file:
            self.config['out_pic_name'] = out_file

        if time:
            self.config['time_index'] = time

        if self.config.get('verbose'):
            print( 'plotter start plotting' )

        psi = self.config.get( 'inter_density', 0 )
        
        if nmc_grid:            
            self.W = nmc_grid

        if self.W == None:        
            if self.uwf and self.vwf:
                UW = wnd.NMCVar( self.uwf, 'uwnd' )
                VW = wnd.NMCVar( self.vwf, 'vwnd' )
                self.W = wnd.NMCSurfaceVField( UW, VW,
                                  self.config.get( 'time_index', -1 ) )
            else:
                self.W = None
        
        if self.W:
            Inter = Interpolator( self.W, self.config.get( 'grid_step_lat', 2.5 ),\
                                  self.config.get( 'grid_step_lon', 2.5 ) )
         
            self.W.grid_save_( 'grid.txt' ) #saving base gird
         
##----------------------- generating interpolation grid -----------------------

            if self.config.get('verbose'):
                print('generating inter_grid')
         
            RV = rand_vec.RandVecGenerator( self.domain[0], \
                self.domain[1], 90 + self.domain[2], 90 + self.domain[3] )
                #domain[1], 90 - domain[3], 90 - domain[2] )
            if psi == 0:
                max_wind = 10
                psi = 1
            else:
                RV.hp_generate( psi, DEGREES, self.config.get( 'verbose' ) )
                vecs = RV.Grid.points_angular
         
##-------------------------- making interpolations ----------------------

                if self.config.get('verbose'):
                    print('interpolating')
                max_wind = 0
                with open('interpolated_vectors.txt','w') as outp:
                    for v in vecs:
                        temp = Inter.interpolate_simple( v[1], v[0] )
                        
                        if sqrt( temp[0]*temp[0] + temp[1]*temp[1]) > max_wind:
                            max_wind = sqrt( temp[0]*temp[0] + temp[1]*temp[1])
                            
                        outp.write(str(v[0])+' '+str(v[1])+' '+ \
                                   str(temp[0])+' '+str(temp[1])+' 0 0 0 \n')


##------------------------- preparing draw_config.txt -------------------------
        
        if self.config.get('verbose'):
            print('preparing draw_config')
        
        #scaling factor for vectors on picture
        if psi == 0:
            max_wind = 10
            psi = 1
        dphi = self.domain[1] - self.domain[0]
        dtheta = self.domain[3] - self.domain[2]
        width = sqrt ( abs( dphi*dtheta ) )
        psi *= self.config.get( 'vector_scaling', \
                self.deft_conf['vector_scaling'] )
##        if dphi > 90:
##            psi /=4
        
        with open('draw_config.txt','w') as dc:
            
            dc.write( self.draw_prep_str.format( \
                pic_file_name = self.config.get( 'out_pic_name', 'map.eps' ),
                view = self.config.get( 'view', '-Rg -JG350/20/6i -Bag30 ' ),
                ground_colr = self.config.get( 'ground_colr', '255/226/164' ),
                coasts = self.config.get( 'coasts', self.deft_conf['coasts'] ),
                verb = self.verbose
                ))
        
            for line in self.config.get( 'underlays',
                                         self.deft_conf['underlays'] ):
                dc.write( line + '\n' )

            inw = self.config.get( 'inter_wind', None )
                                   #self.deft_conf['coasts'] )
            grw = self.config.get( 'grid_wind', None )
                                    #self.deft_conf['grid_wind'] )
            
            if inw or grw:
                if inw == None:
                    inw = self.deft_conf['coasts']
                if grw == None:
                    grw = self.deft_conf['grid_wind']                
                
                dc.write( self.draw_wind_str.format(
                    verb = self.verbose, inter_wind = inw, grid_wind = grw,
                    inter_scale = str( (psi * 40) / (max_wind * width) ),
                    grid_scale = str( (psi * 40) / (max_wind * width) ),
                    ) )
                
            for line in self.config.get( 'overlays', [] ):
                dc.write( line + '\n' )
            
##-------------------------------- drawing ------------------------------
        
        frame = self.config.get( 'frame', None )
        if frame : call(["gmtset", str(frame)])
                
        if self.config.get('verbose'): print('drawing')
        self.D = GMT_Drawer('draw_config.txt')

        if self.config.get('verbose'): print('drawer initialized, drawing now')
        
        self.D.draw( verbose = self.config.get( 'verbose' ) ) #at last - plotting

        if self.config.get('verbose'): print('drawing DONE')
        
    pass

def main():
    G = GMT_Plotter ( 'plot_config.py' )
    G.plot()

if __name__ == '__main__':
    main()   
    
