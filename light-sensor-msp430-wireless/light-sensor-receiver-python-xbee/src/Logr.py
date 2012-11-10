''' 
Simplified debug logging to std output
TODO Real Logging ...
'''

import time
from threading import Thread
from Queue import Queue
from _Config import Config


class Logr(Thread):
    
    def __init__(self):
        Thread.__init__(self)
        self.daemon = True
        self.LogQ = Queue(32)
        self.terminating = False
        
    def debug(self, msg):
        if Config.DEBUG:
            self.LogQ.put( "%s - %s" % (time.strftime("%Y-%m-%d %H:%M:%S",time.localtime()) , msg) )

    def run(self):
        ''' TODO Real Logging '''
        while not self.terminating:
            msg = self.LogQ.get(True)
            if self.terminating:
                break
            print msg
