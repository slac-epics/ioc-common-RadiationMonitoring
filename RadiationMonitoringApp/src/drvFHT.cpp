/*#==============================================================
#
#  Abs: C++ Source file for Thermo FHT driver
#
#  Name: drvFHT.cpp
#
#  Desc: This file should include drvFHT.h
#
#  Facility:  LCLS2
#
#  Auth: 21-Jul-2017, M. Dunning (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       dd-mmm-yyyy, Name           (USERNAME):
#          comment
#
#==============================================================*/

#include <cstdlib>
#include <cstring>

#include <epicsThread.h>
#include <epicsExport.h>
#include <epicsExit.h>
#include <asynOctetSyncIO.h>
#include <iocsh.h>
#include <errlog.h>
#include <alarm.h>
#include <dbAccessDefs.h>

#include "drvFHT.h"

const int MAX_CHAN = 16;
const double DEFAULT_TIMEOUT = 1.0;
const double DEFAULT_POLL_TIME = 2.0;
const int POLL_INT_MIN = 1;
const int POLL_INT_MAX = 60;

static const char *driverName = "drvFHT";

static void pollerThreadC(void * pPvt) {
    drvFHT *pdrvFHT = (drvFHT *)pPvt;
    pdrvFHT->pollerThread();
}

static void exitHandler(void *drvPvt);

/* Constructor for the drvFHT class */
drvFHT::drvFHT(const char *port, const char* IOport, int addr, double timeout) 
   : asynPortDriver(port, MAX_CHAN+1,
                    asynInt32Mask | asynFloat64Mask | asynOctetMask | asynDrvUserMask, /* Interface mask */
                    asynInt32Mask | asynFloat64Mask | asynOctetMask,  /* Interrupt mask */
                    ASYN_CANBLOCK | ASYN_MULTIDEVICE, /* asynFlags  */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0), /* Default stack size*/
                    deviceAddr_(addr),
                    timeout_(timeout),
                    pollTime_(DEFAULT_POLL_TIME),
                    forceCallback_(1),
                    running_(true),
                    exited_(false),
                    err_count_(0)
{
    const char *functionName = "drvFHT";
    asynStatus status;

    if (!timeout_) timeout_  = DEFAULT_TIMEOUT;

    status = pasynOctetSyncIO->connect(IOport, 0, &pasynUser, 0);
    
    if (status != asynSuccess) {
        epicsPrintf("%s::%s: Failed to connect to I/O port %s\n", driverName, functionName, IOport);
    }

    createParam(snString,                     asynParamOctet,          &sn);
    createParam(fwString,                     asynParamOctet,          &fw);
    createParam(devTypeString,                asynParamOctet,          &devType);
    createParam(devTimeString,                asynParamOctet,          &devTime);
    createParam(devAddrString,                asynParamInt32,          &devAddr);
    createParam(systemStatusString,           asynParamInt32,          &systemStatus);
    createParam(chanStateString,              asynParamInt32,          &chanState);
    createParam(chanModeString,               asynParamInt32,          &chanMode);
    createParam(chanChanString,               asynParamInt32,          &chanChan);
    createParam(chanUnitsString,              asynParamInt32,          &chanUnits);
    createParam(chanPreunitString,            asynParamInt32,          &chanPreunit);
    createParam(chanTypeString,               asynParamInt32,          &chanType);
    createParam(chanPortString,               asynParamInt32,          &chanPort);
    createParam(chanDecodeString,             asynParamInt32,          &chanDecode);
    createParam(chanAddressString,            asynParamInt32,          &chanAddress);
    createParam(chanDisplayFactorString,      asynParamFloat64,        &chanDisplayFactor);
    createParam(chanReadallString,            asynParamInt32,          &chanReadall);
    createParam(pollIntervalString,           asynParamInt32,          &pollInterval);
    createParam(measString,                   asynParamFloat64,        &meas);
    createParam(measStatusString,             asynParamInt32,          &measStatus);
    createParam(meas40GString,                asynParamFloat64,        &meas40G);
    createParam(measStatus40GString,          asynParamInt32,          &measStatus40G);
    createParam(measTime40GString,            asynParamFloat64,        &measTime40G);
    createParam(type40GString,                asynParamInt32,          &type40G);
    createParam(calFact40GString,             asynParamFloat64,        &calFact40G);
    createParam(deadTime40GString,            asynParamFloat64,        &deadTime40G);
    createParam(dynRange40GString,            asynParamFloat64,        &dynRange40G);
    createParam(swThresh40GString,            asynParamFloat64,        &swThresh40G);
    createParam(calDate40GString,             asynParamOctet,          &calDate40G);
    createParam(hwResetString,                asynParamInt32,          &hwReset);


    /* Start the thread to poll inputs and do callbacks to device support */
    epicsThreadId tid = epicsThreadCreate("drvFHTPoller",
                    epicsThreadPriorityMedium,
                    epicsThreadGetStackSize(epicsThreadStackMedium),
                    (EPICSTHREADFUNC)pollerThreadC, this);

    if (!tid) {
        epicsPrintf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
        return;
    }

    /* Create an EPICS exit handler */ 
    epicsAtExit(exitHandler, this);
}

