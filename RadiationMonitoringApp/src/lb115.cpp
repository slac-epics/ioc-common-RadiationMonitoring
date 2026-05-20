#include "lb115.h"

LB115Driver::LB115Driver(const char *portName, const char *ipPort) :
    asynPortDriver(portName,
                   1,
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

    pasynUser = nullptr;
    asynStatus driverStatus = pasynOctetSyncIO->connect(ipPort, 0, &pasynUser, NULL);
    pasynOctetSyncIO->setOutputEos(pasynUser, "\r\n", 2);
    pasynOctetSyncIO->setInputEos(pasynUser, "\r", 1);

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

/**
 * @brief Creates asyn params for device general registers.
 */
void LB115Driver::initGeneralParameters() {
    createParam("PROGRAM_VER",     asynParamOctet, &P_program_version);
    createParam("KERNEL_VER",      asynParamOctet, &P_kernel_version);
    createParam("MAC_ADDR",        asynParamOctet, &P_mac_address);
    createParam("IP_ADDR",         asynParamOctet, &P_ip_address);
    createParam("SERIAL_NUM",      asynParamInt32, &P_serial_number);
    createParam("DHCP_STATUS",     asynParamInt32, &P_dhcp_status);
    createParam("PORT_NUM",        asynParamInt32, &P_port_number);
    createParam("SUBNET",          asynParamOctet, &P_subnet_mask);
    createParam("DEFAULT_GATEWAY", asynParamOctet, &P_default_gateway);
    createParam("DEVICE_STATUS",   asynParamInt32, &P_device_status);
    createParam("DEVICE_NAME",     asynParamOctet, &P_device_name);
}

/**
 * @brief Creates asyn params for device channel registers for all 4 channels.
 *
 * There are 4 channels with 4 registers of interest each. The first three
 * return easily parsable scalar values of interst, but the final register
 * returns a large dictionary of key:value pairs that needs to be parsed
 * and put into pvs. 
 */
void LB115Driver::initChannelParameters() {

    for (int ch = 0; ch < MAX_CH; ch++) {
    
        char name[64];
        // The simple register parameters:
        snprintf(name, sizeof(name), "CH%d_CURR_MEAS", ch+1);
        createParam(name, asynParamFloat64, &P_curr_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_OLDEST_MEAS", ch+1);
        createParam(name, asynParamFloat64, &P_oldest_meas[ch]);
        snprintf(name, sizeof(name), "CH%d_FIFO_MEAS_STATUS", ch+1);
        createParam(name, asynParamInt32, &P_fifo_meas_status[ch]);
        // The single key:value dictionary register parameters:
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

/**
 * @brief Polling thread for both general and channel registers.
 */
void LB115Driver::pollerThread() {

    const double loopDelay = 0.25; //seconds
    
    while(running.load()) {
        // General registers are always being polled if the thread is active.
        for (int genReg = 0; genReg < 11; genReg++) {

            std::string cmd = buildGeneralCommand(genReg);

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
                processGeneralResponse(response, genReg);
                //printf("Poller REG %d RX (%zu): %s\n", genReg, nRead, response);
            }
        }
            
        
        // Reading the same register for all active channels before
        // moving onto the next register
        for (int channel = 0; channel < MAX_CH; channel++) {
            if (!chState[channel].enabled) {
                epicsThreadSleep(0.05);
                continue;
            }

            int ch = channel + 1;
            int chReg = CH_REG_LIST[chState[channel].regIndex];
            std::string cmd = buildChannelCommand(ch, chReg);
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
                processChannelResponse(response, ch, chReg);
            }
            // advance state machine
            chState[channel].regIndex = (chState[channel].regIndex + 1) % NUM_REG;
        }
        epicsThreadSleep(loopDelay);
    }
}

const int LB115Driver::CH_REG_LIST[NUM_REG] = {0,1,7,9};

/*
 * LB 115 Tranfer Pattern:
 * TDV FDV PRC MBR TYP ITM (MSK Data) *CSM*CR
 * where:
 *   * TDV --> logical address of data logger (0001-9999)
 *   * FDV --> logical address of host computer (9001-9008)
 *   * PRC --> process (00-03)
 *   * MBR --> member (00-08) 
 *   * TYP --> type (00-03)
 *   * ITM --> item (00-30) 
 *   * MSK --> mask on data (000000-FFFFFF) (not used)
 *   * Data --> data (variable length)
 *   * CSM --> checksum (empty if no checksum)
 *   * CR --> carriage return ** DEFINED EARLIER IN CONNECTION TO PORT **
 *
 * Notes:
 * 1) The asterisks "*" must always be included, even if CSM is empty
 * 2) The LB 115 send the frame header with inverted TDV, FDV
 * 3) The LB 115 does not respond to requests if:
 *    - TDV does not match ID number
 *    - FDV is not between 9001 and 9008
 *    - The transfer pattern is not correct
 *    - The checksum is not correct
 */

/**
 * @brief Builds command for general registers based off of transfer pattern above.
 *
 * @param reg - register of interest (00-10)
 */
std::string LB115Driver::buildGeneralCommand(int reg) {
    char buf[64];
    snprintf(buf, sizeof(buf), "*00019001030101%02d**", reg);
    return std::string(buf);
}

/**
 * @brief Builds command for channel registers based off of transfer pattern above.
 *
 * @param channel - channel of interest (00-03)
 * @param reg     - register of interest (00-10)
 */
std::string LB115Driver::buildChannelCommand(int channel, int reg) {
    char buf[64];
    snprintf(buf, sizeof(buf), "*0001900101%02d01%02d**", channel, reg);
    return std::string(buf);
}

/**
 * @brief Sends command to LB115 device and receives data in a buffer.
 *
 * @param outCmd - command that is sent to device
 * @param inBuf  - buffer received from device
 * @param nRead  - size of the buffer received 
 */
asynStatus LB115Driver::sendAndReceive(const char* outCmd, char* inBuf, size_t& nRead) {

    size_t nWritten = 0;
    int eomReason = 0;

    if (!pasynUser || !pasynOctetSyncIO) {
        printf("pasynUser is null - not connected\n");
        return asynError;
    }
    asynStatus status = pasynOctetSyncIO->writeRead(
            pasynUser,
            outCmd,
            strlen(outCmd),
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
    //if (nRead >= MAX_MSG){
    //    nRead = MAX_MSG -1;
    //}
    //inBuf[nRead] = '\0';

    asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "TS: %s\nRX: %s\n", outCmd, inBuf);
    std::string resp(inBuf, nRead);
    
    return asynSuccess;
}

/**
 * @brief Creates dictionary map of channel register 9's data stream
 *
 * @param data - data with transmission header and checksum stripped off (ex. *00019001010109<data>*CSM*)
 */
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

/**
 * @brief Replaces invalid ai epics datatypes with NAN.
 *
 * @param val - parameter value from transmission string
 */
double LB115Driver::parseDoubleSafe(const std::string& val) {
    if (val == "Tmout" || val.empty()) {
        return NAN;
    }
    return atof(val.c_str());
}

/**
 * @brief Replaces invalid ai epics datatypes with safe 16-bit integer.
 *
 * @param val - parameter value from transmission string
 */
int LB115Driver::parseHexSafe(const std::string& val) {
    return strtol(val.c_str(), NULL, 16);
}

/**
 * @brief Sets EPICS pvs for given registers.
 *
 * @param resp - full returned response from LB115 (ex. *00019001010101<data>*CSM*)
 * @param reg  - device register
 */
void LB115Driver::processGeneralResponse(const std::string& resp, int reg) {
    
    std::string payload = parsePayload(resp);

    switch(reg) {
        case 0:
            setStringParam(P_program_version, payload.c_str());
            break;
        case 1:
            setStringParam(P_kernel_version, payload.c_str());
            break;
        case 2:
            setStringParam(P_mac_address, payload.c_str());
            break;
        case 3:
            setStringParam(P_ip_address, payload.c_str());
            break;
        case 4:
            setIntegerParam(P_serial_number, parseDoubleSafe(payload));
            break;
        case 5:
            setIntegerParam(P_dhcp_status, parseDoubleSafe(payload));
            break;
        case 6:
            setIntegerParam(P_port_number, parseDoubleSafe(payload));
            break;
        case 7:
            setStringParam(P_subnet_mask, payload.c_str());
            break;
        case 8:
            setStringParam(P_default_gateway, payload.c_str());
            break;
        case 9:
            setIntegerParam(P_device_status, parseHexSafe(payload));
            break;
        case 10:
            setStringParam(P_device_name, payload.c_str());
            break;
    }
    callParamCallbacks();
}

/**
 * @brief Sets EPICS pvs for given channel registers.
 *
 * @param resp    - full returned response from LB115 (ex. *00019001010101<data>*CSM*)
 * @param channel - device channel
 * @param reg     - device register
 */
void LB115Driver::processChannelResponse(const std::string& resp, int channel, int reg) {
    int ind = channel - 1;
    std::string payload;

    switch(reg) {
        case 0: { 
            payload = parsePayload(resp); 
            setDoubleParam(P_curr_meas[ind], parseDoubleSafe(payload));
            break;
                }
        case 1: {
            payload = parsePayload(resp); 
            setDoubleParam(P_oldest_meas[ind], parseDoubleSafe(payload));
            break;
                }
        case 7: {
            payload = parsePayload(resp); 
            setIntegerParam(P_fifo_meas_status[ind], parseHexSafe(payload));
            break;
                }
        case 9: {
            payload = parsePayload(resp);

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
            break;
            }
    }
    callParamCallbacks();
}

/**
 * @brief Strips transmission header and checksum from payload
 *
 * @param resp - full returned response from LB115 (ex. *00019001010101<data>*CSM*)
 */
std::string LB115Driver::parsePayload(const std::string& resp) {
    size_t start = resp.find('*');
    size_t end = resp.find('*', start + 1);
    
    if (start == std::string::npos || end == std::string::npos)
        return "";

    std::string body = resp.substr(start + 1, end - start - 1);
    const size_t HEADER_LEN = 16; 

    if(body.size() <= HEADER_LEN)
        return "";

    return body.substr(HEADER_LEN);
}

/**
 * @brief Enables a channel to be polled in the thread:
 *
 * @param channel - device channel 
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
        new LB115Driver(portName, ipPort);
        return 0;
    }
    static const iocshArg arg0 = {"portName", iocshArgString};
    static const iocshArg arg1 = {"ipPort", iocshArgString};
    static const iocshArg * args[] = {&arg0, &arg1};
    static const iocshFuncDef funcDef = {"lb115Configure", 2, args};

    static void funcCall(const iocshArgBuf *args) {
        LB115Configure(args[0].sval, args[1].sval);
    }

    void LB115ConfigureRegister(void) {
        iocshRegister(&funcDef, funcCall);
    }

    epicsExportRegistrar(LB115ConfigureRegister);
}

extern "C" {
    int LB115EnableChannel(const char* portName, int channel) {
        printf("Enabling Channel: %0d\n", channel);

        LB115Driver *driver = NULL;
        driver = (LB115Driver*) findAsynPortDriver(portName);
        //pC = (asynMotorController*) findAsynPortDriver(portName);
        
        if (!driver) {
            printf("No LB115 port named %s\n", portName);
            return -1;
        }

        driver->enableChannel(channel);

        return 0;
    }
    static const iocshArg enableArg0 = {"portName", iocshArgString};
    static const iocshArg enableArg1 = {"channelNum", iocshArgInt};
    static const iocshArg * enableArgs[] = {&enableArg0, &enableArg1};
    static const iocshFuncDef enable_funcDef = {"lb115EnableChannel", 2, enableArgs};

    static void enable_funcCall(const iocshArgBuf *enableArgs) {
        LB115EnableChannel(enableArgs[0].sval, enableArgs[1].ival);
    }

    void LB115EnableChannelRegister(void) {
        iocshRegister(&enable_funcDef, enable_funcCall);
    }

    epicsExportRegistrar(LB115EnableChannelRegister);

}
