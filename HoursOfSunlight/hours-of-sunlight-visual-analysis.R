#
# hours-of-sunlight.R
#
# Visual analysis of the sunlight data captured on 3 different days
# 
#

source("help-functions.R")

#
# Sensor data recorded at 3 different location on 3 separaTE DAYS
#
dataFiles = c(
  "data/loc_FR5_2012-08-26.csv",
  "data/loc_FL2_2012-09-03.csv",
  "data/loc_FF3_2012-09-04.csv"
)
#
locationCodes = c("FR5", "FL2", "FF3")

dataSets = list()
zooSets = list()
for( i in seq_along(dataFiles) ) {
  dataSets[[i]] = get.data(dataFiles[i])
  zooSets[[i]] = get.data.as.zoo(dataFiles[i], T)
}
names(dataSets) = locationCodes

plot.type.1m(dataSets)


# another possible type of plot using zoo:
#
#plot.type.2(zooSets[[1]])
#
#########################################