drvFHT::~drvFHT() {
/*-----------------------------------------------------------
    Destructor.
--------------------------------------------------------------- */
    static const char *functionName = "~drvFHT";

    lock();
    running_ = false;
    unlock();

    while (!exited_) {
        epicsThreadSleep(0.2);
    }
    epicsPrintf("%s::%s Exiting...\n", driverName, functionName);
}

void drvFHT::pollerThread() {
/*-------------------------------------------------------------------- 
    This function runs in a separate thread.  
    Poll parameter values from device; pause for poll time. 
----------------------------------------------------------------------*/
    const char *functionName = "pollerThread";
    //asynStatus status;
    char cmdStr[128];
    int i;
    int pollInt;
    int readAll = 1;

    // Wait until iocInit is finished
    while (!interruptAccept) {
        epicsThreadSleep(0.2);
    }

    // Do these once    
    lock();
    // Serial number
    _readString(sn, "NR");
    // Firmware
    _readString(fw, "VR");
    // Device type
    _readString(devType, "DP");
    // Set device address
    setIntegerParam(devAddr, deviceAddr_);
    callParamCallbacks();
    unlock();

    // Poller loop
    while(running_) {
        lock();
        
        // System status
        _readStatus(systemStatus, "##");

        // Date and time from device
        _readString(devTime, "ZR");

        // Channel display mode; this is useless
        //_readChanMode("xR0");

        // Data polling interval
        _readInt(pollInterval, "yR", 0);
        getIntegerParam(pollInterval, &pollInt);

        // Channel config
        for(i=1; i<3; i++) {
            sprintf(cmdStr, "cR%d", i);
            _readChanConfig(cmdStr, i);
        }

        // Always read channels 1-2
        // Meas. value
        for(i=1; i<3; i++) {
            sprintf(cmdStr, "RM%d", i);
            _readChan(cmdStr, i);
        }

        // Channel state
        for(i=1; i<3; i++) {
            sprintf(cmdStr, "eR%d", i);
            _readInt(chanState, cmdStr, i);
        }

        // Channel units, display factor, pre-unit
        for(i=1; i<3; i++) {
            sprintf(cmdStr, "dR%d", i);
            _readUnits(cmdStr, i);
        }

        // Read channels 3-16?
        getIntegerParam(chanReadall, &readAll);

        if (readAll) {
            asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                    "Reading chans 3-16.\n");
            // Channel config
            for(i=3; i<MAX_CHAN+1; i++) {
                sprintf(cmdStr, "cR%d", i);
                _readChanConfig(cmdStr, i);
            }

            // Meas. value (read all channels)
            for(i=3; i<MAX_CHAN+1; i++) {
                sprintf(cmdStr, "RM%d", i);
                _readChan(cmdStr, i);
            }

            // Channel state (read all channels)
            for(i=3; i<MAX_CHAN+1; i++) {
                sprintf(cmdStr, "eR%d", i);
                _readInt(chanState, cmdStr, i);
            }

            // Channel units, display factor, pre-unit (read all channels)
            for(i=3; i<MAX_CHAN+1; i++) {
                sprintf(cmdStr, "dR%d", i);
                _readUnits(cmdStr, i);
            }
        }

        // Read 40G ports
        _readChan40G("MR1", 1);
        _readChan40G("MR2", 2);
        _readCal("KP1", 1);
        _readCal("KP2", 2);

        callParamCallbacks();
        unlock();

        // Poll at the same rate as the device if possible; otherwise use default
        epicsThreadSleep((pollInt >= 0)?pollInt:pollTime_);
    } // End of while loop

    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s::%s: Poller thread exiting ...\n", driverName, functionName);
    exited_= true;
}

