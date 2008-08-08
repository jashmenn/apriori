#-----------------------------------------------------------------------
# File    : c_rev.gp
# Contents: visualization of the dependence between posterior confidence
#           and reversed confidence, i.e. the confidence of the reversed
#           rule antecedent <- consequent
# Author  : Christian Borgelt
# History : 23.09.1998 file created
#-----------------------------------------------------------------------
# set terminal postscript

set view 50,330,1
set xrange [0.001:0.999]
set yrange [0.001:0.999]
set zrange [0:1]
set isosamples 20
set hidden3d

set xlabel "c_post"
set ylabel "c_rev"

min(a,b) = a < b ? a : b

diff(x,y,p) = (1 -p -(y*p)/x +y*p >= 0) ? abs(x -p) : 0

quot(x,y,p) = (1 -p -(y*p)/x +y*p >= 0) ? 1 -min(x/p,p/x) : 0

i(x,y,p)    = (y*p*log((y*p)/(p*(y*p)/x)) \
            + (p-y*p)*log((p-y*p)/(p*(1-(y*p)/x))) \
            + ((y*p)/x-y*p)*log(((y*p)/x-y*p)/((1-p)*(y*p)/x)) \
            + (1-p-(y*p)/x+y*p)*log((1-p-(y*p)/x+y*p) \
                                  /((1-p)*(1-(y*p)/x)))) /log(2)

info(x,y,p) = (1 -p -(y*p)/x +y*p >= 0) ? i(x,y,p) : 0

chi2(x,y,p) = (1 -p -(y*p)/x +y*p >= 0) \
            ? (p*((y*p)/x)-y*p)**2 /(p*(1-p)*((y*p)/x)*(1-(y*p)/x)) : 0

set zlabel "d_diff"
set title "prior confidence 0.2"
splot diff(x,y,0.2)
pause -1 "Hit return to continue"

set title "prior confidence 0.3"
splot diff(x,y,0.3)
pause -1 "Hit return to continue"

set title "prior confidence 0.4"
splot diff(x,y,0.4)
pause -1 "Hit return to continue"

set zlabel "d_quot"
set title "prior confidence 0.2"
splot quot(x,y,0.2)
pause -1 "Hit return to continue"

set title "prior confidence 0.3"
splot quot(x,y,0.3)
pause -1 "Hit return to continue"

set title "prior confidence 0.4"
splot quot(x,y,0.4)
pause -1 "Hit return to continue"

set zlabel "d_info"
set title "prior confidence 0.2"
splot info(x,y,0.2)
pause -1 "Hit return to continue"

set title "prior confidence 0.3"
splot info(x,y,0.3)
pause -1 "Hit return to continue"

set title "prior confidence 0.4"
splot info(x,y,0.4)
pause -1 "Hit return to continue"

set zlabel "d_chi^2"
set title "prior confidence 0.2"
splot chi2(x,y,0.2)
pause -1 "Hit return to continue"

set title "prior confidence 0.3"
splot chi2(x,y,0.3)
pause -1 "Hit return to continue"

set title "prior confidence 0.4"
splot chi2(x,y,0.4)
pause -1 "Hit return to continue"

