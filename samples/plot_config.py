#This is config file for plotting_test
#It must be created manualy
#All options are optional)
#Options marked like '!*' may be passed from scenario
#Most importano option is 'view' - in defines projection and map region

uwind_file = 'u2014.nc' #!* #file for uwind source for grid and interpolation
vwind_file = 'v2014.nc' #!* #file for vwind source for grid and interpolation
#time_index = -1 #!* #time index in NMC file 
#grid_step_lat = 2.5 #grid step in latitude degrees
#grid_step_lon = 2.5 #grid step in longitude degrees
#out_pic_name = 'outDraw.eps' #!* #output picture name
#verbose = False #verbosity

#Interpolation domain in degrees. Only used for interpolating winds.
#inter_domain = ( 0, 360, 75, 90 )  #north pole
#inter_domain = ( 150, 250, 65, 85 )  #beaufot-chukchi
#inter_domain = ( 262.5, 302.5, 7, 30 ) #caribbean
#inter_domain = ( 0, 360, -90, 90 )
#inter_domain = ( 330, 350, 50, 60 )
#inter_domain = ( -10, 10, -10, 10 )

inter_domain = ( 170, 240, 55, 85 )

#interpolation grid resolution in degrees (average).
#Zero means 'no additional interpolation for output'
#inter_density = 0 # average=5.
#inter_density = 0.5
inter_density = 0

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#Beaufort-Chukchi sea, Lambert Equal Area (seems to be)
##view = '-R190/65/240/75r -JA205/71/15 -Bag30 '
view = '-R185/62/240/75r -JA205/71/15 -Bag30 '
##view = '-Rg -JA205/71/15 -Bag30 '

#Polar Stereographic (hope that is really stereo)
#view = '-R315/70/135/70r -JS0/90/6i -Bag30 ' #north pole
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#view = '-R262.5/7/302.5/30r -JA282.5/20/15 -Bag30 ' #caribbean
#view = '-R175/-7/215/35r -JA195/10/15 -Bag30 ' #speedtest
#view = '-R0/360/78/90 -JG0/90/6i -Bag30 '      #north pole
#view = '-Rg -JG350/20/6i -Bag30 '
#view = '-R330/350/50/60 -JB340/0/50/50/6i -Bag30 '
#view = '-R-20/20/-20/20 -JB340/0/50/50/6i -Bag30 '

#ground_colr = '255/226/164' #ground color
#coasts = 'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0\
#-I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '

#vector_scaling = 0.1
#vector_scaling = 0.01
#vector_scaling = 5.5

overlays = [
##'psxy shapes.ll -R -J -Sc0.01 -B -Gred'
    ]
##inter_wind = 'psvelo interpolated_vectors.txt -R -J -W0.525p,forestgreen \
##-A2.5p+e -L -Se'
grid_wind = 'psvelo grid.txt -R -J -W0.25p,blue -L -A1.50p+e+gblue -Se'

#any underlays (right after coasts, befor winds)
underlays = [
        'psxy Poly*.txt -R -J -B -Gred -L '#draw poly for freedrift-monitor
    ]

#any additional overlays
#overlays = [
#'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0 -I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '
#'psvelo int_res2 -R -J -W0.25p,10/180/10  -L -A3.0p+e+g10/180/10 -Se0.04/1/1'
#'psvelo int_res1 -R -J -W0.25p,red  -L -A3.0p+e+gred -Se0.04/1/1'
#'psvelo int_res -R -J -W0.25p,150/50/50  -L -A3.0p+e+g150/50/50 -Se0.04/1/1'
#'psvelo grid.txt -R -J -W0.5p,white  -L -A3.0p+e+gwhite -Se0.04/1/1'
#'psvelo vel1 -R -J -W0.5p,red  -L -A3.0p+e+gred -Se10/1/1'
#    ]
