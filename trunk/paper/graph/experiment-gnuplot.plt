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
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 32"
set ylabel "time(unit:sec)" font "times-roman, 32"
set yrange [50:2000]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("50" 50,"500" 500,"1000" 1000,"1500" 1500,"2000" 2000)
set output "time-v4k-e85k-a.eps"
plot "time-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-dfs with \rho
set output "time-v4k-e85k-b.eps"
plot "time-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

# runtime on syn-v4k-e85k-bfs with \rho
set output "time-v4k-e85k-c.eps"
plot "time-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"time-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"time-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"time-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 

#####################################################
#FIG cut-v4k-e85k
# cut/m on syn-v4k-e85k-random with \rho
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 32"
set yrange [0:0.6]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("0" 0,"0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5, "0.6" 0.6)
set output "cut-v4k-e85k-a.eps"
plot "cut-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

set output "cut-v4k-e85k-b.eps"
plot "cut-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

set output "cut-v4k-e85k-c.eps"
plot "cut-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"cut-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 

#####################################################
#FIG maxload-v4k-e85k
# maxload on syn-v4k-e85k-random with \rho
set xlabel "{/:Italic {/Symbol \162}}(%)" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 32"
set yrange [1:1.3]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5)
set ytics ("1" 1,"1.06" 1.06,"1.12" 1.12,"1.18" 1.18,"1.24" 1.24,"1.3" 1.3)
set output "maxload-v4k-e85k-a.eps"
plot "maxload-v4k-e85k-a.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-a.dat" using 1:5 title "SGLs" with linespoint 

set output "maxload-v4k-e85k-b.eps"
plot "maxload-v4k-e85k-b.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-b.dat" using 1:5 title "SGLs" with linespoint 

set output "maxload-v4k-e85k-c.eps"
plot "maxload-v4k-e85k-c.dat" using 1:2 title "FNL" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:3 title "SGLd-eq" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:4 title "SGLd-dbs" with linespoint, \
"maxload-v4k-e85k-c.dat" using 1:5 title "SGLs" with linespoint 


#####################################################
#FIG time-to-k-v4k-e85k
# time to k on syn-v4k-e85k-random with \rho
set xlabel "k" font "times-roman, 32"
set ylabel "time(unit:sec)" font "times-roman, 32"
set yrange [200:1100]
set xtics ("2" 2,"4" 4,"6" 6,"8" 8,"10" 10,"12" 12,"14" 14,"16" 16)
set ytics ("200" 200,"500" 500,"800" 800,"1100" 1100)
set output "time-to-k-v4k-e85k-a.eps"
plot "time-to-k-v4k-e85k.dat" using 1:2 title "FNL" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:5 title "SGLs" with linespoint 

set output "time-to-k-v4k-e85k-b.eps"
plot "time-to-k-v4k-e85k.dat" using 1:6 title "FNL" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:9 title "SGLs" with linespoint 

set output "time-to-k-v4k-e85k-c.eps"
plot "time-to-k-v4k-e85k.dat" using 1:10 title "FNL" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "time-to-k-v4k-e85k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG cut-to-k-v4k-e85k
# cut to k on syn-v4k-e85k-random with \rho
set xlabel "k" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 32"
set yrange [0:0.6]
set xtics ("2" 2,"4" 4,"6" 6,"8" 8,"10" 10,"12" 12,"14" 14,"16" 16)
set ytics ("0" 0,"0.2" 0.2,"0.4" 0.4,"0.6" 0.6)
set output "cut-to-k-v4k-e85k-a.eps"
plot "cut-to-k-v4k-e85k.dat" using 1:2 title "FNL" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:5 title "SGLs" with linespoint 

set output "cut-to-k-v4k-e85k-b.eps"
plot "cut-to-k-v4k-e85k.dat" using 1:6 title "FNL" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:9 title "SGLs" with linespoint 

set output "cut-to-k-v4k-e85k-c.eps"
plot "cut-to-k-v4k-e85k.dat" using 1:10 title "FNL" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "cut-to-k-v4k-e85k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG maxload-to-k-v4k-e85k
# maxload to k on syn-v4k-e85k-random with \rho
set xlabel "k" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 32"
set yrange [1:1.5]
set xtics ("2" 2,"4" 4,"6" 6,"8" 8,"10" 10,"12" 12,"14" 14,"16" 16)
set ytics ("1.0" 1,"1.1" 1.1,"1.2" 1.2,"1.3" 1.3,"1.4" 1.4,"1.5" 1.5)
set output "maxload-to-k-v4k-e85k-a.eps"
plot "maxload-to-k-v4k-e85k.dat" using 1:2 title "FNL" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:5 title "SGLs" with linespoint 

set output "maxload-to-k-v4k-e85k-b.eps"
plot "maxload-to-k-v4k-e85k.dat" using 1:6 title "FNL" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:9 title "SGLs" with linespoint 

