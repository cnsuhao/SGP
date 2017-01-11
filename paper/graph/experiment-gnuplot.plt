set title "A demonstration of gnuplot"
set grid
set key right top Left reverse width 0 box 3
set xlabel "memory"
set ylabel "time"
set yrange [50:3000]
set xtics ("0.1" 0.1,"0.2" 0.2,"0.3" 0.3,"0.4" 0.4,"0.5" 0.5,"0.6" 0.6,"0.7" 0.7,"0.8" 0.8,"0.9" 0.9)
set ytics ("50" 50,"500" 500,"1000" 1000,"1500" 1500,"2000" 2000, "2500" 2500, "3000" 3000)
set size 0.7,0.6
set terminal latex
set output "D:\\个人工作\\研究\\big graph partition\\SGP\\trunk\\paper\\graph\\plot.tex"
plot "D:\\个人工作\\研究\\big graph partition\\SGP\\trunk\\paper\\graph\\ex1.dat" using 1:2 title "FNL" with linespoint, "D:\\个人工作\\研究\\big graph partition\\SGP\\trunk\\paper\\graph\\ex1.dat" using 1:3 title "SGLd" with linespoint
unset output
reset
#plot cos(x), sin(x), sin(1+x)
#D:\\个人工作\\研究\\big graph partition\\SGP\\trunk\\paper\\graph\\