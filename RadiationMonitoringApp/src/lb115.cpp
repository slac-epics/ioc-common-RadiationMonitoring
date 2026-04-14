#include "lb115.h"

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
                   10
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   0, 1, 0, 0) {

    createParam("MEAS_VAL", asynFloat64, &P_MeasVal);
    createParam("NET_VAL", asynFloat64, &NetVal);
    createParam("GROSS_VAL", asynFloat64, &P_GrossVal);
    createParam("DOSE_VAL", asynFloat64, &P_DoseVal);
    createParam("STATUS", asynInt32, &P_Status);
    createParam("MEAS_STATUS", asynInt32, &P_MeasStatus);

    pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);

    pollerId = epicsThreadCreate("LB115Poller",
                                 epicsThreadPriorityMedium,
                                 epicsThreadGetStackSize(epicsThreadStackMedium),
                                 [](void *p){((LB115Driver*)p)->pollerThread();},
                                 this)
    }

void LB115Driver:sendRequest() {
    const char *cmd = '';
    size_t nwrite;
    pasynOctetSyncIO->(pasynUser, cmd, strlen(cmd),TIMEOUT, &nwrite);

}