set output "maxload-to-k-v4k-e85k-c.eps"
plot "maxload-to-k-v4k-e85k.dat" using 1:10 title "FNL" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "maxload-to-k-v4k-e85k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG cut-to-ac-v4k-e85k
# cut to ac on syn-v4k-e85k-random with \rho
set xlabel "AC" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 32"
set yrange [0:0.4]
set xtics ("10" 10,"20" 20,"30" 30,"40" 40,"50" 50,"60" 60,"70" 70,"80" 80)
set ytics ("0" 0,"0.2" 0.2,"0.4" 0.4)
set output "cut-to-ac-v4k-e85k-a.eps"
plot "cut-to-ac-v4k-e85k.dat" using 1:2 title "SGLd-eq" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:3 title "SGLd-dbs" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:4 title "SGLs" with linespoint 

set output "cut-to-ac-v4k-e85k-b.eps"
plot "cut-to-ac-v4k-e85k.dat" using 1:5 title "SGLd-eq" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:6 title "SGLd-dbs" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:7 title "SGLs" with linespoint 

set output "cut-to-ac-v4k-e85k-c.eps"
plot "cut-to-ac-v4k-e85k.dat" using 1:8 title "SGLd-eq" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:9 title "SGLd-dbs" with linespoint, \
     "cut-to-ac-v4k-e85k.dat" using 1:10 title "SGLs" with linespoint 

#####################################################
#FIG maxload-to-ac-v4k-e85k
# maxload to ac on syn-v4k-e85k-random with \rho
set xlabel "AC" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 32"
set yrange [1.0:1.3]
set xtics ("10" 10,"20" 20,"30" 30,"40" 40,"50" 50,"60" 60,"70" 70,"80" 80)
set ytics ("1.0" 1,"1.1" 1.1,"1.2" 1.2,"1.3" 1.3)
set output "maxload-to-ac-v4k-e85k-a.eps"
plot "maxload-to-ac-v4k-e85k.dat" using 1:2 title "SGLd-eq" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:3 title "SGLd-dbs" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:4 title "SGLs" with linespoint 

set output "maxload-to-ac-v4k-e85k-b.eps"
plot "maxload-to-ac-v4k-e85k.dat" using 1:5 title "SGLd-eq" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:6 title "SGLd-dbs" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:7 title "SGLs" with linespoint 

set output "maxload-to-ac-v4k-e85k-c.eps"
plot "maxload-to-ac-v4k-e85k.dat" using 1:8 title "SGLd-eq" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:9 title "SGLd-dbs" with linespoint, \
     "maxload-to-ac-v4k-e85k.dat" using 1:10 title "SGLs" with linespoint 

#####################################################
#FIG time-v-change-e8k
# time to v on syn-v4k-e85k-random with \rho
set xlabel "# Vex" font "times-roman, 32"
set ylabel "time(unit:sec)" font "times-roman, 32"
set yrange [0:1100]
set xtics ("2k" 2000,"3k" 3000,"4k" 4000,"5k" 5000)
set ytics ("0" 0,"200" 200,"400" 400,"600" 600,"800" 800,"1000" 1000,"1200" 1200)
set output "time-v-change-e80k-a.eps"
plot "time-v-change-e80k.dat" using 1:2 title "FNL" with linespoint, \
     "time-v-change-e80k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "time-v-change-e80k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "time-v-change-e80k.dat" using 1:5 title "SGLs" with linespoint 

set output "time-v-change-e80k-b.eps"
plot "time-v-change-e80k.dat" using 1:6 title "FNL" with linespoint, \
     "time-v-change-e80k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "time-v-change-e80k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "time-v-change-e80k.dat" using 1:9 title "SGLs" with linespoint 

set output "time-v-change-e80k-c.eps"
plot "time-v-change-e80k.dat" using 1:10 title "FNL" with linespoint, \
     "time-v-change-e80k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "time-v-change-e80k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "time-v-change-e80k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG cut-v-change-e80k
set xlabel "# Vex" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 32"
set yrange [0.5:0.7]
set xtics ("2k" 2000,"3k" 3000,"4k" 4000,"5k" 5000)
set ytics ("0.5" 0.5,"0.55" 0.55,"0.6" 0.6,"0.65" 0.65,"0.7" 0.7)
set output "cut-v-change-e80k-a.eps"
plot "cut-v-change-e80k.dat" using 1:2 title "FNL" with linespoint, \
     "cut-v-change-e80k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "cut-v-change-e80k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "cut-v-change-e80k.dat" using 1:5 title "SGLs" with linespoint 

set output "cut-v-change-e80k-b.eps"
plot "cut-v-change-e80k.dat" using 1:6 title "FNL" with linespoint, \
     "cut-v-change-e80k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "cut-v-change-e80k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "cut-v-change-e80k.dat" using 1:9 title "SGLs" with linespoint 

