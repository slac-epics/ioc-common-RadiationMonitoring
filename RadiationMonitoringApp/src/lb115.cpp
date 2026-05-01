#include "lb115.h"

static std::mutex mtx;
LB115Driver* LB115Driver::_instance = nullptr;

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
                   100,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask | asynDrvUserMask,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   0,
                   1,
                   0,
                   0), 
    _ipPort(ipPort)
{
    
    // Initializing Asyn Parameters:
    //initGeneralParameters();
    initChannelParameters();

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
    pollerId = epicsThreadCreate("LB115GeneralPoller",
                                 epicsThreadPriorityMedium,
                                 epicsThreadGetStackSize(epicsThreadStackMedium),
                                 [](void *p){((LB115Driver*)p)->generalPollerThread();},
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
        createParam("CURR_MEAS",        asynParamFloat64, &P_curr_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_OLDEST_MEAS", ch+1);
        createParam("OLDEST_MEAS",      asynParamFloat64, &P_oldest_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_FIFO_MEAS_STATUS", ch+1);
        createParam("FIFO_MEAS_STATUS", asynParamInt32, &P_fifo_meas_status[ch]);
        snprintf(name, sizeof(name), "CH%d_READ_INDEX", ch+1);
        createParam("READ_INDEX",       asynParamOctet, &P_read_index[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTOR_INDEX", ch+1);
        createParam("DETECTOR_NAME",    asynParamOctet, &P_detector_name[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTOR_TAG", ch+1);
        createParam("DETECTOR_TAG",     asynParamOctet, &P_detector_tag[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_ID", ch+1);
        createParam("MEAS_ID",          asynParamOctet, &P_meas_id[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_ID_NAME", ch+1);
        createParam("MEAS_ID_NAME",     asynParamOctet, &P_meas_id_name[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_DATE", ch+1);
        createParam("MEAS_DATE",        asynParamOctet, &P_meas_date[ch]);
        snprintf(name, sizeof(name), "CH%d_MEMORY_INDEX", ch+1);
        createParam("MEMORY_INDEX",     asynParamInt32, &P_memory_index[ch]);
        snprintf(name, sizeof(name), "CH%d_DOSE_TIME", ch+1);
        createParam("DOSE_TIME",        asynParamInt32, &P_dose_time[ch]);        // seconds
        snprintf(name, sizeof(name), "CH%d_MEAS_TIME", ch+1);
        createParam("MEAS_TIME",        asynParamInt32, &P_meas_time[ch]);        // seconds
        snprintf(name, sizeof(name), "CH%d_BKG_MEAS_TIME", ch+1);
        createParam("BKG_MEAS_TIME",    asynParamInt32, &P_bkg_meas_time[ch]);    // seconds
        snprintf(name, sizeof(name), "CH%d_MEAS_STATUS", ch+1);
        createParam("MEAS_STATUS",      asynParamInt32, &P_meas_status[ch]);
        snprintf(name, sizeof(name), "CH%d_STATUS", ch+1);
        createParam("STATUS",           asynParamInt32, &P_status[ch]);
        snprintf(name, sizeof(name), "CH%d_MEAS_VAL", ch+1);
        createParam("MEAS_VAL",         asynParamFloat64, &P_meas_val[ch]);       // mrem/h
        snprintf(name, sizeof(name), "CH%d_MAX_MEAS_VAL", ch+1);
        createParam("MAX_MEAS_VAL",     asynParamFloat64, &P_max_meas_val[ch]);
        snprintf(name, sizeof(name), "CH%d_DOSE_VAL", ch+1);
        createParam("DOSE_VAL",         asynParamFloat64, &P_dose_val[ch]);       // mrem
        snprintf(name, sizeof(name), "CH%d_GROSS_VAL", ch+1);
        createParam("GROSS_VAL",        asynParamFloat64, &P_gross_val[ch]);
        snprintf(name, sizeof(name), "CH%d_NET_VAL", ch+1);
        createParam("NET_VAL",          asynParamFloat64, &P_net_val[ch]);
        snprintf(name, sizeof(name), "CH%d_BKG_VAL", ch+1);
        createParam("BKG_VAL",          asynParamFloat64, &P_bkg_val[ch]);
        snprintf(name, sizeof(name), "CH%d_UNCERTAINTY_ABS", ch+1);
        createParam("UNCERTAINTY_ABS",  asynParamFloat64, &P_unc_abs[ch]);
        snprintf(name, sizeof(name), "CH%d_UNCERTAINTY_REL", ch+1);
        createParam("UNCERTAINTY_REL",  asynParamFloat64, &P_unc_rel[ch]);
        snprintf(name, sizeof(name), "CH%d_DETECTION_LIMIT", ch+1);
        createParam("DETECTION_LIMIT",  asynParamFloat64, &P_detection_limit[ch]);
        snprintf(name, sizeof(name), "CH%d_DECISION_THRES", ch+1);
        createParam("DECISION_THRES",   asynParamFloat64, &P_decision_thres[ch]);
        snprintf(name, sizeof(name), "CH%d_BEST_EST", ch+1);
        createParam("BEST_EST",         asynParamFloat64, &P_best_est[ch]);
        snprintf(name, sizeof(name), "CH%d_UNC_BEST_EST", ch+1);
        createParam("UNC_BEST_EST",     asynParamFloat64, &P_unc_best_est[ch]);
        snprintf(name, sizeof(name), "CH%d_LOWER_LIMIT_CONF", ch+1);
        createParam("LOWER_LIMIT_CONF", asynParamFloat64, &P_lower_conf[ch]);
        snprintf(name, sizeof(name), "CH%d_UPPER_LIMIT_CONF", ch+1);
        createParam("UPPER_LIMIT_CONF", asynParamFloat64, &P_upper_conf[ch]);
        snprintf(name, sizeof(name), "CH%d_CALIB_FACTOR", ch+1);
        createParam("CALIB_FACTOR",     asynParamFloat64, &P_calib_factor[ch]);
        snprintf(name, sizeof(name), "CH%d_DELTA_SCINT", ch+1);
        createParam("DELTA_SCINT",      asynParamFloat64, &P_delta_scint[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM1", ch+1);
        createParam("ALARM_LIM1",       asynParamFloat64, &P_alarm1[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM2", ch+1);
        createParam("ALARM_LIM2",       asynParamFloat64, &P_alarm2[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM3", ch+1);
        createParam("ALARM_LIM3",       asynParamFloat64, &P_alarm3[ch]);
        snprintf(name, sizeof(name), "CH%d_ALARM_LIM4", ch+1);
        createParam("ALARM_LIM4",       asynParamFloat64, &P_alarm4[ch]);
        snprintf(name, sizeof(name), "CH%d_UNIT_MEAS_VAL", ch+1);
        createParam("UNIT_MEAS_VAL",    asynParamOctet, &P_unit_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_UNIT_DOSE_VAL", ch+1);
        createParam("UNIT_DOSE_VAL",    asynParamOctet, &P_unit_dose[ch]);
    }
}


std::string LB115Driver::buildCommand(int channel, int reg) {
    char buf[64];
    snprintf(buf, sizeof(buf), "0001900101%02d01%02d**", channel, reg);
    return std::string(buf);
}

void LB115Driver::generalPollerThread() {
    const double pollDelay = 10.0; // seconds
    const int registers[] = {0, 1, 7, 9};

    printf("Poller started\n");

    while(running.load()) {
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
                std::string msg = buildCommand(channel, reg);
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

asynStatus LB115Driver::sendAndReceive(const char* outMsg, char* inBuf, size_t& nRead, int channel) {

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
    
    if (nRead >= MAX_MSG){
        nRead = MAX_MSG -1;
    }
    inBuf[nRead] = '\0';

    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "TS: %s\nRX: %s\n", outMsg, inBuf);
    std::string resp(inBuf, nRead);
    processResponse(resp, channel);
    
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
    size_t start = resp.find('*');
    size_t end = resp.rfind('*');

    if (start == std::string::npos || end == std::string::npos || end <= start)
        return;

    std::string body = resp.substr(start + 1, end - start - 1);
    
    size_t headerEnd = body.find_first_not_of("0123456789");
    std::string payload = body.substr(headerEnd);
    
    auto kv = parseData(payload);

    printf("Measured Val: %f\n", parseDoubleSafe(kv["meas_time"]));
    //setDoubleParam(P_meas_val, 0.1);
    setStringParam(P_read_index[channel], kv["read_index"]);
    setStringParam(P_detector_name[channel], kv["detector_name"]);
    setStringParam(P_detector_tag[channel], kv["detector_tag"]);
    setStringParam(P_meas_id[channel], kv["meas_id"]);
    setStringParam(P_meas_id_name[channel], kv["meas_id_name"]);
    setStringParam(P_meas_date[channel], kv["meas_date"]);
    setIntegerParam(P_memory_index[channel], parseDoubleSafe(kv["memory_index"]));
    setIntegerParam(P_dose_time[channel], parseDoubleSafe(kv["dose_time"]));
    setIntegerParam(P_meas_time[channel], parseDoubleSafe(kv["meas_time"]));
    setIntegerParam(P_bkg_meas_time[channel], parseDoubleSafe(kv["bkg_meas_time"]));
    setIntegerParam(P_meas_status[channel], parseHexSafe(kv["meas_status"]));
    setIntegerParam(P_status[channel], parseHexSafe(kv["status"]));
    setDoubleParam(P_meas_val[channel], parseDoubleSafe(kv["meas_val"]));
    setDoubleParam(P_max_meas_val[channel], parseDoubleSafe(kv["max_meas_val"]));
    setDoubleParam(P_dose_val[channel], parseDoubleSafe(kv["dose_val"]));
    setDoubleParam(P_gross_val[channel], parseDoubleSafe(kv["gross_val"]));
    setDoubleParam(P_net_val[channel], parseDoubleSafe(kv["net_val"]));
    setDoubleParam(P_bkg_val[channel], parseDoubleSafe(kv["bkg_val"]));
    setDoubleParam(P_unc_abs[channel], parseDoubleSafe(kv["uncertainty_abs"]));
    setDoubleParam(P_unc_rel[channel], parseDoubleSafe(kv["uncertainty_rel"]));
    setDoubleParam(P_detection_limit[channel], parseDoubleSafe(kv["detection_limit"]));
    setDoubleParam(P_decision_thres[channel], parseDoubleSafe(kv["decision_thres"]));
    setDoubleParam(P_best_est[channel], parseDoubleSafe(kv["best_est"]));
    setDoubleParam(P_unc_best_est[channel], parseDoubleSafe(kv["unc_best_est"]));
    setDoubleParam(P_lower_conf[channel], parseDoubleSafe(kv["lower_limit_conf"]));
    setDoubleParam(P_upper_conf[channel], parseDoubleSafe(kv["upper_limit_conf"]));
    setDoubleParam(P_calib_factor[channel], parseDoubleSafe(kv["calib_factor"]));
    setDoubleParam(P_delta_scint[channel], parseDoubleSafe(kv["delta_scint_factor"]));
    setDoubleParam(P_alarm1[channel], parseDoubleSafe(kv["alarm_limit1"]));
    setDoubleParam(P_alarm2[channel], parseDoubleSafe(kv["alarm_limit2"]));
    setDoubleParam(P_alarm3[channel], parseDoubleSafe(kv["alarm_limit3"]));
    setDoubleParam(P_alarm4[channel], parseDoubleSafe(kv["alarm_limit4"]));
    setStringParam(P_unit_meas[channel], kv["unit_meas_val"]);
    setStringParam(P_unit_dose[channel], kv["unit_dose_val"]);

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
