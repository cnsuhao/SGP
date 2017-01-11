#D:\\个人工作\\研究\\big graph partition\\SGP\\trunk\\paper\\graph\\
#load "D:\\study\\big graph partition\\experiment\\paper\\graph\\experiment-gnuplot.plt"
#set grid
#set size 0.7,0.6
#set terminal latex
#set terminal epslatex standalone lw 2 color 11
#set output "D:\\study\\big graph partition\\experiment\\paper\\graph\\plot.tex"
#enhanced text mode : octal
#set loadpath "D:\\study\\big graph partition\\experiment\\paper\\graph\\"

set terminal postscript eps lw 2 color 11 enhanced
set key right top Left reverse width 0 box 3
set pointsize 2.0

# runtime on syn-v4k-e85k-random with \rho
set title "syn-v4k-e85k-random" font "times-roman, 18"
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 18"
set ylabel "time(unit:sec)" font "times-roman, 18"
set yrange [50:2000]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("50" 50,"500" 500,"1000" 1000,"1500" 1500,"2000" 2000)
set output "fig1.eps"
plot "fig1.dat" using 1:2 title "FNL" with linespoint, \
"fig1.dat" using 1:3 title "SGLd-eq" with linespoint, \
"fig1.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"fig1.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-dfs with \rho
set title "syn-v4k-e85k-dfs" font "times-roman, 18"
set output "fig2.eps"
plot "fig2.dat" using 1:2 title "FNL" with linespoint, "fig2.dat" using 1:3 title "SGLd-eq" with linespoint, "fig2.dat" using 1:4 title "SGLd-dbs" with linespoint, "fig2.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-bfs with \rho
set title "syn-v4k-e85k-bfs" font "times-roman, 18"
set output "fig3.eps"
plot "fig3.dat" using 1:2 title "FNL" with linespoint, "fig3.dat" using 1:3 title "SGLd-eq" with linespoint, "fig3.dat" using 1:4 title "SGLd-dbs" with linespoint, "fig3.dat" using 1:5 title "SGLs" with linespoint 

unset output
reset