set output "cut-v-change-e80k-c.eps"
plot "cut-v-change-e80k.dat" using 1:10 title "FNL" with linespoint, \
     "cut-v-change-e80k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "cut-v-change-e80k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "cut-v-change-e80k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG maxload-v-change-e80k
set xlabel "# Vex" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 32"
set yrange [1.0:1.12]
set xtics ("2k" 2000,"3k" 3000,"4k" 4000,"5k" 5000)
set ytics ("1.0" 1,"1.05" 1.05,"1.1" 1.1,"1.15" 1.15)
set output "maxload-v-change-e80k-a.eps"
plot "maxload-v-change-e80k.dat" using 1:2 title "FNL" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:5 title "SGLs" with linespoint 

set output "maxload-v-change-e80k-b.eps"
plot "maxload-v-change-e80k.dat" using 1:6 title "FNL" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:9 title "SGLs" with linespoint 

set output "maxload-v-change-e80k-c.eps"
plot "maxload-v-change-e80k.dat" using 1:10 title "FNL" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "maxload-v-change-e80k.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG time-v5k-e-change
set xlabel "# EDGE" font "times-roman, 32"
set ylabel "time(unit:sec)" font "times-roman, 32"
set yrange [0:3000]
set xtics ("80k" 80000,"100k" 100000,"120k" 120000,"140k" 140000, "160k" 160000)
set ytics ("0" 0,"500" 500,"1000" 1000,"1500" 1500,"2000" 2000,"2500" 2500,"3000" 3000)
set output "time-v5k-e-change-a.eps"
plot "time-v5k-e-change.dat" using 1:2 title "FNL" with linespoint, \
     "time-v5k-e-change.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "time-v5k-e-change.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "time-v5k-e-change.dat" using 1:5 title "SGLs" with linespoint 

set output "time-v5k-e-change-b.eps"
plot "time-v5k-e-change.dat" using 1:6 title "FNL" with linespoint, \
     "time-v5k-e-change.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "time-v5k-e-change.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "time-v5k-e-change.dat" using 1:9 title "SGLs" with linespoint 

set output "time-v5k-e-change-c.eps"
plot "time-v5k-e-change.dat" using 1:10 title "FNL" with linespoint, \
     "time-v5k-e-change.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "time-v5k-e-change.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "time-v5k-e-change.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG cut-v5k-e-change
set xlabel "# EDGE" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \172}}(%)" font "times-roman, 32"
set yrange [0.5:0.7]
set xtics ("80k" 80000,"100k" 100000,"120k" 120000,"140k" 140000, "160k" 160000)
set ytics ("0.5" 0.5,"0.55" 0.55,"0.6" 0.6,"0.65" 0.65,"0.7" 0.7)
set output "cut-v5k-e-change-a.eps"
plot "cut-v5k-e-change.dat" using 1:2 title "FNL" with linespoint, \
     "cut-v5k-e-change.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "cut-v5k-e-change.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "cut-v5k-e-change.dat" using 1:5 title "SGLs" with linespoint 

set output "cut-v5k-e-change-b.eps"
plot "cut-v5k-e-change.dat" using 1:6 title "FNL" with linespoint, \
     "cut-v5k-e-change.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "cut-v5k-e-change.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "cut-v5k-e-change.dat" using 1:9 title "SGLs" with linespoint 

set output "cut-v5k-e-change-c.eps"
plot "cut-v5k-e-change.dat" using 1:10 title "FNL" with linespoint, \
     "cut-v5k-e-change.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "cut-v5k-e-change.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "cut-v5k-e-change.dat" using 1:13 title "SGLs" with linespoint 

#####################################################
#FIG maxload-v5k-e-change
set xlabel "# EDGE" font "times-roman, 32"
set ylabel "{/:Italic {/Symbol \164}}(%)" font "times-roman, 32"
set yrange [1.0:1.12]
set xtics ("80k" 80000,"100k" 100000,"120k" 120000,"140k" 140000, "160k" 160000)
set ytics ("1.0" 1,"1.05" 1.05,"1.1" 1.1,"1.15" 1.15)
set output "maxload-v5k-e-change-a.eps"
plot "maxload-v5k-e-change.dat" using 1:2 title "FNL" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:3 title "SGLd-eq" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:4 title "SGLd-dbs" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:5 title "SGLs" with linespoint 

set output "maxload-v5k-e-change-b.eps"
plot "maxload-v5k-e-change.dat" using 1:6 title "FNL" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:7 title "SGLd-eq" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:8 title "SGLd-dbs" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:9 title "SGLs" with linespoint 

set output "maxload-v5k-e-change-c.eps"
plot "maxload-v5k-e-change.dat" using 1:10 title "FNL" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:11 title "SGLd-eq" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:12 title "SGLd-dbs" with linespoint, \
     "maxload-v5k-e-change.dat" using 1:13 title "SGLs" with linespoint 

unset output
reset
