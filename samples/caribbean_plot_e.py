#just some caribbean sea
view = '-R250/-8/330/8r -JA290/0/70 -X1 -Y1 -Bag30 '

uwind_file = 'u2014.nc' #!* #file for uwind source for grid and interpolation
vwind_file = 'v2014.nc' #!* #file for vwind source for grid and interpolation

vector_scaling = 3

inter_wind = 'psvelo interpolated_vectors.txt -R -J -W0.525p,forestgreen \
-A2.5p+e -L -Se'
grid_wind = 'psvelo grid.txt -R -J -W0.25p,blue -L -A1.50p+e+gblue -Se'


##coasts = ' '
##ground_colr = '109/202/255'
