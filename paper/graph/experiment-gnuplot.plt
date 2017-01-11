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

#####################################################
#FIG time-v4k-e85k
# runtime on syn-v4k-e85k-random with \rho
set title "syn-v4k-e85k-random" font "times-roman, 18"
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 18"
set ylabel "time(unit:sec)" font "times-roman, 18"
set yrange [50:2000]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("50" 50,"500" 500,"1000" 1000,"1500" 1500,"2000" 2000)
set output "time-v4k-e85k-a.eps"
plot "time-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-dfs with \rho
set title "syn-v4k-e85k-dfs" font "times-roman, 18"
set output "time-v4k-e85k-b.eps"
plot "time-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-bfs with \rho
set title "syn-v4k-e85k-bfs" font "times-roman, 18"
set output "time-v4k-e85k-c.eps"
plot "time-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 

#####################################################
#FIG cut-v4k-e85k
# cut/m on syn-v4k-e85k-random with \rho
set title "syn-v4k-e85k-random" font "times-roman, 18"
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 18"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 18"
set yrange [0:0.6]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("0" 0,"0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5, "0.6" 0.6)
set output "cut-v4k-e85k-a.eps"
plot "cut-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

set title "syn-v4k-e85k-dfs" font "times-roman, 18"
set output "cut-v4k-e85k-b.eps"
plot "cut-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

set title "syn-v4k-e85k-bfs" font "times-roman, 18"
set output "cut-v4k-e85k-c.eps"
plot "cut-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 

#####################################################
#FIG maxload-v4k-e85k
# maxload on syn-v4k-e85k-random with \rho
set title "syn-v4k-e85k-random" font "times-roman, 18"
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 18"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 18"
set yrange [1:1.3]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("1" 1,"1.06" 1.06,"1.12" 1.12,"1.18" 1.18,"1.24" 1.24,"1.3" 1.3)
set output "maxload-v4k-e85k-a.eps"
plot "maxload-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

set title "syn-v4k-e85k-dfs" font "times-roman, 18"
set output "maxload-v4k-e85k-b.eps"
plot "maxload-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

set title "syn-v4k-e85k-bfs" font "times-roman, 18"
set output "maxload-v4k-e85k-c.eps"
plot "maxload-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 


unset output
reset
