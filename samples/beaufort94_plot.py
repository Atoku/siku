uwind_file = 'u1994.nc' #!* #file for uwind source for grid and interpolation
vwind_file = 'v1994.nc' #!* #file for vwind source for grid and interpolation

inter_domain = ( 170, 240, 55, 85 )

inter_density = 0

##view = '-R190/65/240/75r -JA205/71/15 -Bag30 '
##view = '-R185/62/240/75r -JA205/71/15 -Bag30 '
view = '-R190/65/235/75r -JA210/71/15 -Bag30 '

overlays = []

grid_wind = 'psvelo grid.txt -R -J -W0.25p,blue -L -A1.50p+e+gblue -Se'

underlays = [
    'psxy Poly*.txt -R -J -B -Gred -L '#draw poly for freedrift-monitor
    ]


