#
# data captured using the simpel msp430 implementation of light sensor
# first part is ambient light in a room, slightly decreasing.
# Then the sensor was moved under a fluorescent office lamp.
#

folder = "C:/opt/GitHub/SCR/light-sensor-msp430-simple/light-sensor-receiver-python/data"
setwd(folder)
dataset = read.csv("capturedData_3.csv", header=T)
head(dataset)
#light = dataset[,2]
light.ts = ts(dataset[,2], start=0, frequency=5)
plot(light.ts, type='l', xlab="time [sec]", ylab="light illuminance", main="Light sensor data sample 3", col=2)

#
# smoothed 
#
light.ts.f1 = filter(light.ts, filter=rep(1/7,7))
lines(light.ts.f1, type='l', xlab="time [sec]", ylab="light illuminance", main="Light sensor data - smoothed", col=4)
