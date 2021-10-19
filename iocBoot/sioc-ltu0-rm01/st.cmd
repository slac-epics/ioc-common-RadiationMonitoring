#!../../bin/rhel6-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  LCLS Radiation Monitoring Controls
#
#  Auth: 22-Jan-2020, Mike Dunning       (mdunning)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini         (luchini):
#         add caPutLogInit after iocInit
#         move STARTUP to common/st.cmd.soft
#         add env var LOCATION
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "lcls-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:LTU0:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

# Configure communication port
drvAsynIPPortConfigure("RADM_LTU0_998", "ts-b913-nw02:2010", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_LTU0_998", "RADM_LTU0_998", 1, 1.5)

# Asyn diagnostics
#asynSetTraceMask("FHT_LTU0_998", -1, 0x11)
asynSetTraceIOMask("FHT_LTU0_998", -1, 0x1)

# Load record instances
dbLoadRecords("db/sioc-ltu0-rm01.db", "P=RADM:LTU0:998, PORT=FHT_LTU0_998")
dbLoadRecords("db/asynRecord.db","P=$(IOC_NAME):,R=Asyn, PORT=RADM_LTU0_998, ADDR=0, IMAX=0, OMAX=0")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



