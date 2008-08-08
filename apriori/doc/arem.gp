#-----------------------------------------------------------------------
# File    : arem.gp
# Contents: visualization of the selection behaviour
#           of the additional rule evaluation measures
# Author  : Christian Borgelt
# History : 22.09.1998 file created
#-----------------------------------------------------------------------
# set terminal postscript

set view 50,325,1
set hidden3d
set isosamples 20,20

set xrange [0.001:0.999]
set yrange [0.001:0.999]
set zrange [0:1]

min(a,b) = a < b ? a : b

diff(x,y) = abs(x -y)

quot(x,y) = 1 -min(x/y, y/x)

info(x,y,s) = (x >= y*s) && (1-x >= s *(1-y)) \
            ? (s*y*log(y/x) +(x-s*y)*log((x-s*y)/(x*(1-s))) \
            +  s*(1-y)*log((1-y)/(1-x)) \
            + (1-x-s*(1-y))*log((1-x-s*(1-y))/((1-x)*(1-s)))) /log(2) : 0

chi2(x,y,s) = (x >= y*s) && (1-x >= s *(1-y)) \
            ? (x*s -y*s)**2 /(x*(1-x)*s*(1-s)) : 0

set title "d_diff (independent of antecedent support)"
set xlabel "c_prior"
set ylabel "c_post"
set zlabel "d_diff"
splot diff(x,y)
pause -1 "Hit return to continue"

set title "d_quot (independent of antecedent support)"
set zlabel "d_quot"
splot quot(x,y)
pause -1 "Hit return to continue"

set zlabel "d_info"
set title "d_info with antecedent support 0.2"
splot info(x,y,0.2)
pause -1 "Hit return to continue"

set title "d_info with antecedent support 0.3"
splot info(x,y,0.3)
pause -1 "Hit return to continue"

set title "d_info with antecedent support 0.4"
splot info(x,y,0.4)
pause -1 "Hit return to continue"

set zlabel "d_chi^2"
set title "d_chi^2 with antecedent support 0.2"
splot chi2(x,y,0.2)
pause -1 "Hit return to continue"

set title "d_chi^2 with antecedent support 0.3"
splot chi2(x,y,0.3)
pause -1 "Hit return to continue"

set title "d_chi^2 with antecedent support 0.4"
splot chi2(x,y,0.4)
pause -1 "Hit return to continue"