unsigned char drvFHT::_checksumCalc(const char* str) {
/*-----------------------------------------------------------------------------
 * 8-bit checksum
 *---------------------------------------------------------------------------*/
    static const char *functionName = "_checksumCalc";
    unsigned char sum = 0;
    while (*str) {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: *str=%d\n", driverName, functionName, *str);
        sum += *str++;
    }
    sum %= 256;
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: checksum=%d\n", driverName, functionName, sum);
    return sum;
}

asynStatus drvFHT::_readString(int function, const char *cmd) {
/*-----------------------------------------------------------------------------
 * Write a command string to device, read response,
 * set value in parameter library, do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readString";
    asynStatus status = asynSuccess; 
    
    status = _writeRead(cmd);

    if (status) {
        // Got no response or got garbage; set alarm status to invalid
        setParamAlarmStatus(function, COMM_ALARM);
        setParamAlarmSeverity(function, INVALID_ALARM);
        callParamCallbacks();

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: function=%d, status=%d\n", 
            driverName, functionName, function, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, function=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, function, cmd, replyBuffer_);

    // Set alarm status to normal 
    setParamAlarmStatus(function, NO_ALARM);
    setParamAlarmSeverity(function, NO_ALARM);

    if (forceCallback_) setStringParam(function, replyBuffer_);
    callParamCallbacks();

    return status;
}
    
asynStatus drvFHT::_readInt(int function, const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Write a command string to device, read response,
 * set value in parameter library, do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readInt";
    asynStatus status = asynSuccess; 
    int stat = -1;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, function, COMM_ALARM);
        setParamAlarmSeverity(addr, function, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: function=%d, status=%d\n", 
            driverName, functionName, function, status);
    }
    
    stat = strtol(replyBuffer_, NULL, 10);

    // Set alarm status to normal 
    setParamAlarmStatus(addr, function, NO_ALARM);
    setParamAlarmSeverity(addr, function, NO_ALARM);

    if (forceCallback_) setIntegerParam(addr, function, stat);
    callParamCallbacks(addr);

    return status;
}
    
asynStatus drvFHT::_readStatus(int function, const char *cmd) {
/*-----------------------------------------------------------------------------
 * Get system status from device; response is a 4 digit hex number.
 * Convert the value to decimal and then set value in parameter library.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readStatus";
    asynStatus status = asynSuccess; 
    int stat = -1;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(function, COMM_ALARM);
        setParamAlarmSeverity(function, INVALID_ALARM);
        callParamCallbacks();

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: function=%d, status=%d\n", 
            driverName, functionName, function, status);
        return status;
    }
    
    stat = strtol(replyBuffer_, NULL, 16);

    // Set alarm status to normal 
    setParamAlarmStatus(function, NO_ALARM);
    setParamAlarmSeverity(function, NO_ALARM);

    if (forceCallback_) setIntegerParam(function, stat);
    callParamCallbacks();

    return status;
}
    
asynStatus drvFHT::_readChan(const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form xxx hhhh gggg; 
 * where xxx = measured value, hhhh is a 4-digit hex status word, gggg is the 
 * system status which we ignore because we're reading it already. 
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readChan";
    asynStatus status = asynSuccess; 
    double value = -1.0;
    int valStatus = -1;
    char *pch;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, meas, COMM_ALARM);
        setParamAlarmSeverity(addr, meas, INVALID_ALARM);
        setParamAlarmStatus(addr, measStatus, COMM_ALARM);
        setParamAlarmSeverity(addr, measStatus, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) value = atof(pch);
        if (forceCallback_) setDoubleParam(addr, meas, value);
        pch = strtok(NULL, " ");
        if (pch) valStatus = strtol(pch, NULL, 16);
        if (forceCallback_) setIntegerParam(addr, measStatus, valStatus);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(addr, meas, NO_ALARM);
    setParamAlarmSeverity(addr, meas, NO_ALARM);
    setParamAlarmStatus(addr, measStatus, NO_ALARM);
    setParamAlarmSeverity(addr, measStatus, NO_ALARM);

    callParamCallbacks(addr);

    return status;
}

asynStatus drvFHT::_readChan40G(const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form xxx hhhh yyy;
 * where xxx = mean value, hhhh is a 4-digit hex status word, 
 * yyy is the sample time.
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readChan";
    asynStatus status = asynSuccess; 
    double value = -1.0;
    int valStatus = -1;
    double sampleTime = -1.0;
    char *pch;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, meas40G, COMM_ALARM);
        setParamAlarmSeverity(addr, meas40G, INVALID_ALARM);
        setParamAlarmStatus(addr, measStatus40G, COMM_ALARM);
        setParamAlarmSeverity(addr, measStatus40G, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) value = atof(pch);
        if (forceCallback_) setDoubleParam(addr, meas40G, value);
        pch = strtok(NULL, " ");
        if (pch) valStatus = strtol(pch, NULL, 16);
        if (forceCallback_) setIntegerParam(addr, measStatus40G, valStatus);
        pch = strtok(NULL, " ");
        if (pch) sampleTime = atof(pch);
        if (forceCallback_) setDoubleParam(addr, measTime40G, sampleTime);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(addr, meas40G, NO_ALARM);
    setParamAlarmSeverity(addr, meas40G, NO_ALARM);
    setParamAlarmStatus(addr, measStatus40G, NO_ALARM);
    setParamAlarmSeverity(addr, measStatus40G, NO_ALARM);

    callParamCallbacks(addr);

    return status;
}

asynStatus drvFHT::_readChanMode(const char *cmd) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form n mm;
 * where n is the channel mode (0 or 1), 
 * mm is the channel.
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readChanMode";
    asynStatus status = asynSuccess; 
    int mode = 0;
    int chan = 0;
    char *pch;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(chanMode, COMM_ALARM);
        setParamAlarmSeverity(chanMode, INVALID_ALARM);
        setParamAlarmStatus(chanChan, COMM_ALARM);
        setParamAlarmSeverity(chanChan, INVALID_ALARM);
        callParamCallbacks();

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) mode = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(chanMode, mode);
        pch = strtok(NULL, " ");
        if (pch) chan = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(chanChan, chan);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(chanMode, NO_ALARM);
    setParamAlarmSeverity(chanMode, NO_ALARM);
    setParamAlarmStatus(chanChan, NO_ALARM);
    setParamAlarmSeverity(chanChan, NO_ALARM);

    callParamCallbacks();

    return status;
}

asynStatus drvFHT::_readUnits(const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form nn xxx o;
 * where n is the unit number (integer), 
 * xxx is the display factor for the local display (float),
 * o is the preunit value (integer).
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readUnits";
    asynStatus status = asynSuccess; 
    double displayFactor = -1.0;
    int unit = -1;
    int preunit = -1;
    char *pch;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, chanUnits, COMM_ALARM);
        setParamAlarmSeverity(addr, chanUnits, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) unit = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanUnits, unit);
        pch = strtok(NULL, " ");
        if (pch) displayFactor = strtod(pch, NULL);
        if (forceCallback_) setDoubleParam(addr, chanDisplayFactor, displayFactor);
        pch = strtok(NULL, " ");
        if (pch) preunit = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanPreunit, preunit);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(addr, chanUnits, NO_ALARM);
    setParamAlarmSeverity(addr, chanUnits, NO_ALARM);

    callParamCallbacks(addr);

    return status;
}

asynStatus drvFHT::_readChanConfig(const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form nn mm oo pp; where:
 * nn is the probe type (integer), 
 * mm is the port ? (integer),
 * oo is the decode ? (integer).
 * pp is the address ? (integer).
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readChanConfig";
    asynStatus status = asynSuccess; 
    int type = -1;
    int port = -1;
    int decode = -1;
    int address = -1;
    char *pch;
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, chanType, COMM_ALARM);
        setParamAlarmSeverity(addr, chanType, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) type = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanType, type);
        pch = strtok(NULL, " ");
        if (pch) port = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanPort, port);
        pch = strtok(NULL, " ");
        if (pch) decode = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanDecode, decode);
        pch = strtok(NULL, " ");
        if (pch) address = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, chanAddress, address);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(addr, chanType, NO_ALARM);
    setParamAlarmSeverity(addr, chanType, NO_ALARM);

    callParamCallbacks(addr);

    return status;
}

asynStatus drvFHT::_readCal(const char *cmd, int addr) {
/*-----------------------------------------------------------------------------
 * Read value from a channel; response is of the form nn xx yy zz aa ss; where:
 * nn is the probe type ? (integer), 
 * xx is the calibration factor (float),
 * yy is the dead time ? (float).
 * zz is the dynamic range (float).
 * aa is the switching threshold (float).
 * ss is the cal. date (string).
 * Then set value in parameter library & do callback.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_readCal";
    asynStatus status = asynSuccess; 
    int type = -1;
    double cf = 0.0;
    double dt = 0.0;
    double dr = 0.0;
    double st = 0.0;
    char *pch;
    char date[64];
    
    status = _writeRead(cmd);
    if (status) {
        setParamAlarmStatus(addr, type40G, COMM_ALARM);
        setParamAlarmSeverity(addr, type40G, INVALID_ALARM);
        callParamCallbacks(addr);

        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW, 
            "%s:%s: ERROR: status=%d\n", 
            driverName, functionName, status);
        return status;
    }
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, cmd=%s, replyBuffer_=%s\n", 
            driverName, functionName, status, cmd, replyBuffer_);
   
    if (strchr(replyBuffer_, ' ')) {
        pch = strtok(replyBuffer_, " ");
        if (pch) type = strtol(pch, NULL, 10);
        if (forceCallback_) setIntegerParam(addr, type40G, type);
        pch = strtok(NULL, " ");
        if (pch) cf = strtod(pch, NULL);
        if (forceCallback_) setDoubleParam(addr, calFact40G, cf);
        pch = strtok(NULL, " ");
        if (pch) dt = strtod(pch, NULL);
        if (forceCallback_) setDoubleParam(addr, deadTime40G, dt);
        pch = strtok(NULL, " ");
        if (pch) dr = strtod(pch, NULL);
        if (forceCallback_) setDoubleParam(addr, dynRange40G, dr);
        pch = strtok(NULL, " ");
        if (pch) st = strtod(pch, NULL);
        if (forceCallback_) setDoubleParam(addr, swThresh40G, dt);
        pch = strtok(NULL, " ");
        if (pch) strcpy(date, pch);
        if (forceCallback_) setStringParam(addr, calDate40G, date);
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Unrecognized command %s\n", driverName, functionName, cmd);
    }
 
    // Set alarm status to normal 
    setParamAlarmStatus(addr, type40G, NO_ALARM);
    setParamAlarmSeverity(addr, type40G, NO_ALARM);

    callParamCallbacks(addr);

    return status;
}

asynStatus drvFHT::_write(const char *buffer) {
/*-----------------------------------------------------------------------------
 * Writes a string to device.
 * Format should be: <BEL><addr><command><checksum><ETX>, where:
 * <BEL> = ASCII code 7, 
 * <addr> = [01:99],
 * <command> is remote command, 
 * <checksum> = mod 256 checksum encoded as 2-digit hex number,
 * <ETX> = ASCII code 3.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_write";
    asynStatus status = asynSuccess; 
    size_t nbytesTransfered;
    unsigned char checksum;
    char tmpStr[128];
    char cmdBuffer[128];

    /* Construct command string with start character, address, command, checksum, 
        and terminator */
    sprintf(tmpStr, "%s%02d%s", "\x07", deviceAddr_, buffer);
    checksum = _checksumCalc(tmpStr);
    sprintf(cmdBuffer, "%s%02X", tmpStr, checksum);

    // Set output terminator
    status = pasynOctetSyncIO->setOutputEos(pasynUser, "\x03", 1); 

    status = pasynOctetSyncIO->flush(pasynUser);
    status = pasynOctetSyncIO->write(pasynUser, cmdBuffer, strlen(cmdBuffer), timeout_, &nbytesTransfered);

    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: status=%d, buffer=%s, nbytesTrans=%d\n", driverName, 
            functionName, status, cmdBuffer, (int)nbytesTransfered);

    if (status) {
        // Print an error message if this is the first error        
        if (!err_count_) {
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
              "%s::%s: Error: buffer=%s, status=%d, nbytesTransfered=%d\n",
              driverName, functionName, cmdBuffer, status, (int)nbytesTransfered);
        }
    }
  
    return status;
}

