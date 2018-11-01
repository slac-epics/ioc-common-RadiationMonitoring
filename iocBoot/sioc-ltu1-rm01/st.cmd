#!../../bin/rhel6-x86_64/RadiationMonitoring

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("IOC_NAME", "SIOC:LTU1:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

# Configure communication port
drvAsynIPPortConfigure("RADM_LTU1_998", "ts-b913-nw02:2010", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_LTU1_998", "RADM_LTU1_998", 1, 1.5)

# Asyn diagnostics
#asynSetTraceMask("RADM_LTU1_998", -1, 0x9)
#asynSetTraceIOMask("RADM_LTU1_998", -1, 0x5)

# Load record instances
dbLoadRecords("db/sioc-ltu1-rm01.db", "P=RADM:LTU1:998, PORT=FHT_LTU1_998")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



