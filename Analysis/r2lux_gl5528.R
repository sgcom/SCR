# R function that numerically calculates the  
# approximate illuminance [lux] by resistance [kOhm]
# for LDR photoresistor based on relatively realistic guestimation
#
r2lux_gl5528 = function( resistance_kOhm ) {
  #abandoning this model#269.535 / resistance_kOhm^1.4306
  # below parameters are very approximately guesstimated based on achieving realistic result
  exp(7.15 - 1.95*log(resistance_kOhm))
  ##can be simplified
}

# Nclock count to Illuminance [lux] calculation.
# The model parameters here are very roughly guestimated.
# If precise measurements the system must be callibrated appropriately.
# Most applications targeted here do not rely on exact measurement, 
# but more on relative changes.
#
SystemKonstanT = 0.001 * 1.8271 # captures system-specifics- LDR, capacitor,...
n2lux_gl5528 = function( Ndis ) {
  exp(7.15 - 1.95*log(SystemKonstanT*Ndis))
  #can be simplified
}
