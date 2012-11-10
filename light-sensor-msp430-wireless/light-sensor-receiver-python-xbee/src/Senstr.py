#!/usr/bin/python

'''
Working Prototype for wireless sensor network receiver.
Part of future Wireless Sensor Network Base Station.
'''

import time
import serial
import signal
import sys
from Queue import Queue
from xbee import ZigBee
#
from _Config import Config
from _Nodes import SensorNodes
from Utils import toHexString
from threading import Lock, Thread
from Logr import Logr


####################################################################################


class XbeeDescriptor(object):

    def __init__(self):
        self.address16 = "N/A"
        self.address64 = "N/A"
        self.parent    = "N/A"
        self.nodeId    = "N/A"
        self.deviceType= "N/A"
        self.signalDB  = -999
        self.nodePtr   = None
    #

    def getXbeeDeviceTypeDescr(self):
        if self.deviceType == 0:
            return "coordinator"
        elif self.deviceType == 1:
            return "router"
        else:
            return "end node"
    #

####################################################################################

class Coordinatr(Thread):
    
    def __init__(self, loggr):
        Thread.__init__(self)
        self.daemon = True
        self.logr = logr
        self.RxQ = Queue()
        self.terminating = False
        self.FrameIdSequence = 0
        self.frameIdSequenceLock = Lock()

    def debug(self, msg):
        logr.debug("Coordinatr: " + str(msg))

    def dataReceived(self, dataframe):
        '''The XBee API calls this function when new data packet is received.'''
        self.RxQ.put(dataframe)

    def tick(self):
        pass #TODO
        
    def run(self):
        while not self.terminating:
            response = self.RxQ.get(True)
            frameId = response.get('id')
            if frameId == 'at_response':
                pass #TODO
            elif frameId == 'rx':
                ''' Received RX Frame '''
                remoteAddress64 = toHexString(response['source_addr_long'])
                remoteAddress16 = toHexString(response['source_addr'])
                packet_type = ord(response['rf_data'][0])
                self.debug('rcvd RX frame: [addr64|addr16|packet_type]: [ ' + remoteAddress64 + " | " + remoteAddress16 + " | " + hex(packet_type) + ' ]')
                if packet_type == Config.PACKET_PAYLOAD:
                    ''' Received PAYLOAD Data from a sensor node '''
                    if SensorNodes.repo.has_key(remoteAddress64):
                        ''' Found the node by address '''
                        node = SensorNodes.repo.get(remoteAddress64)
                        node.consumeRxData(remoteAddress16, response['rf_data'])
                    #
                #end if PAYLOAD
            #end zbDataReceived
            pass

    def getNextFrameId(self, anode):
        self.frameIdSequenceLock.acquire()
        self.FrameIdSequence += 1
        if self.FrameIdSequence > 255:
            self.FrameIdSequence = 1
        self.frameIdSequenceLock.release()
        return chr(self.FrameIdSequence)
    
    
####################################################################################


SerialDevice = serial.Serial(Config.PORT, Config.BAUD_RATE)
SensorNodes()
logr = Logr()
logr.start()
coordinatr = Coordinatr(logr)
xbee = ZigBee(SerialDevice, True, coordinatr.dataReceived, True )
coordinatr.xbee = xbee
coordinatr.start()


def debug(msg):
    logr.debug("main: " + str(msg))


def run_main():
    global xbee
    debug("Senstr Started...")
    while True:
        time.sleep(120)
        coordinatr.tick()
    #


def doTerminate():
    logr.terminating = True
    coordinatr.terminating = True
    xbee.halt()
    SerialDevice.close()
    time.sleep(1)
    logr._Thread__stop()
    coordinatr._Thread__stop()
    time.sleep(2)
    sys.exit(0)


def signal_handler(signal, f):
    debug("signal_handler: Got signal to terminate: " + str(signal))
    doTerminate()


if __name__ == '__main__':
    try:
        signal.signal(signal.SIGTERM, signal_handler)
        signal.signal(signal.SIGABRT, signal_handler)
        signal.signal(signal.SIGINT, signal_handler)
        run_main()
    finally:
        doTerminate()
    #end main
