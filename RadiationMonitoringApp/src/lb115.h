#include <iostream>
#include <cstdint>
#include <memory>
#include <functional>
#include <map>
#include <string>

#include <iocsh.h>
#include <string.h>
#include <sstream>
#include <cstring>
#include <math.h>
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
        asynStatus sendAndReceive(const char* outMsg, char* inBuf, size_t& nRead);
        void getData();
        std::map<std::string, std::string> parseData(const std::string& data);
        double parseDoubleSafe(const std::string& val);
        int parseHexSafe(const std::string& val);

        void processResponse(const std::string& val);
        void pollerThread();

    protected:
        
        std::string P_read_index;
        std::string P_detector_name;
        std::string P_detector_tag;
        std::string P_meas_id;
        std::string P_meas_id_name;
        std::string P_meas_date;
        int P_memory_index;
        int P_dose_time;
        int P_meas_time;
        int P_bkg_meas_time;
        int P_meas_status;
        int P_status;
        float P_meas_val;
        float P_max_meas_val;
        float P_dose_val;
        float P_gross_val;
        float P_net_val;
        float P_bkg_val;
        float P_unc_abs;
        float P_unc_rel;
        float P_detection_limit;
        float P_decision_thres;
        float P_best_est;
        float P_unc_best_est;
        float P_lower_conf;
        float P_upper_conf;
        float P_calib_factor;
        float P_delta_scint;
        float P_alarm1;
        float P_alarm2;
        float P_alarm3;
        float P_alarm4;
        std::string P_unit_meas;
        std::string P_unit_dose;

    private:

        LB115Driver(const char *portName,
                    const char *ipPort);

        LB115Driver(const LB115Driver&) = delete;
        LB115Driver& operator=(const LB115Driver&) = delete;

        std::string _ipPort;
        static LB115Driver* _instance;
        
        epicsThreadId pollerId;
        std::atomic<bool> running;

        asynUser *pasynUser;
        char cmdBuffer[MAX_MSG];
        char sendBuffer[MAX_MSG];
};
