# This is tutorial config file that demonstrates all possible features of
# automated GMT (generic mapping tools) adjustment.
# Any kind of scenario file must use such config files to set frame of
# map, colors, brushes, overlays and so on. However most of the options
# may be omitted as have default values.

# Options marked like '!*' may (and often must) be passed from scenario
# as it may for several files drawn in one run.

# This very file is not used in any scenrio.
##__________________________________________________________________________

# grid files are .nc files with proper content. These files are being used
# to extract wind data, interpolate it if necessary and draw to output picture.
uwind_file = 'u2014.nc' #!* #file for uwind source for grid and interpolation
vwind_file = 'v2014.nc' #!* #file for vwind source for grid and interpolation

time_index = 0 #!* #time index in .nc file as the first dimension of
               # stored data is time.
    
out_pic_name = 'outDraw.eps' #!* #output picture name.

verbose = False #verbosity (amount of logs printed in console)

# Interpolation domain in degrees. Only used for interpolating winds when one
# needs to see the wind configuration in some region of smaller size than
# the input wind grid.
inter_domain = ( 0, 360, 75, 90 )  #north pole
##inter_domain = ( 150, 250, 65, 85 )  #beaufot-chukchi seas
##inter_domain = ( 262.5, 302.5, 7, 30 ) #caribbean sea

# interpolation grid resolution in degrees (average).
# Zero means 'no additional interpolation for output'
inter_density = 0.5

# scaling of wind vectors on picture
vector_scaling = 0.1

### Most important option is 'view' - in defines projection and map region

#Beaufort-Chukchi sea, Lambert Equal Area (seems to be)
view = '-R190/65/235/75r -JA210/71/15 -X1 -Y1.5 -Bag30 '
# '-X1 -Y1.5' - displacements of picture relative to frame.
##view = '-R315/70/135/70r -JS0/90/6i -Bag30 ' # Polar Stereographic projection
##view = '-R262.5/7/302.5/30r -JA282.5/20/15 -Bag30 ' # caribbean
##view = '-Rg -JG350/20/6i -Bag30 ' # entire planet

ground_colr = '255/226/164' # ground color for GMT

# command to draw the land. Have nice default settings (recomended to use).
coasts = 'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0\
-I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '


# Any GMT underlays (right after coasts, befor winds). Various commands
# may be given. Main purpose - draw ice floe polygons.
underlays = [
        'psxy Poly*.txt -R -J -B -Gred -L '#draw poly for freedrift-monitor
    ]

# Grid and interpolated winds.
grid_wind = 'psvelo grid.txt -R -J -W0.25p,blue -L -A1.50p+e+gblue -Se'
##inter_wind = 'psvelo interpolated_vectors.txt -R -J -W0.525p,forestgreen \
##-A2.5p+e -L -Se'

# Any additional overlays to be drawn above all other layers. For example
# date stamps, text data e.t.c.
##overlays = [
##'pscoast -R -J -B -I1/0.25p,70/150/255 -N1/0.15p,110/80/0 -I2/0.1p,70/150/255 -G187/142/46 -S109/202/255 '
##'psvelo interpolation_res2 -R -J -W0.25p,10/180/10  -L -A3.0p+e+g10/180/10 -Se0.04/1/1'
##    ]