asynStatus drvFHT::_writeRead(const char *buffer) {
/*-----------------------------------------------------------------------------
 * Writes a string to device and reads response.
 * Output format should be: <BEL><addr><command><checksum><ETX>, where:
 * <BEL> = ASCII code 7, 
 * <addr> = [01:99],
 * <command> is remote command, 
 * <checksum> = mod 256 checksum encoded as 2-digit hex number,
 * <ETX> = ASCII code 3.
 * 
 * Response format should be: <BEL><addr><command><data><checksum><ETX>, where:
 * <data> is the returned data, and all else is the same format as output.
 * Response can also be:
 * No response = wrong address or invalid command.
 * <NAK> = parity or checksum error
 * <ACK> = command received; no data to return.
 *---------------------------------------------------------------------------*/
    const char *functionName = "_writeRead";
    asynStatus status = asynSuccess; 
    size_t nbytesOut, nbytesIn; 
    int eomReason;
    unsigned char checksum;
    char tmpStr[128];
    char cmdBuffer[128];

    /* Construct command string with start character, address, command, checksum, 
        and terminator */
    sprintf(tmpStr, "%s%02d%s", "\x07", deviceAddr_, buffer);
    checksum = _checksumCalc(tmpStr);
    sprintf(cmdBuffer, "%s%02X", tmpStr, checksum);

    // Set input/output terminators
    status = pasynOctetSyncIO->setInputEos(pasynUser, "\x03", 1); 
    status = pasynOctetSyncIO->setOutputEos(pasynUser, "\x03", 1); 

    // pasynOctetSyncIO->writeRead calls flush, write, and read
    status = pasynOctetSyncIO->writeRead(pasynUser, cmdBuffer, strlen(cmdBuffer),
    replyBuffer_, sizeof(replyBuffer_), timeout_, &nbytesOut, &nbytesIn, &eomReason);
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: buffer=%s, status=%d, nbytesOut=%d, replyBuffer_=%s, nbytesIn=%d\n",
            driverName, functionName, buffer, status, (int)nbytesOut, replyBuffer_, (int)nbytesIn);
  
    if((status != asynSuccess) || !nbytesIn || ((int)nbytesIn > REPLY_BUF_LEN)) {
        // Print an error message if this is the first error        
        if (!err_count_) {
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                    "%s::%s: Error: cmdBuffer=%s, status=%d, nbytesOut=%d, replyBuffer_=%s, nbytesIn=%d\n",
                    driverName, functionName, cmdBuffer, status, (int)nbytesOut, replyBuffer_, (int)nbytesIn);
        }
        err_count_++;
        return asynError;
    }
    
    if (strcmp(replyBuffer_, "\x15") == 0) {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Got NAK: parity or checksum error.\n", driverName, functionName);
    } else if (strcmp(replyBuffer_, "\x06") == 0) {
        asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
                "%s::%s: Got ACK: command OK, no data returned.\n", driverName, functionName);
    }
    

    // Remove the echoed command from the beginning of the reply
    memmove(replyBuffer_, replyBuffer_ + strlen(tmpStr), strlen(replyBuffer_));
    
    // Remove the checksum (last two bytes of reply)
    replyBuffer_[strlen(replyBuffer_) - 2] = '\0';
  
    // Reset error message counter
    if (err_count_) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
                "%s::%s: Status OK after %d error(s)\n",
                driverName, functionName, err_count_);
        err_count_ = 0;
    }
 
    return status;
}

