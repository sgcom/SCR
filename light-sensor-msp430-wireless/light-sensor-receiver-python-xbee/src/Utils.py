### UTILS
#
import time


def toHexString(s):
    "Converts to HEX String with dot-separator like 7f.10.00.8a..."
    if not s:
        return "None"
    lst = []
    if isinstance( s, int ):
        s = str(s)
    for ch in s:
        hv = hex(ord(ch)).replace('0x', '')
        if len(hv) == 1:
            hv = '0'+hv
        lst.append(hv)
    return reduce(lambda x,y:x+'.'+y, lst)


def list2csv(lst):
    st = ""
    i = 0
    for el in lst:
        if i > 0:
            st += ", "
        st += str(el)
        i += 1
    return st


def asFormattedListOfTime(tmList, timeFormat):
    fTimeLst = []
    size = len(tmList)
    for i in range(0, size, 1):
        tm = tmList[i]
        ptm = time.strptime(time.ctime(tm))
        ftm = time.strftime(timeFormat, ptm)
        fTimeLst.append(ftm)
    return fTimeLst


def currentDateTime(formatStr):
    return time.strftime(formatStr, time.localtime())

#
####
