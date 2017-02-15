set term post eps

set output "forces.eps"
plot 'group0.txt' u 1:2 w l lt rgb 'red', \
 'group1.txt' u 1:2 w l lt rgb 'black', \
 'group2.txt' u 1:2 w l lt rgb 'blue', \
 'group3.txt' u 1:2 w l lt rgb 'green'

set term x11