asynStatus drvFHT::writeInt32(asynUser *pasynUser, epicsInt32 value) {
    const char* functionName = "writeInt32";
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    int addr;
    const char *paramName;
    char tmpStr[64];

    getAddress(pasynUser, &addr);
    
    getParamName(function, &paramName);
    
    asynPrint(pasynUserSelf, ASYN_TRACE_FLOW,
            "%s::%s: function=%d (%s), addr=%d, value=%d\n", 
            driverName, functionName, function, paramName, addr, value);
    
    // Hardware reset
    if (function == hwReset) {
        status = _write("!!");
    // Channel activate/deactivate
    } else if (function == chanState) {
        sprintf(tmpStr, "eW%d %d", addr, value);
        status = _write(tmpStr);
    // Device poll interval
    } else if (function == pollInterval) {
        if (value < POLL_INT_MIN) value = POLL_INT_MIN;
        if (value > POLL_INT_MAX) value = POLL_INT_MAX;
        sprintf(tmpStr, "yW %d", value);
        status = _write(tmpStr);
    }
    
    /* Set the parameter in the parameter library. */
    status = (asynStatus)setIntegerParam(addr, function, value);
    
    status = (asynStatus)callParamCallbacks(addr);
    
    if (status) { 
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                 "%s:%s, port %s, ERROR writing %d to address %d, status=%d\n",
                 driverName, functionName, portName, value, addr, status);
    } else {        
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER, 
                 "%s:%s, port %s, wrote %d to address %d\n",
                 driverName, functionName, portName, value, addr);
    }
    
    return status;
}

