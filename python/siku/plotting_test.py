'''Module for interpolating and plotting wind data.

!!!! Ruqeires discussion and planning for further development
Right now it`s plotting random generated test data with 3 kind of different
aproximation methods on one single plot. 
'''
from gmt_Drawer import GMT_Drawer
import wnd
from  interpolate import Interpolator
import rand_vec

#import pickle

if __name__ == '__main__':
    UW = wnd.NMCVar('u2014.nc', 'uwnd') #read NMC reanalysis uwind
    VW = wnd.NMCVar('v2014.nc', 'vwnd') #read NMC reanalysis vwind

    W = wnd.NMCWind(UW,VW,1) #produse NMCWind class

    Inter = Interpolator(W, 2.5, 2.5) #Inits interpolator with NMC data


##---------------------------- generating input --------------------------
    
##region should match itself in the draw_config.txt
    rand_vec.polar('invecs.txt', \
                   rand_vec.RandVecGenerator(-10,10,-10,10 ), 5000)

##this is for beaufort zone
##    with open('invecs.txt','w') as iv:
##        RV = rand_vec.RandVecGenerator(330, 350, 50, 60) #cyclon_eye
##        #RV = rand_vec.RandVecGenerator(186, 193, 67, 72) #beaufort_small
##        #RV = rand_vec.RandVecGenerator(175, 240, 65, 78) #beaufort_chukchi
##        for i in range(1000):
##            vec = RV.generate()
##            iv.write(str(vec[1])+'  '+str(vec[0])+'\n')

    
##-------------------------- making interpolations ----------------------

    vecs = []
    with open('invecs.txt','r') as inp:
        line = inp.readline()
        while line:
            temp = line.split()
            vecs.append((float(temp[0]),float(temp[1])))
            line = inp.readline()

##slerp - color green
    with open('int_res2','w') as outp:
        for v in vecs:
            temp = Inter.interpolate_slerp(v[0],v[1])
            outp.write(str(v[1])+' '+str(v[0])+' '+ \
                       str(temp[0])+' '+str(temp[1])+' 0 0 0 \n')   

##poportional azimuth-lenght - color blue
    with open('int_res1','w') as outp:
        for v in vecs:
            temp = Inter.interpolate(v[0],v[1])
            outp.write(str(v[1])+' '+str(v[0])+' '+ \
                       str(temp[0])+' '+str(temp[1])+' 0 0 0 \n')        

##proportional vector components - color red
    with open('int_res','w') as outp:
        for v in vecs:
            temp = Inter.interpolate_simple(v[0],v[1])
            outp.write(str(v[1])+' '+str(v[0])+' '+ \
                       str(temp[0])+' '+str(temp[1])+' 0 0 0 \n') 
    
##-------------------------------- drawing ------------------------------

    W.grid_save_('vel') #saving base gird

    #starting plotter with predef config
    D = GMT_Drawer('dict.txt','draw_config.txt') 
    
##    f = open('testdict','wb')
##    pickle.dump(D.Dict,f,-1)
##    f.close()
    D.draw() #at last - plotting

