from GMT_Drawer import GMT_Drawer
import wnd

if __name__ == '__main__':
    #out = open('vel', 'w')
    uw = wnd.NMCVar('2014.nc', 'uwnd')
    vw = wnd.NMCVar('2014.nc', 'uwnd')

    carw = wnd.NMCWind(uw,vw,1)

    carw.test_save_('vel')

    D = GMT_Drawer('dict.txt','Draw_config.txt')
    D.AUTOSPACING = True
    D.draw()
