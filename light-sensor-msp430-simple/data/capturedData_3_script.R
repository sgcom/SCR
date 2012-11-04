#
# data captured using the simpel msp430 implementation of light sensor
# first part is ambient light in a room, slightly decreasing.
# Then the sensor was moved under a fluorescent office lamp.
#

folder = "C:/opt/GitHub/SCR/light-sensor-msp430-simple/light-sensor-receiver-python/data"
setwd(folder)
dataset = read.csv("capturedData_3.csv", header=T)

# as timeseries
light.ts = ts(dataset[,2], start=0, deltat=5)
#
# smoothed
light.ts.f1 = filter(light.ts, filter=rep(1/7,7))
#
# plot the raw measurements and the smoothed series on top of it
#
plot(light.ts, 
     type='l', 
     xlab="time [sec]", 
     ylab="light illuminance", 
     main="Light sensor data sample 3", 
     col=hsv(0,1,1,.66))
lines(light.ts.f1, 
      type='l', 
      lty=4, 
      col=hsv(.7,1,1,.66))

#lines(light.ts.f1, type='l', lty=4, lwd=1, xlab="time [sec]", ylab="light illuminance", main="Light sensor data - smoothed", col=hsv(.7,1,1,.66))
