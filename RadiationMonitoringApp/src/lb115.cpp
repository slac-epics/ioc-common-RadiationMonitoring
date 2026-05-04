#include "lb115.h"

static std::mutex mtx;
LB115Driver* LB115Driver::_instance = nullptr;

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
                   300,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask | asynDrvUserMask,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   0,
                   1,
                   0,
                   0), 
    _ipPort(ipPort)
{
    
    // Initializing Asyn Parameters:
    initGeneralParameters();
    initChannelParameters();

    // TODO: Make pasynUser per thread and create different connections
    pasynUser = nullptr;
    asynStatus driverStatus = pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);
    pasynOctetSyncIO->setInputEos(pasynUser, "\r", 1);
    pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);

    printf("CONNECT status=%d pasynUser=%p ipPort %s\n", driverStatus, pasynUser, ipPort);

    if (driverStatus != asynSuccess || !pasynUser) {
        printf("Initial connection failed\n");
        printf("\nFailed to connect to port %s", ipPort);
        pasynUser = nullptr;

        return;
        
    } else {
        printf("Connected successfully\n");
    }

    running = true;
    pollerId = epicsThreadCreate("LB115Poller",
                                 epicsThreadPriorityMedium,
                                 epicsThreadGetStackSize(epicsThreadStackMedium),
                                 [](void *p){((LB115Driver*)p)->pollerThread();},
                                 this);
}

