
from _Config import Config
from Utils import currentDateTime

class RemoteXB_Node(object):
    '''
    Remote XBee Node - Base class.    
    '''
    
    def __init__(self, shortID, descr, dataDir, statusDir):
        self.address64       = 'N/A'
        self.address16       = 'N/A'
        self.parentAddress16 = 'N/A'
        self.xbeeNodeId      = 'N/A'
        self.xbeeDevType     = 'N/A'
        self.shortID         = shortID
        self.nodeDescription = descr
        self.dataDir         = dataDir
        self.dataFileName    = "FileName-NotSet"
        self.statusDir   = statusDir
        self.lastATDBResponse= 0
        pass
    
    
    def setRemoteAddress64(self, value):
        self.address64 = value

    
    def setLastATDBResponse(self, value):
        self.lastATDBResponse = value
        
            
    def writeDataLines(self, lines):
        dateStr = currentDateTime(Config.DEFAULT_DATE_FORMAT)
        self.dataFileName = "data_%s_%s.csv" % (self.shortID, dateStr)
        fullDataFileName = "%s/%s" % (self.dataDir, self.dataFileName)
        dfile = open(fullDataFileName, 'a')
        dfile.writelines(lines)
        dfile.close()

