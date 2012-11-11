#
# help-functions.R
# Part of the "Hours of Sunlight" project.
#

library(ggplot2)
library(scales)
library(zoo)
library(gridExtra)

SunThreshold = 32.5

# for using gridExtra see: http://www.imachordata.com/?p=730

# Converts the raw "light measurements" to a more convenient dimensionless 
# log-scale measure to emphasize the high-light range of values - because 
# now the interest is to catch the direct sunlight and distinguish between 
# direct sunlight and shadow.
#
as.light.index = function(light) {
  return ( (20.0-log2(light))^2/5.0 )
}

# sum(diff(x))
#
#sumdif = function(x) {
#  return( sum(diff(x)) )
#}

#
# Reads the sensor data from CSV file and returns data-frame
#
get.data = function( dataFileName ) {
  lsd = read.csv(dataFileName, header=F)
  names(lsd) = c("time", "light", "battery", "temp")
  lsd$time = as.POSIXct(lsd$time)
  lsd$light.index = as.light.index(lsd$light)
  lsd = subset(lsd, select=-battery) # drop the battery voltage; it is regulated now
  lsd$sun = lsd$light.index > SunThreshold   # enrich the data by adding direct sunlight boolean
  #lsd$sun[lsd$light.index > SunThreshold] = 50.0
  lsd
}

#
# Reads the sensor data from CSV file and zoo object
#
get.data.as.zoo = function( dataFileName, smoothed=F ) {
  z = read.zoo(dataFileName, sep = ",", header=F, FUN=as.POSIXct)
  names(z) = c("light", "battery", "temp")
  z$light = as.light.index(z$light)
  z = subset(z, select=-battery) # drop the battery voltage; it is regulated now
  if(smoothed) { 
    z = rollapply(z, 15, mean) 
  }
  z
}

#
# Plot type 1 - light over time, color by device temperature.
#             - note that the device gets hotter under direct sunlight
#
plot.type.1 = function(dataframe, setname) {
  # -------+ Assuming unimodal period of sunlight.
  # WARNING: this is not going to work as is otherwise
  # -------+ Needs more work to make it valid in a more general case.
  startSunTime = min(dataframe$time[dataframe$light.index > SunThreshold])
  endSunTime = max(dataframe$time[dataframe$light.index > SunThreshold])
  sunTime = endSunTime - startSunTime
  formattedSunTime = format(sunTime, digits = 2)
  ggplot(dataframe, 
         aes(x=time, y=light.index, color=temp)) + 
         #aes(x=time, y=light.index)) +  
         geom_point() + geom_line() + 
         scale_x_datetime(breaks=date_breaks("2 hours"),labels=date_format("%H:00")) + 
         scale_colour_gradientn(colours = heat.colors(3,alpha=0.1)) +
         annotate("rect", xmin = startSunTime, xmax = endSunTime, ymin = 0, ymax = 50, colour="red", fill="orange", alpha = .1) +
         ggtitle( paste(formattedSunTime, "of sun at location", setname) )
  #annotate("rect", xmin = min(dataframe$time[dataframe$light.index>SunThreshold]), xmax = max(dataframe$time[dataframe$light.index>SunThreshold]), ymin = 0, ymax = 50, colour="red", fill="orange", alpha = .1)
  #annotate("text", x = min(dataframe$time[dataframe$light.index>SunThreshold]), y = 10, label = " Direct Sunlight")
         #+ geom_line(aes(y=sun))
}

plot.type.1m = function(dataframes) {
  codenames = names(dataframes)
  plots = list()
  for( i in seq_along(dataframes) ) {
    plots[[i]] = plot.type.1(dataframes[[i]], codenames[i])
  }
  #grid.arrange(plots[[1]], plots[[2]], ncol=1)
  do.call(grid.arrange,  plots)
}

#
# Plot type 2 - light and temperature over time;
#             - using the zoo package
#
plot.type.2 = function(data) {
  plot(data, plot.type="single", col=c("blue2", "orange"), lwd=1)
}



#some other possible plots using ggplot2
#
#simple light-time
#ggplot(lsd, aes(x=time, y=light.index)) + geom_line() + scale_x_datetime(breaks=date_breaks("2 hours"),labels=date_format("%H:00")) 
# temp - time
#ggplot(lsd, aes(x=time, y=temp, color=temp)) + geom_point() + geom_line() + scale_x_datetime(breaks=date_breaks("2 hours"),labels=date_format("%H:00")) + scale_colour_gradientn(colours = heat.colors(3,alpha=0.5))
# temp-light phase space
#ggplot(lsd, aes(x=temp, y=light.index, color = time)) + geom_point()