LB115Driver& LB115Driver::getInstance(const char* portName,
                                      const char* ipPort) {
    
    std::lock_guard<std::mutex> lock(mtx);

    printf("getInstance called with %s %s\n", portName, ipPort);

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
void LB115Driver::startGeneralThread() {

    // TODO: Create the generalPollerThread function.
    pollerId[0] = epicsThreadCreate("LB115GeneralPoller",
                                 epicsThreadPriorityMedium,
                                 epicsThreadGetStackSize(epicsThreadStackMedium),
                                 [](void *p){((LB115Driver*)p)->generalPollerThread();},
                                 this);
}
*/

void LB115Driver::initGeneralParameters() {
    /*
     * There are a number of general parameters to instantiate,
     * each with their own registers.
     */
    createParam("PROGRAM_VER",     asynParamOctet, &P_program_version);
    createParam("KERNEL_VER",      asynParamOctet, &P_kernel_version);
    createParam("MAC_ADDR",        asynParamOctet, &P_mac_address);
    createParam("IP_ADDR",         asynParamOctet, &P_ip_address);
    createParam("SERIAL_NUM",      asynParamOctet, &P_serial_number);
    createParam("DHCP_STATUS",     asynParamOctet, &P_dhcp_status);
    createParam("PORT_NUM",        asynParamOctet, &P_port_number);
    createParam("SUBNET",          asynParamOctet, &P_subnet_mask);
    createParam("DEFAULT_GATEWAY", asynParamOctet, &P_default_gateway);
    createParam("DEVICE_STATUS",   asynParamOctet, &P_device_status);
    createParam("DEVICE_NAME",     asynParamOctet, &P_device_name);
}

void LB115Driver::initChannelParameters() {
    /*
     * There are 4 channels with 4 registers of interest each. The first three
     * return easily parsable scalar values of interst, but the final register
     * returns a large dictionary of key:value pairs that needs to be parsed
     * and put into pvs. 
     */

    for (int ch = 0; ch < MAX_CH; ch++) {
        char name[64];
        snprintf(name, sizeof(name), "CH%d_CURR_MEAS", ch+1);
        createParam(name, asynParamFloat64, &P_curr_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_OLDEST_MEAS", ch+1);
        createParam(name, asynParamFloat64, &P_oldest_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_FIFO_MEAS_STATUS", ch+1);
        createParam(name, asynParamInt32, &P_fifo_meas_status[ch]);
        snprintf(name, sizeof(name), "CH%d_READ_INDEX", ch+1);
        createParam(name, asynParamOctet, &P_read_index[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTOR_NAME", ch+1);
        createParam(name, asynParamOctet, &P_detector_name[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTOR_TAG", ch+1);
        createParam(name, asynParamOctet, &P_detector_tag[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_ID", ch+1);
        createParam(name, asynParamOctet, &P_meas_id[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_ID_NAME", ch+1);
        createParam(name, asynParamOctet, &P_meas_id_name[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_DATE", ch+1);
        createParam(name, asynParamOctet, &P_meas_date[ch]);
        snprintf(name, sizeof(name), "CH%d_MEMORY_INDEX", ch+1);
        createParam(name, asynParamInt32, &P_memory_index[ch]);
        snprintf(name, sizeof(name), "CH%d_DOSE_TIME", ch+1);
        createParam(name, asynParamInt32, &P_dose_time[ch]);        // seconds
        snprintf(name, sizeof(name), "CH%d_MEAS_TIME", ch+1);
        createParam(name, asynParamInt32, &P_meas_time[ch]);        // seconds
        snprintf(name, sizeof(name), "CH%d_BKG_MEAS_TIME", ch+1);
        createParam(name, asynParamInt32, &P_bkg_meas_time[ch]);    // seconds
        snprintf(name, sizeof(name), "CH%d_MEAS_STATUS", ch+1);
        createParam(name, asynParamInt32, &P_meas_status[ch]);
        snprintf(name, sizeof(name), "CH%d_STATUS", ch+1);
        createParam(name, asynParamInt32, &P_status[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_meas_val[ch]);       // mrem/h
        snprintf(name, sizeof(name), "CH%d_MAX_MEAS_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_max_meas_val[ch]);
        snprintf(name, sizeof(name), "CH%d_DOSE_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_dose_val[ch]);       // mrem
        snprintf(name, sizeof(name), "CH%d_GROSS_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_gross_val[ch]);
        snprintf(name, sizeof(name), "CH%d_NET_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_net_val[ch]);
        snprintf(name, sizeof(name), "CH%d_BKG_VAL", ch+1);
        createParam(name, asynParamFloat64, &P_bkg_val[ch]);
        snprintf(name, sizeof(name), "CH%d_UNCERTAINTY_ABS", ch+1);
        createParam(name, asynParamFloat64, &P_unc_abs[ch]);
        snprintf(name, sizeof(name), "CH%d_UNCERTAINTY_REL", ch+1);
        createParam(name, asynParamFloat64, &P_unc_rel[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTION_LIMIT", ch+1);
        createParam(name, asynParamFloat64, &P_detection_limit[ch]);
        snprintf(name, sizeof(name), "CH%d_DECISION_THRES", ch+1);
        createParam(name, asynParamFloat64, &P_decision_thres[ch]);
        snprintf(name, sizeof(name), "CH%d_BEST_EST", ch+1);
        createParam(name, asynParamFloat64, &P_best_est[ch]);
        snprintf(name, sizeof(name), "CH%d_UNC_BEST_EST", ch+1);
        createParam(name, asynParamFloat64, &P_unc_best_est[ch]);
        snprintf(name, sizeof(name), "CH%d_LOWER_LIMIT_CONF", ch+1);
        createParam(name, asynParamFloat64, &P_lower_conf[ch]);
        snprintf(name, sizeof(name), "CH%d_UPPER_LIMIT_CONF", ch+1);
        createParam(name, asynParamFloat64, &P_upper_conf[ch]);
        snprintf(name, sizeof(name), "CH%d_CALIB_FACTOR", ch+1);
        createParam(name, asynParamFloat64, &P_calib_factor[ch]);
        snprintf(name, sizeof(name), "CH%d_DELTA_SCINT", ch+1);
        createParam(name, asynParamFloat64, &P_delta_scint[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM1", ch+1);
        createParam(name, asynParamFloat64, &P_alarm1[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM2", ch+1);
        createParam(name, asynParamFloat64, &P_alarm2[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM3", ch+1);
        createParam(name, asynParamFloat64, &P_alarm3[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM4", ch+1);
        createParam(name, asynParamFloat64, &P_alarm4[ch]);
        snprintf(name, sizeof(name), "CH%d_UNIT_MEAS_VAL", ch+1);
        createParam(name, asynParamOctet, &P_unit_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_UNIT_DOSE_VAL", ch+1);
        createParam(name, asynParamOctet, &P_unit_dose[ch]);
    }
}

const int LB115Driver::REG_LIST[1] = {9};

std::string LB115Driver::buildCommand(int channel, int reg) {
    char buf[64];
    snprintf(buf, sizeof(buf), "*0001900101%02d01%02d**", channel, reg);
    return std::string(buf);
}

void LB115Driver::pollerThread() {

    const double loopDelay = 0.05; //seconds
    
    while(running.load()) {
        
        // Reading the same register for all active channels before
        // moving onto the next register
        for (int channel = 0; channel < MAX_CH; channel++) {
            if (!chState[channel].enabled) {
                //printf("channel %0d disabled", channel);
                epicsThreadSleep(0.5);
                continue;
            }

            int ch = channel + 1;

            printf("channel %0d enabled", channel);

            int reg = REG_LIST[chState[channel].regIndex];
            
            std::string cmd = buildCommand(ch, reg);

            char response[MAX_MSG] = {0};
            size_t nRead = 0;

            asynStatus status = sendAndReceive(cmd.c_str(), response, nRead);

            if (status != asynSuccess) {
                printf("Poller not connected, retrying...\n");

                pasynOctetSyncIO->disconnect(pasynUser);
                pasynUser = nullptr;

                epicsThreadSleep(2.0);
                break; //restart outer loop
            }

            if (nRead > 0) {
                processResponse(response, ch);
                //printf("Poller RX (%zu): %s\n", nRead, response);
            }

            // advance state machine
            chState[channel].regIndex = (chState[channel].regIndex + 1) % NUM_REG;

        }
        epicsThreadSleep(loopDelay);
    }
}

/*
void LB115Driver::channelPollerThread(int channel) {
    const double pollDelay = 10.0; // seconds
    const int registers[] = {0, 1, 7, 9};

    printf("Poller started\n");

    while(running[channel]) {
        try {
            if (!pasynUser) {
                printf("Poller: not connected, retrying...\n");

                asynStatus status = pasynOctetSyncIO->connect(_ipPort.c_str(), 0, &pasynUser, NULL);

                if (status != asynSuccess || !pasynUser) {
                    printf("Reconnect failed \n");
                    pasynUser = nullptr;
                    epicsThreadSleep(2.0);
                    continue;
                }

                printf("Reconnect successful\n");

                pasynOctetSyncIO->setInputEos(pasynUser, "\r", 1);
                pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);
            }
            
            //const char* msg = "*0001900101010109**";

            for (int reg : registers) {
                const char* msg = buildCommand(channel, reg);
                printf("Poller TX: %s\n", msg);

                char response[MAX_MSG] = {0};
                size_t nRead = 0;

                asynStatus status = sendAndReceive(msg, response, nRead, channel);

                if (status != asynSuccess) {
                    printf("Poller: communication error\n");

                    pasynOctetSyncIO->disconnect(pasynUser);
                    pasynUser = nullptr;

                    epicsThreadSleep(2.0);
                    continue;
                }

                if (nRead > 0) {
                    printf("Poller RX (%zu): %s\n", nRead, response);
                }
            }
            
        } catch (...) {
            printf("Poller: caught expection (prevented crash)\n");
        }

        epicsThreadSleep(pollDelay);

    }
    printf("Poller thread exiting\n");
}
*/

asynStatus LB115Driver::sendAndReceive(const char* outMsg, char* inBuf, size_t& nRead) {

    size_t nWritten = 0;
    int eomReason = 0;

    if (!pasynUser || !pasynOctetSyncIO) {
        printf("pasynUser is null - not connected\n");
        return asynError;
    }
    asynStatus status = pasynOctetSyncIO->writeRead(
            pasynUser,
            outMsg,
            strlen(outMsg),
            inBuf,
            MAX_MSG-1,
            TIMEOUT,
            &nWritten,
            &nRead,
            &eomReason
            );

    if (status != asynSuccess) {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
                  "writeRead failed: %d\n", status);
        return status;
    }
    
    // NOTE: why do I need this
    if (nRead >= MAX_MSG){
        nRead = MAX_MSG -1;
    }
    inBuf[nRead] = '\0';

    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "TS: %s\nRX: %s\n", outMsg, inBuf);
    std::string resp(inBuf, nRead);
    
    return asynSuccess;
}

std::map<std::string, std::string> LB115Driver::parseData(const std::string& data) {
    std::map<std::string, std::string> result;
    std::stringstream ss(data);
    std::string item;
    
    while (std::getline(ss, item, '|')) {
        auto pos = item.find(':');
        if (pos != std::string::npos) {
            result[item.substr(0, pos)] = item.substr(pos + 1);
        }
    }

    return result;
}

double LB115Driver::parseDoubleSafe(const std::string& val) {
    if (val == "Tmout" || val.empty()) {
        return NAN;
    }
    return atof(val.c_str());
}

int LB115Driver::parseHexSafe(const std::string& val) {
    return strtol(val.c_str(), NULL, 16);
}

void LB115Driver::processResponse(const std::string& resp, int channel) {
    int ind = channel - 1;
    size_t start = resp.find('*');
    size_t end = resp.rfind('*');

    
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return;

    std::string body = resp.substr(start + 1, end - start - 1);
    
    size_t headerEnd = body.find_first_not_of("0123456789");
    std::string payload = body.substr(headerEnd);
    
    auto kv = parseData(payload);
    setStringParam(P_read_index[ind], kv["read_index"]);
    setStringParam(P_detector_name[ind], kv["detector_name"]);
    setStringParam(P_detector_tag[ind], kv["detector_tag"]);
    setStringParam(P_meas_id[ind], kv["meas_id"]);
    setStringParam(P_meas_id_name[ind], kv["meas_id_name"]);
    setStringParam(P_meas_date[ind], kv["meas_date"]);
    setIntegerParam(P_memory_index[ind], parseDoubleSafe(kv["memory_index"]));
    setIntegerParam(P_dose_time[ind], parseDoubleSafe(kv["dose_time"]));
    setIntegerParam(P_meas_time[ind], parseDoubleSafe(kv["meas_time"]));
    setIntegerParam(P_bkg_meas_time[ind], parseDoubleSafe(kv["bkg_meas_time"]));
    setIntegerParam(P_meas_status[ind], parseHexSafe(kv["meas_status"]));
    setIntegerParam(P_status[ind], parseHexSafe(kv["status"]));
    setDoubleParam(P_meas_val[ind], parseDoubleSafe(kv["meas_val"]));
    setDoubleParam(P_max_meas_val[ind], parseDoubleSafe(kv["max_meas_val"]));
    setDoubleParam(P_dose_val[ind], parseDoubleSafe(kv["dose_val"]));
    setDoubleParam(P_gross_val[ind], parseDoubleSafe(kv["gross_val"]));
    setDoubleParam(P_net_val[ind], parseDoubleSafe(kv["net_val"]));
    setDoubleParam(P_bkg_val[ind], parseDoubleSafe(kv["bkg_val"]));
    setDoubleParam(P_unc_abs[ind], parseDoubleSafe(kv["uncertainty_abs"]));
    setDoubleParam(P_unc_rel[ind], parseDoubleSafe(kv["uncertainty_rel"]));
    setDoubleParam(P_detection_limit[ind], parseDoubleSafe(kv["detection_limit"]));
    setDoubleParam(P_decision_thres[ind], parseDoubleSafe(kv["decision_thres"]));
    setDoubleParam(P_best_est[ind], parseDoubleSafe(kv["best_est"]));
    setDoubleParam(P_unc_best_est[ind], parseDoubleSafe(kv["unc_best_est"]));
    setDoubleParam(P_lower_conf[ind], parseDoubleSafe(kv["lower_limit_conf"]));
    setDoubleParam(P_upper_conf[ind], parseDoubleSafe(kv["upper_limit_conf"]));
    setDoubleParam(P_calib_factor[ind], parseDoubleSafe(kv["calib_factor"]));
    setDoubleParam(P_delta_scint[ind], parseDoubleSafe(kv["delta_scint_factor"]));
    setDoubleParam(P_alarm1[ind], parseDoubleSafe(kv["alarm_limit_1"]));
    setDoubleParam(P_alarm2[ind], parseDoubleSafe(kv["alarm_limit_2"]));
    setDoubleParam(P_alarm3[ind], parseDoubleSafe(kv["alarm_limit_3"]));
    setDoubleParam(P_alarm4[ind], parseDoubleSafe(kv["alarm_limit_4"]));
    setStringParam(P_unit_meas[ind], kv["unit_meas_val"]);
    setStringParam(P_unit_dose[ind], kv["unit_dose_val"]);

    callParamCallbacks();
}

/*
void LB115Driver::getData() {
    char response[MAX_MSG];
    size_t nRead = 0;

    const char* msg = "*0001900101010109**";

    asynStatus status = sendAndReceive(msg, response, nRead);

    if (status != asynSuccess) {
        printf("Communication failed\n");
        return;
    }

    printf("Received (%zu bytes): %s\n", nRead, response);
}
*/

void LB115Driver::enableChannel(int channel) {
    if (channel < 1 || channel > MAX_CH) {
         return;
    }
    chState[channel-1].enabled = true;
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

extern "C" {
    int LB115EnableChannel(int channel) {
        LB115Driver::getInstance().enableChannel(channel);
        return 0;
    }
    static const iocshArg enableArg0 = {"channelNum", iocshArgInt};
    static const iocshArg * enableArgs[] = {&enableArg0};
    static const iocshFuncDef enable_funcDef = {"lb115EnableChannel", 1, enableArgs};

    static void enable_funcCall(const iocshArgBuf *enableArgs) {
        LB115EnableChannel(enableArgs[0].ival);
    }

    void LB115EnableChannelRegister(void) {
        iocshRegister(&enable_funcDef, enable_funcCall);
    }

    epicsExportRegistrar(LB115EnableChannelRegister);

}
