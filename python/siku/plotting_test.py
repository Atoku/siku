from GMT_Drawer import GMT_Drawer
import wnd
from  interpolate import Interpolator

import pickle

if __name__ == '__main__':
    uw = wnd.NMCVar('u2014.nc', 'uwnd')
    vw = wnd.NMCVar('v2014.nc', 'vwnd')

    carw = wnd.NMCWind(uw,vw,1)

    Inter = Interpolator(carw, 2.5, 2.5)

    vecs = []
    with open('invecs.txt','r') as inp:
        line = inp.readline()
        while line:
            temp = line.split()
            vecs.append((float(temp[0]),float(temp[1])))
            line = inp.readline()

    with open('int_res','w') as outp:
        for v in vecs[:500]:
            temp = Inter.interpolate(v[0],v[1])
            outp.write(str(v[1])+' '+str(v[0])+' '+ \
                       str(temp[0])+' '+str(temp[1])+' 0 0 0 \n')        
    
    
    
    

    


    carw.test_save_('vel')    

    D = GMT_Drawer('dict.txt','Draw_config.txt')
##    f = open('testdict','wb')
##    pickle.dump(D.Dict,f,-1)
##    f.close()
    D.draw()

