/*#==============================================================
#
#  Abs: C++ header file for Thermo FHT driver
#
#  Name: drvFHT.h
#
#  Desc: This file should be included in drvFHT.cpp
#
#  Facility:  LCLS2
#
#  Auth: 21-Jul-2017, M. Dunning (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       dd-mmm-yyyy, Name           (USERNAME):
#          comment
#
#==============================================================*/

#include "asynPortDriver.h"

#define snString                     "SN"                        /* asynOctet,        r   */
#define fwString                     "FW"                        /* asynOctet,        r   */
#define devTypeString                "DEV_TYPE"                  /* asynOctet,        r   */
#define devTimeString                "DEV_TIME"                  /* asynOctet,        r   */
#define devAddrString                "DEV_ADDR"                  /* asynInt32,        r   */
#define systemStatusString           "STATUS_SYS"                /* asynInt32,        r   */
#define chanStateString              "CHAN_STATE"                /* asynInt32,        r   */
#define chanModeString               "CHAN_MODE"                 /* asynInt32,        r   */
#define chanChanString               "CHAN_CHAN"                 /* asynInt32,        r   */
#define chanUnitsString              "CHAN_UNITS"                /* asynInt32,        r   */
#define chanPreunitString            "CHAN_PREUNIT"              /* asynInt32,        r   */
#define chanTypeString               "CHAN_TYPE"                 /* asynInt32,        r   */
#define chanPortString               "CHAN_PORT"                 /* asynInt32,        r   */
#define chanDecodeString             "CHAN_DECODE"               /* asynInt32,        r   */
#define chanAddressString            "CHAN_ADDR"                 /* asynInt32,        r   */
#define chanDisplayFactorString      "CHAN_DISP_FACT"            /* asynFloat64,      r   */
#define chanReadallString            "CHAN_READALL"              /* asynInt32,        r   */
#define pollIntervalString           "POLL_INT"                  /* asynInt32,        r   */
#define measString                   "MEAS"                      /* asynFloat64,      r   */
#define measStatusString             "MEAS_STATUS"               /* asynInt32,        r   */
#define meas40GString                "MEAS_40G"                  /* asynFloat64,      r   */
#define measStatus40GString          "MEAS_STATUS_40G"           /* asynInt32,        r   */
#define measTime40GString            "MEAS_TIME_40G"             /* asynFloat64,      r   */
#define type40GString                "TYPE_40G"                  /* asynInt32,        r   */
#define calFact40GString             "CAL_FACT_40G"              /* asynFloat64,      r   */
#define deadTime40GString            "DEAD_TIME_40G"             /* asynFloat64,      r   */
#define dynRange40GString            "DYN_RNG_40G"               /* asynFloat64,      r   */
#define swThresh40GString            "SW_THR_40G"                /* asynFloat64,      r   */
#define calDate40GString             "CAL_DATE_40G"              /* asynOctet,      r   */
#define hwResetString                "HW_RESET"                  /* asynInt32,        r   */


class drvFHT : public asynPortDriver{
public:
    drvFHT(const char* port, const char* IOport, int addr, double timeout);
    virtual ~drvFHT();
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual void pollerThread();

protected:
    asynStatus _write(const char *buffer);
    asynStatus _writeRead(const char *buffer);
    void _readString(int function, const char *cmd);
    void _readInt(int function, const char *cmd, int addr);
    void _readStatus(int function, const char *cmd);
    void _readChan(const char *cmd, int addr);
    void _readChan40G(const char *cmd, int addr);
    void _readChanConfig(const char *cmd, int addr);
    void _readChanMode(const char *cmd);
    void _readUnits(const char *cmd, int addr);
    void _readCal(const char *cmd, int addr);

    int sn;
    int fw;
    int devType;
    int devTime;
    int devAddr;
    int systemStatus;
    int chanState;
    int chanMode;
    int chanChan;
    int chanUnits;
    int chanPreunit;
    int chanType;
    int chanPort;
    int chanDecode;
    int chanAddress;
    int chanDisplayFactor;
    int chanReadall;
    int pollInterval;
    int meas;
    int measStatus;
    int meas40G;
    int measStatus40G;
    int measTime40G;
    int type40G;
    int calFact40G;
    int deadTime40G;
    int dynRange40G;
    int swThresh40G;
    int calDate40G;
    int hwReset;
    #define FIRST_COMMAND sn 
    #define LAST_COMMAND hwReset

private:
    asynUser *pasynUser;
    static const int REPLY_BUF_LEN = 256;
    char replyBuffer_[REPLY_BUF_LEN];
    int deviceAddr_;
    double timeout_;
    double pollTime_;
    int forceCallback_;
    bool running_;
    bool exited_;
    int err_count_;
    unsigned char _checksumCalc(const char* str);
};

#define N_PARAMS ((int)(&LAST_COMMAND - &FIRST_COMMAND + 1))


