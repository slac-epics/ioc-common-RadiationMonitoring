#!../../bin/rhel6-x86_64/RadiationMonitoring

# Setup environment variables
< envPaths
epicsEnvSet("ENGINEER","Jacob DeFilippis")
epicsEnvSet("LOCATION","SLAC Perimeter")
epicsEnvSet("STREAM_PROTOCOL_PATH",".:${TOP}/protocols")

cd ${TOP}


## Register all support components
dbLoadDatabase "dbd/RadiationMonitoring.dbd"
RadiationMonitoring_registerRecordDeviceDriver pdbbase

## Setup asyn connections PM01-PM08
drvAsynIPPortConfigure("B24-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("B24-TS","wb-site-rm01:5001")
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("B24_FHT", "B24-TS", 1, 1.5)


## Load record instances
#General

#Station B24 Test Stand
dbLoadRecords("db/peripheral-station.db","UNIT=B24,GPORT=B24-GAMMA,NPORT=B24-FHT,M=1")

cd ${TOP}/iocBoot/${IOC}
iocInit

