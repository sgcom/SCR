'''
LightSensorA_Node.py
'''

import time
from RemoteXB_Node import RemoteXB_Node
from Utils import asFormattedListOfTime
from _Config import Config


class LightSensorA_Node(RemoteXB_Node):
    
    def __init__(self, shortID, nodeName, dataDir, statusDir):
        RemoteXB_Node.__init__(self, shortID, nodeName, dataDir, statusDir)
        self.timeBetweenMeasurements = 0
        self.timeBetweenEmissions = 0
        self.sensorData = []
        self.timeStamps = []
        self.batteryVoltage = []
        self.mcuTemperature = []
        self.lastTimeReceived = 0.0
        self.nMPS = 0


    def consumeRxData(self, remoteAddress16, dat):
        'This method is to be called immediately after a data frame is received from the sensor node. It parses and handles the payload.'
        tnow = time.time()
        self.lastTimeReceived = tnow
        self.address16 = remoteAddress16
        self.timeBetweenMeasurements = ord(dat[1])
        nCoreDataBytes = ord(dat[2])
        self.nMPS = nCoreDataBytes / Config.LSA_DATA_MODALITY
        self.timeBetweenEmissions = self.timeBetweenMeasurements * self.nMPS
        self.sensorData = []
        self.batteryVoltage = []
        self.mcuTemperature = []
        for i in range(0, nCoreDataBytes, Config.LSA_DATA_MODALITY):
            mv = ord(dat[Config.LSA_HEADER_SIZE+i])<<16 | ord(dat[Config.LSA_HEADER_SIZE+i+1])<<8 | ord(dat[Config.LSA_HEADER_SIZE+i+2])
            _bat= ord(dat[Config.LSA_HEADER_SIZE+i+3])<<8 | ord(dat[Config.LSA_HEADER_SIZE+i+4])
            bat = float(_bat / 100.0);
            tmp = ord(dat[Config.LSA_HEADER_SIZE+i+5])
            self.sensorData.append(mv)
            self.batteryVoltage.append(bat)
            self.mcuTemperature.append(tmp)
        self.timeStamps = []
        for i in range(self.nMPS-1, -1, -1):
            tm = tnow - i * self.timeBetweenMeasurements
            self.timeStamps.append(tm)
            pass
        RemoteXB_Node.writeDataLines(self, self.asCSVDataEntry())
        pass

    
    def asCSVDataEntry(self):
        'Converts the data to CSV format ready to be saved in CSV file. '
        lines = []
        fTimes = asFormattedListOfTime(self.timeStamps, Config.DEFAULT_TIME_FORMAT) 
        for i in range(0, self.nMPS, 1):
            lines.append( "%s, %d, %s, %s\n" % (fTimes[i], self.sensorData[i], self.batteryVoltage[i], self.mcuTemperature[i]) )
        return lines

    
    def __str__(self):
        "Converts the instance to string - to be used for debugging only."
        s = ""
        s += "Node: %s (%s)\n" % (self.nodeDescription, self.shortID)
        s += "address64: %s\n" % self.address64
        s += "address16: %s\n" % self.address16
        s += "lastTimeReceived: %s\n" % time.ctime(self.lastTimeReceived)
        s += "Last atDB response: %d\n" % self.lastATDBResponse
        s += "nMPS: %d\n" % self.nMPS
        s += "timeBetweenMeasurements: %d\n" % self.timeBetweenMeasurements
        s += "timeBetweenEmissions: %d, \n" % self.timeBetweenEmissions
        s += "sensorData: %s\n" % self.sensorData
        s += "timeStamps: %s\n" % asFormattedListOfTime(self.timeStamps, Config.DEFAULT_TIME_FORMAT)
        s += "batteryVoltage: %s\n" % self.batteryVoltage
        s += "mcuTemperature: %s" % self.mcuTemperature
        return s
