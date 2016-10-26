#!/usr/bin/gnuplot

set term png enhanced
set key right bottom
set size ratio -1

set xlabel "x"
set ylabel "y" 


#############################################################
set output "terre_soleil.png"

plot "./output.data" using 1:2 with lines title "soleil", \
    "./output.data" using 3:4 with lines title "terre"
