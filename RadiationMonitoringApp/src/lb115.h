#include <asynPortDriver.h>
#include <epicsThread.h>
#include <epicsTime.h>
#include <iocsh.h>
#include <asynOctetSyncIO.h>

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <cstring>

#define MAX_MSG 4096
#define TIMEOUT 1.0

class LB115Driver : public asynPortDriver {
    public:
        LB115Driver(const char *portName,
                    const chat *ipPort);


    protected:
        /*
        int P_Index;
        std::string P_DetectorDescription;
        std::string P_DetectorDesignation;
        int P_Status;
        float P_MeasVal;
        int P_MaxMeasVal;
        std::string P_Units;
        float P_AbsoluteUncertainty;
        float P_RelativeUncertainty;
        float P_Threshold; 
        float P_DetectionLimit;
        */


    private:
        void pollerThread();
        void sendRequest();
        void readResponse();
        void processResponse(const std::string&);

        std::map<std::string, std::string> parseData(const std::string&);
        double parseDoubleSafe(const std::string&);
        int parseHexSafe(const std::string&);

        int P_MeasVal;
        int P_NetVal;
        int P_GrossVal;
        int P_DoseVal;
        int P_Status;
        int P_MeasStatus;
        
        asynUser *pasynUser;

        epicsThreadId pollerId;     
}
