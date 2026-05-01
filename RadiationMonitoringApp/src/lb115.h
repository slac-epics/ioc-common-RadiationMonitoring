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

        void initGeneralParameters();
        void initChannelParameters();
        asynStatus connect(const char* ipPort);
        asynStatus sendAndReceive(const char* outMsg, char* inBuf, size_t& nRead);
        void getData();
        std::map<std::string, std::string> parseData(const std::string& data);
        double parseDoubleSafe(const std::string& val);
        int parseHexSafe(const std::string& val);

        void processResponse(const std::string& val);
        void generalPollerThread();

    protected:
        // General Parameters:
        int P_program_version;
        int P_kernel_version;
        int P_mac_address;
        int P_ip_address;
        int P_serial_number;
        int P_dhcp_status;
        int P_port_number;
        int P_subnet_mask;
        int P_default_gateway;
        int P_device_status;
        int P_device_name;

        // Channel Parameters:
        int P_curr_meas;
        int P_oldest_meas;
        int P_fifo_meas_status;
        int P_read_index;
        int P_detector_name;
        int P_detector_tag;
        int P_meas_id;
        int P_meas_id_name;
        int P_meas_date;
        int P_memory_index;
        int P_dose_time;
        int P_meas_time;
        int P_bkg_meas_time;
        int P_meas_status;
        int P_status;
        int P_meas_val;
        int P_max_meas_val;
        int P_dose_val;
        int P_gross_val;
        int P_net_val;
        int P_bkg_val;
        int P_unc_abs;
        int P_unc_rel;
        int P_detection_limit;
        int P_decision_thres;
        int P_best_est;
        int P_unc_best_est;
        int P_lower_conf;
        int P_upper_conf;
        int P_calib_factor;
        int P_delta_scint;
        int P_alarm1;
        int P_alarm2;
        int P_alarm3;
        int P_alarm4;
        int P_unit_meas;
        int P_unit_dose;

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