static void exitHandler(void *drvPvt) {
/*---------------------------------------------------------
    Exit handler, delete the drvFHT object.
-----------------------------------------------------------*/
  drvFHT *pdrvFHT = (drvFHT *)drvPvt;
  delete pdrvFHT;
}

// Configuration routine.  Called directly, or from the iocsh function below
extern "C" {
int drvFHTConfigure(const char* port, const char* IOPort, int addr, double timeout) {
/*------------------------------------------------------------------------------
 * EPICS iocsh callable function to call constructor for the drvFHT class.
 *  port    The name of the asyn port driver to be created.
 *  IOPort  The communication port name.
 *  addr    The device address.
 *  timeout The timeout for I/O.
 *----------------------------------------------------------------------------*/
    new drvFHT(port, IOPort, addr, timeout);
    return asynSuccess;
}

static const iocshArg initArg0 = {"port", iocshArgString};
static const iocshArg initArg1 = {"IOport", iocshArgString};
static const iocshArg initArg2 = {"addr", iocshArgInt};
static const iocshArg initArg3 = {"timeout", iocshArgDouble};
static const iocshArg* const initArgs[] = {&initArg0, &initArg1, &initArg2, &initArg3};
static const iocshFuncDef initFuncDef = {"drvFHTConfigure", 4, initArgs};

static void initCallFunc(const iocshArgBuf *args) {
    drvFHTConfigure(args[0].sval, args[1].sval, args[2].ival, args[3].dval);
}

void drvFHTRegister(void){
    iocshRegister(&initFuncDef, initCallFunc);
}

epicsExportRegistrar(drvFHTRegister);
}



