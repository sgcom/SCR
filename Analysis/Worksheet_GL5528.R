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
# r10 = 10k; r100 = 10/10^0.7

r100 = 10/10^0.7

# r100 is approx. 2k; then:

vlx = c(100,10)   # illuminance [Lx]
vrx = c(2,10)     # resistance [kOhm]


#
# converting to log-log scale
#
lvlx = log10(vlx)
lvrx = log10(vrx)

# log-linear nmodel
#
lm(lvrx~lvlx)

#  (Intercept)         lvlx  
#1.699       -0.699  

# Matlab
# syms R
# R2L = 10^((1/0.699)*(1.699-log10(R)))
# simplify(R2L)
# ans = (100*10^(301/699))/R^(1000/699)
#    ~=  269.535 / R^1.4306

# R function, worked out assuming 
# gamma = 0.7 and R10 = 10K
#
r2lux = function( resistance_kOhm ) {
  # "converts" the LDR resistance [kOhm] to illuminance [Lux]
  # for photoresistor GL5529, ~ 10k @ 10 Lx, gamma = 0.7
  269.535 / resistance_kOhm^1.4306
}

# If the numerical reverse-engineering is correct, the following 
# plot should be very similar to the one from the data sheet
R3 = seq(1,150,1)
LX3 = r2lux(R3)
plot(R3~LX3, type='l', log = "xy", main="Illuminance - Resistance response for LDR GL5528", ylab="Resistance [kOhm]", xlab="Illuminance [lux]")
grid()

