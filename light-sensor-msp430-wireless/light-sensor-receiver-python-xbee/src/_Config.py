#
# ENVIRONMENT-SPECIFIC file
#
class Config(object):
    '''
    ENVIRONMENT-SPECIFIC configuration. Modify as needed.
    '''
              
    ''' Environment-specific '''
    PORT        = 'COM17'
    BAUD_RATE   = 57600
    DATA_DIR    = '/opt/GitHub/SCR/light-sensor-msp430-wireless/data'
    STATUS_DIR  = '#not used in this simplified version#'
#   LOG_FILE    = 'TODO'
    
    ''' Application-specific '''
    LSA_HEADER_SIZE     = 3
    LSA_DATA_MODALITY   = 6
    PACKET_PING         = 0x01
    PACKET_PAYLOAD      = 0x10
    DEFAULT_DATE_FORMAT = '%Y-%m-%d'
    DEFAULT_TIME_FORMAT = '%Y-%m-%d %H:%M:%S'

    DEBUG               = True
