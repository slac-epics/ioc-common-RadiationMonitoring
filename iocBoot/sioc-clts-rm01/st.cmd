#!../../bin/rhel6-x86_64/RadiationMonitoring

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("IOC_NAME", "SIOC:CLTS:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

# Configure communication port
drvAsynIPPortConfigure("RADM_CLTS_585", "ts-bsy0-pp02:2001", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_CLTS_585", "RADM_CLTS_585", 1, 1.5)

# Asyn diagnostics
#asynSetTraceMask("RADM_CLTS_585", -1, 0x9)
#asynSetTraceIOMask("RADM_CLTS_585", -1, 0x5)

# Load record instances
dbLoadRecords("db/sioc-clts-rm01.db", "P=RADM:CLTS:585, PORT=FHT_CLTS_585")
dbLoadRecords("db/asynRecord.db","P=$(IOC_NAME):,R=Asyn, PORT=RADM_CLTS_585, ADDR=0, IMAX=0, OMAX=0")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Start autosave
< ../common/start_restore.cmd.soft

# End of file

