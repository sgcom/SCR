# R function that numerically calculates the  
# approximate illuminance [lux] by resistance [kOhm]
# for LDR photoresistor GL5528, ~ 10k @ 10 Lx, gamma = 0.7
#
r2lux_gl5528 = function( resistance_kOhm ) {
  269.535 / resistance_kOhm^1.4306
}
