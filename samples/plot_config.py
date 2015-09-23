#This is config file for plotting_test
#It mast be created manualy

uwind_file = 'u2014.nc'
vwind_file = 'v2014.nc'
time_index = -1 #time index to read
grid_step_lat = 2.5 #grid step in latitude degrees
grid_step_lon = 2.5 #grid step in longitude degrees
out_pic_name = 'outDraw.eps' #output picture name
verbose = False

#interpolation domain in degrees
#inter_domain = ( 0, 360, 75, 90 )  #north pole
inter_domain = ( 150, 250, 65, 85 )  #beaufot-chukchi
#inter_domain = ( 0, 360, -90, 90 )
#inter_domain = ( 330, 350, 50, 60 )
#inter_domain = ( -10, 10, -10, 10 )
inter_density = 0#5.525 #interpolation grid resolution in degrees (average)


#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#Beaufort-Chukchi sea, Lambert Equal Area (seems to be)
##view = '-R172/62/263/75r -JA205/71/15 -Bag30 '
view = '-Rg -JA205/71/15 -Bag30 '

#Polar Stereographic (hope that is really stereo)
#view = '-R315/70/135/70r -JS0/90/6i -Bag30 ' #north pole
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#view = '-R0/360/78/90 -JG0/90/6i -Bag30 ' #north pole
#view = '-Rg -JG350/20/6i -Bag30 '  #view
#view = '-R330/350/50/60 -JB340/0/50/50/6i -Bag30 ' #view
#view = '-R-20/20/-20/20 -JB340/0/50/50/6i -Bag30 ' #view

ground_colr = '255/226/164' #ground color
coasts = 'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0\
-I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '

vector_scaling = 0.02
#vector_scaling = 5.5

inter_wind = 'psvelo interpolated_vectors.txt -R -J -W0.25p,blue -L\
-A3.0p+e+gblue -Se'
grid_wind = 'psvelo grid.txt -R -J -W0.5p,white -L -A3.0p+e+gwhite -Se'

#any underlays (right after coasts, befor winds)
underlays = [
        'psxy Poly*.txt -R -J -B -Gred -L '#draw poly for freedrift-monitor
    ]

#any additional overlays
overlays = [
#'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0 -I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '
#'psvelo int_res2 -R -J -W0.25p,10/180/10  -L -A3.0p+e+g10/180/10 -Se0.04/1/1'
##'psvelo int_res1 -R -J -W0.25p,red  -L -A3.0p+e+gred -Se0.04/1/1'
#'psvelo int_res -R -J -W0.25p,150/50/50  -L -A3.0p+e+g150/50/50 -Se0.04/1/1'
#'psvelo grid.txt -R -J -W0.5p,white  -L -A3.0p+e+gwhite -Se0.04/1/1'
#'psvelo vel1 -R -J -W0.5p,red  -L -A3.0p+e+gred -Se10/1/1'
    ]
