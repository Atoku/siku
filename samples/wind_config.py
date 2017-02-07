# The demonstration of interpolated winds. The interpolation procedure
# and it`s results exist only for drawing module without interfering the
# calculation process itself.

inter_domain = ( 170, 250, 25, 75 )

inter_density = 0.8
vector_scaling = 0.2

view = '-Rg -JA210/71/15 -X1 -Y1.5 -Bag30 '

inter_wind = 'psvelo interpolated_vectors.txt -R -J -W0.525p,forestgreen -A2.5p+e -L -Se'
