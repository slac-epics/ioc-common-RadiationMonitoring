#include "lb115.h"

static std::mutex mtx;
LB115Driver* LB115Driver::_instance = nullptr;

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
                   6,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   asynFloat64Mask | asynInt32Mask | asynOctetMask,
                   0,
                   1,
                   0,
                   0), 
    _ipPort(ipPort)
{
    
    // Initializing Asyn Parameters:
    initGeneralParameters();

    pasynUser = nullptr;
    asynStatus driverStatus = pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);
    pasynOctetSyncIO->setInputEos(pasynUser, "*", 1);
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
    
    createParam("CURR_MEAS",        asynParamFloat64, &P_curr_meas);
    createParam("OLDEST_MEAS",      asynParamFloat64, &P_oldest_meas);
    createParam("FIFO_MEAS_STATUS", asynParamInt32, &P_fifo_meas_status);
    createParam("READ_INDEX",       asynParamOctet, &P_read_index);
    createParam("DETECTOR_NAME",    asynParamOctet, &P_detector_name);
    createParam("DETECTOR_TAG",     asynParamOctet, &P_detector_tag);
    createParam("MEAS_ID",          asynParamOctet, &P_meas_id);
    createParam("MEAS_ID_NAME",     asynParamOctet, &P_meas_id_name);
    createParam("MEAS_DATE",        asynParamOctet, &P_meas_date);
    createParam("MEMORY_INDEX",     asynParamInt32, &P_memory_index);
    createParam("DOSE_TIME",        asynParamInt32, &P_dose_time);        // seconds
    createParam("MEAS_TIME",        asynParamInt32, &P_meas_time);        // seconds
    createParam("BKG_MEAS_TIME",    asynParamInt32, &P_bkg_meas_time);    // seconds
    createParam("MEAS_STATUS",      asynParamInt32, &P_meas_status);
    createParam("STATUS",           asynParamInt32, &P_status);
    createParam("MEAS_VAL",         asynParamFloat64, &P_meas_val);       // mrem/h
    createParam("MAX_MEAS_VAL",     asynParamFloat64, &P_max_meas_val);
    createParam("DOSE_VAL",         asynParamFloat64, &P_dose_val);       // mrem
    createParam("GROSS_VAL",        asynParamFloat64, &P_gross_val);
    createParam("NET_VAL",          asynParamFloat64, &P_net_val);
    createParam("BKG_VAL",          asynParamFloat64, &P_bkg_val);
    createParam("UNCERTAINTY_ABS",  asynParamFloat64, &P_unc_abs);
    createParam("UNCERTAINTY_REL",  asynParamFloat64, &P_unc_rel);
    createParam("DETECTION_LIMIT",  asynParamFloat64, &P_detection_limit);
    createParam("DECISION_THRES",   asynParamFloat64, &P_decision_thres);
    createParam("BEST_EST",         asynParamFloat64, &P_best_est);
    createParam("UNC_BEST_EST",     asynParamFloat64, &P_unc_best_est);
    createParam("LOWER_LIMIT_CONF", asynParamFloat64, &P_lower_conf);
    createParam("UPPER_LIMIT_CONF", asynParamFloat64, &P_upper_conf);
    createParam("CALIB_FACTOR",     asynParamFloat64, &P_calib_factor);
    createParam("DELTA_SCINT",      asynParamFloat64, &P_delta_scint);
    createParam("ALARM_LIM1",       asynParamFloat64, &P_alarm1);
    createParam("ALARM_LIM2",       asynParamFloat64, &P_alarm2);
    createParam("ALARM_LIM3",       asynParamFloat64, &P_alarm3);
    createParam("ALARM_LIM4",       asynParamFloat64, &P_alarm4);
    createParam("UNIT_MEAS_VAL",    asynParamOctet, &P_unit_meas);
    createParam("UNIT_DOSE_VAL",    asynParamOctet, &P_unit_dose);
}

void LB115Driver::generalPollerThread() {
    const double pollDelay = 1.0; // seconds

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

                //pasynOctetSyncIO->setInputEos(pasynUser, "", 0);
                //pasynOctetSyncIO->setOutputEos(pasynUser, "", 0);
                pasynOctetSyncIO->setInputEos(pasynUser, "*", 1);
                //pasynOctetSyncIO->setInputEos(pasynUser, "\r\n", 2);
                pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);
            }

            char response[MAX_MSG] = {0};
            size_t nRead = 0;
            
            const char* msg = "*0001900101010109**";

            printf("Poller TX: %s\n", msg);

            asynStatus status = sendAndReceive(msg, response, nRead);

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
    
    //inBuf[nRead] = '\0';
    if (nRead >= MAX_MSG){
        nRead = MAX_MSG -1;
    }
    inBuf[nRead] = '\0';

    //asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
    //          "TS: %s\nRX: %s\n", outMsg, inBuf);
    std::string resp(inBuf, nRead);
    processResponse(resp);
    
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

void LB115Driver::processResponse(const std::string& resp) {
    size_t start = resp.find('*');
    size_t end = resp.rfind('*');

    if (start == std::string::npos || end == std::string::npos || end <= start)
        return;

    std::string body = resp.substr(start + 1, end - start - 1);
    
    size_t headerEnd = body.find_first_not_of("0123456789");
    std::string payload = body.substr(headerEnd);
    
    auto kv = parseData(payload);
    

    printf("Memory Index: %d\n", parseHexSafe(kv["memory_index"]));
    printf("Detector Name: %i\n", kv["detector_name"]);
    printf("Measured Val: %f\n", parseDoubleSafe(kv["meas_val"]));
    printf("Alarm Limit 1: %f\n", parseDoubleSafe(kv["alarm_limit_1"]));
    printf("Alarm Limit 2: %f\n", parseDoubleSafe(kv["alarm_limit_2"]));
    printf("Alarm Limit 3: %f\n", parseDoubleSafe(kv["alarm_limit_3"]));
    printf("Alarm Limit 4: %f\n", parseDoubleSafe(kv["alarm_limit_4"]));
    printf("Status: %d\n", parseHexSafe(kv["status"]));
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
