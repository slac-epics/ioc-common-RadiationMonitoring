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

#define MAX_CH 4
#define NUM_CH_REG 4
#define NUM_GEN_REG 11
#define MAX_MSG 1024
#define TIMEOUT 1.0

class LB115Driver : public asynPortDriver {
    public:

        LB115Driver(const char *portName,
                    const char *ipPort);

        virtual void pollerThread();

        void enableChannel(int ch);

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
        int P_curr_meas[MAX_CH];
        int P_oldest_meas[MAX_CH];
        int P_fifo_meas_status[MAX_CH];
        int P_read_index[MAX_CH];
        int P_detector_name[MAX_CH];
        int P_detector_tag[MAX_CH];
        int P_meas_id[MAX_CH];
        int P_meas_id_name[MAX_CH];
        int P_meas_date[MAX_CH];
        int P_memory_index[MAX_CH];
        int P_dose_time[MAX_CH];
        int P_meas_time[MAX_CH];
        int P_bkg_meas_time[MAX_CH];
        int P_meas_status[MAX_CH];
        int P_status[MAX_CH];
        int P_meas_val[MAX_CH];
        int P_max_meas_val[MAX_CH];
        int P_dose_val[MAX_CH];
        int P_gross_val[MAX_CH];
        int P_net_val[MAX_CH];
        int P_bkg_val[MAX_CH];
        int P_unc_abs[MAX_CH];
        int P_unc_rel[MAX_CH];
        int P_detection_limit[MAX_CH];
        int P_decision_thres[MAX_CH];
        int P_best_est[MAX_CH];
        int P_unc_best_est[MAX_CH];
        int P_lower_conf[MAX_CH];
        int P_upper_conf[MAX_CH];
        int P_calib_factor[MAX_CH];
        int P_delta_scint[MAX_CH];
        int P_alarm1[MAX_CH];
        int P_alarm2[MAX_CH];
        int P_alarm3[MAX_CH];
        int P_alarm4[MAX_CH];
        int P_unit_meas[MAX_CH];
        int P_unit_dose[MAX_CH];
        
        // Watcher PVs:
        int P_timeout_count;
        int P_connection_status;
        int P_channel_state[MAX_CH];

    private:

        void initGeneralParameters();
        void initChannelParameters();

        std::string buildGeneralCommand(int reg);
        std::string buildChannelCommand(int channel, int reg);

        asynStatus sendAndReceive(const char* outCmd, char* inBuf, size_t& nRead);

        std::map<std::string, std::string> parseData(const std::string& data);
        double parseDoubleSafe(const std::string& val);
        int parseHexSafe(const std::string& val);

        void processGeneralResponse(const std::string& val, int reg);
        void processChannelResponse(const std::string& val, int channel, int reg);

        std::string parsePayload(const std::string& resp);

        std::string _ipPort;
        
        epicsThreadId pollerId;
        std::atomic<bool> running;

        asynUser *pasynUser;

        enum channelRegisters {
            CH_REG_0 = 0,
            CH_REG_1 = 1,
            CH_REG_7 = 7,
            CH_REG_9 = 9
        };
        
        struct ChannelState {
            bool active = false;
            int regIndex = 0;
        };

        static const int CH_REG_LIST[4];

        ChannelState chState[MAX_CH];

        int timeoutCount{0};
        bool connectionStatus;
};
