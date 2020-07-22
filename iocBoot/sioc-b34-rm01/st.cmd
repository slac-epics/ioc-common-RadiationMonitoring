#!../../bin/rhel6-x86_64/RadiationMonitoring

< envPaths

cd ${TOP}

dbLoadDatabase "dbd/RadiationMonitoring.dbd"
RadiationMonitoring_registerRecordDeviceDriver pdbbase

# Configure communication port
drvAsynIPPortConfigure("RADM_B34_1", "ts-b34-nw99:2017", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_B34_1", "RADM_B34_1", 1, 1.5)

# Load record instances
dbLoadRecords("db/sioc-b34-rm01.db", "P=RADM:B34:1, PORT=FHT_B34_1")
dbLoadRecords("db/asynRecord.db","P=RADM:B34:1:,R=Asyn, PORT=RADM_B34_1, ADDR=0, IMAX=0, OMAX=0")

iocInit()


