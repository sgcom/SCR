#
# For Photoresistor (LDR) GL5528
#
# Numerically working out the approximate illuminance vs. Resistance response
#

# 3 points taken from the graph on the data sheet, approximately
#
vlx = c(1,10,100)   # illuminance
vrx = c(47,10,2.3)  # resistance in kOhm
#
# converting to log-log scale
#
lvlx = log(vlx)
lvrx = log(vrx)
#
#
plot(lvlx,lvrx)

# log-linear model
#
lm(lvrx~lvlx)
#
#Call:
#  lm(formula = lvrx ~ lvlx)
#
#Coefficients:
#  (Intercept)         lvlx  
#     3.8372      -0.6552  
#
# The illuminanse [lux] can be approximated as
# LX = exp((1/0.6552)*(3.8372-log(R)))
# where R is in kOhm

# log-log plot from the model
#
R = seq(.1,100,.1)
LX = exp((1/0.6552)*(3.8372-log(R)))
plot(LX~R, type='l', log = "xy")
grid()

# The formula can be simplified to 
#     Lx_R = @(R)exp(9593/1638)/R^(1250/819)   # Matlab notation
# or
#     Lx_R = @(R)exp(5.8565)/R^1.5263          # Matlab notation

#
# Or as R function:
#
r2lux = function( resistance_kOhm) {
  # "converts" the LDR resistance [kOhm] to illuminance [lx]
  # for photoresistor GL5529, ~ 10k @ 10 Lx
  exp(9593/1638)/resistance_kOhm^(1250/819)
}


# Again the log-log plot from the approx. model, now using the R function
#
R2 = seq(.1,100,.2)
LX2 = r2lux(R2)
plot(LX2~R2, type='l', log = "xy")
grid()
#


############################################################################################
# Another simpler way to work it out numerically, if GAMMA is given (and correct)
#
# In this case gamma = 0.7 = log10(r10/r100)
# where r10 and r100 is the resistance at 10 and 100 lux.
#
# if r10 = 15k; r100 = 15/10^0.7

r100 = 10/10^0.7

# r100 is approx. 3k; then:

vlx = c(100,10)   # illuminance [Lx]
vrx = c(3,15)     # resistance [kOhm]


#
# converting to log-log scale
#
lvlx = log10(vlx)
lvrx = log10(vrx)

# log-linear nmodel
#
lm(lvrx~lvlx)

#  (Intercept)         lvlx  
#1.875       -0.699  

# Matlab
# syms R
# R2L = 10^((1/0.699)*(1.875-log10(R)))
# simplify(R2L)
# ans = (100*10^(159/233))/R^(1000/699)
#    ~=  481.2862 / R^0.6824

# R function, worked out assuming 
# gamma = 0.7 and R10 = 10K
#
r2lux = function( resistance_kOhm ) {
  # "converts" the LDR resistance [kOhm] to illuminance [Lux]
  # for photoresistor GL5529, ~ 15k @ 10 Lx, gamma = 0.7
  10^((1/0.699)*(1.875-log10(resistance_kOhm)))
  #481.2862 / resistance_kOhm^0.6824
}

# If the numerical reverse-engineering is correct, the following 
# plot should be very similar to the one from the data sheet
R3 = seq(1,50,1)
LX3 = r2lux(R3)
plot(R3~LX3, type='l', log = "xy", main="Illuminance - Resistance response for LDR GL5528", ylab="Resistance [kOhm]", xlab="Illuminance [lux]")
grid()



############################################################################################
# Same as above but using natural logarithm
#
# gamma = 0.7 = log(r10/r100) / log(10)
# where r10 and r100 is the resistance at 10 and 100 lux.

# r100 is approx. 2k; then:

vlx = c(100,10)   # illuminance [Lx]
vrx = c(3,15)     # resistance [kOhm]


#
# converting to log-log scale
#
lvlx = log(vlx)
lvrx = log(vrx)

# log-linear nmodel
#
lm(lvlx~lvrx)

#(Intercept)         lvlx  
#-    5.597       -1.431  
#     6.177       -1.431 

r2lux = function( resistance_kOhm ) {
  exp(6.177 - 1.431*log(resistance_kOhm))
}

###########################################################################################

K = 0.001 * 1.8271
n2lux = function( Ndis ) {
  #exp(6.177 - 1.431*log(resistance_kOhm))
  # here mostly ignorring the data sheet and setting common sense gestimation parameters
  # trying to achieve realistic calculated result
  # direct sun: ~30k-100k lux, dark night: ~ e-3 .. e-4
  exp(7.15 - 1.95*log(K*Ndis))
  #-> exp(73/10)/R^(19/10)
}

R3 = seq(1,150,1)
LX3 = r2lux(R3)
plot(R3~LX3, type='l', log = "xy", main="Illuminance - Resistance response for LDR GL5528", ylab="Resistance [kOhm]", xlab="Illuminance [lux]")
grid()



