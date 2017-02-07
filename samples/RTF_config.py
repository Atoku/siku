# Run_to - run_from config. Some simple view with winds and polygons
view = '-R185/62/240/75r -JA205/71/15 -Bag30 '

grid_wind = 'psvelo grid.txt -R -J -W0.25p,blue -L -A1.50p+e+gblue -Se'

underlays = [ 'psxy Poly*.txt -R -J -B -Gred -L ' ]
