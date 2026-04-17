#include "lb115.h"

static std::mutex mtx;
LB115Driver* LB115Driver::_instance = nullptr;

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
                   10,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   0,
                   1,
                   0,
                   0) 
{
    
    pasynUser = nullptr;
    asynStatus driverStatus = pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);
    pasynOctetSyncIO->setInputEos(pasynUser, "\r\n", 2);
    pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);

    if (driverStatus != asynSuccess) {
        printf("\nFailed to connect to port %s", ipPort);
    }
}
    //
    /*pollerId = epicsThreadCreate("LB115Poller",
                                 epicsThreadPriorityMedium,
                                 epicsThreadGetStackSize(epicsThreadStackMedium),
                                 [](void *p){((LB115Driver*)p)->pollerThread();},
                                 this)
                                 */

LB115Driver& LB115Driver::getInstance(const char* portName,
                                      const char* ipPort) {
    
    std::lock_guard<std::mutex> lock(mtx);

    if (!_instance) {
        _instance = new LB115Driver(portName, ipPort);
    }
    
    return *_instance;
}

LB115Driver& LB115Driver::getInstance() {
    if (!_instance) {
        throw std::runtime_error("LB115Driver not initialized.");
    }
    return *_instance;
}

/*
asynStatus LB115Driver::connect(const char *ipPort) {
    asynStatus driverStatus = pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);
    pasynOctetSyncIO->setInputEos(pasynUser, "\r\n", 2);
    pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);

    if (driverStatus != asynSuccess) {
        printf("\nFailed to connect to port %s", ipPort);
    }
    return driverStatus;
} */

void LB115Driver::getData() {
    asynStatus test;
    size_t nBytesOut, nBytesIn;
    int eomReason;
    const char* buffer = "*0001900101010109**";
    test = pasynOctetSyncIO->writeRead(pasynUser, buffer, strlen(buffer), cmdBuffer, MAX_MSG, TIMEOUT, &nBytesOut, &nBytesIn, &eomReason);

    printf("\n\nBytes out %ld, bytes in %ld", nBytesOut, nBytesIn);
    printf("\n\neomReason %d", eomReason);
    printf("\n\nError message? %s\n", pasynUser->errorMessage);
    printf("\nBuffer:%s\n", buffer);
    printf("\nCommand Buffer: %s\n", cmdBuffer);
    printf("\nTest variable: %d\n", test);
}


extern "C" {
    int LB115Configure(const char* portName, const char* ipPort) {
        printf("Trying to connect to LB115 Device.\n");
        LB115Driver::getInstance(portName, ipPort);
        return 0;
    }
    static const iocshArg arg0 = {"portName", iocshArgString};
    static const iocshArg arg1 = {"ipPort", iocshArgString};
    static const iocshArg * args[] = {&arg0, &arg1};
    static const iocshFuncDef funcDef = {"LB115Configure", 2, args};

    static void funcCall(const iocshArgBuf *args) {
        LB115Configure(args[0].sval, args[1].sval);
    }

    void LB115ConfigureRegister(void) {
        iocshRegister(&funcDef, funcCall);
    }

    epicsExportRegistrar(LB115ConfigureRegister);
}

int LB115GetData() {
    printf("Trying to get data from the LB115 Driver");
    LB115Driver::getInstance().getData();
}

// Initializing ioc shell command arguments
static const iocshFuncDef getData_funcDef = {"LB115GetData", 0, NULL};

// Initializing ioc shell command function
static void getData_funcCall(const iocshArgBuf *args) {
    LB115GetData();
}

void LB115GetDataRegister(void) {
    iocshRegister(&getData_funcDef, getData_funcCall);
}

extern "C" {
    epicsExportRegistrar(LB115GetDataRegister);
}
