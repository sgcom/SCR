#
# ENVIRONMENT-SPECIFIC file
#

from _Config import Config
from LightSensorA_Node import LightSensorA_Node

class SensorNodes(object):
    '''
    ENVIRONMENT-SPECIFIC Nodes Repository
    TODO Short ID is not used - remove it    
    '''
    repo = {
    
#        long address                                Short ID  Description            Data DIR         STATUS DIR
#        -----------------------                     --------  ---------------        --------------   -----------------
        '00.13.a2.00.40.7e.7b.cc': LightSensorA_Node('A',     'Light Sensor Model I', Config.DATA_DIR, Config.STATUS_DIR    )
    }
    
    def __init__(self):
        for k in SensorNodes.repo.keys():
            SensorNodes.repo.get(k).setRemoteAddress64(k)
        #end __init__
        