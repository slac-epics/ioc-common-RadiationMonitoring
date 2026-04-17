#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <string>

#include <iocsh.h>
#include <string.h>
#include <mutex>
#include <atomic>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsTypes.h>
#include <asynPortDriver.h>
#include <asynOctetSyncIO.h>

#define MAX_MSG 1024
#define TIMEOUT 1.0

class LB115Driver : public asynPortDriver {
    public:

        static LB115Driver& getInstance(const char* portName,
                                        const char* ipPort);
        static LB115Driver& getInstance();
        asynStatus connect(const char* ipPort);
        void getData();

    protected:
        
        int P_Index;

    private:

        LB115Driver(const char *portName,
                    const char *ipPort);

        LB115Driver(const LB115Driver&) = delete;
        LB115Driver& operator=(const LB115Driver&) = delete;

        static LB115Driver* _instance;

        asynUser *pasynUser;
        char cmdBuffer[MAX_MSG];
        char sendBuffer[MAX_MSG];
};
