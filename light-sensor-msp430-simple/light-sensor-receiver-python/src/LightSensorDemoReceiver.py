'''
Light Sensor Demo Receiver

Examples for typical illuminance calues from Wikipedia:

http://en.wikipedia.org/wiki/Lux

Illuminance         Surfaces illuminated by:

0.000n lux          Moonless, overcast night sky (starlight)
0.002 lux           Moonless clear night sky with airglow
0.27-1.0 lux        Full moon on a clear night
3.4 lux             Dark limit of civil twilight under a clear sky
50 lux              Family living room lights (Australia, 1998)
80 lux              Office building hallway/toilet lighting
100 lux             Very dark overcast day
320-500 lux         Office lighting
400 lux             Sunrise or sunset on a clear day.
1,000 lux           Overcast day;[2] typical TV studio lighting
10,000-25,000 lux   Full daylight (not direct sun)
32,000-130,000 lux  Direct sunlight

'''

import serial
import math

serialPort = "COM10"             # set your serial port here
SpecialKonstant = 0.001 * 1.8271 #system dependent constant


def n2lux(Ndis):
    'Calculates the illuminance from the count as received from the sensor node'
    return math.exp(7.15 - 1.95*math.log(SpecialKonstant*Ndis))


def receive():
    'Reads data from the sensor node and prints it to std out in CSV format'
    synCnt = 0
    isSync = False
    byteCnt = 0
    numberOfDataBytes = 0
    ser = serial.Serial(serialPort, 9600)
    print ' "Ndis", "ApproxLux"'
    while True:
        try:
            ch = ser.read(1)
            if ord(ch) == 0xff:
                synCnt += 1
                if synCnt == 3:
                    isSync = True
                    byteCnt = 0
                    continue
                #
            if isSync:
                byteCnt += 1
                if byteCnt == 1:
                    pass #seconds between samples ignored at this time
                elif byteCnt == 2:
                    numberOfDataBytes = ord(ch)
                    while numberOfDataBytes > 0:
                        N_discharge = 0
                        N_discharge |= ord(ser.read(1)); N_discharge <<= 8; numberOfDataBytes -= 1
                        N_discharge |= ord(ser.read(1)); N_discharge <<= 8; numberOfDataBytes -= 1
                        N_discharge |= ord(ser.read(1)); numberOfDataBytes -= 1
                        Lux2 = n2lux(N_discharge)
                        print "%6d, %10.4f" % (N_discharge, Lux2)
                    #end while numberOfDataBytes > 0
                    synCnt = 0
                    isSync = False
                #end if/elif byteCnt
            #end if isSync
        except KeyboardInterrupt:
            break
    # end while True
    ser.close()
    pass


if __name__ == '__main__':
    receive()
