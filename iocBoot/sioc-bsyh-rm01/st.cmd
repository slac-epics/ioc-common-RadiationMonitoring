#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  Startup Script for Thermo FHT Rad Mon device(s)
#        BTH/UND (RDMBTH01)  BTH
#
#  Name: st.cmd
#
#  Facility:  LCLS Radiation Monitoring Controls
#
#  Auth: 21-Jul-2017, M. Dunning          (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini          (luchini):
#         add caPutLogInit after iocInit
#         add env var LOCATION
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "lcls-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:BSYH:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

# Configure communication port
drvAsynIPPortConfigure("RADM_BSYH_843", "ts-bsy0-pp01:2001", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_BSYH_843", "RADM_BSYH_843", 1, 1.5)

# Asyn diagnostics
#asynSetTraceMask("RADM_BSYH_843", -1, 0x9)
#asynSetTraceIOMask("RADM_BSYH_843", -1, 0x5)

# Load record instances
dbLoadRecords("db/sioc-bsyh-rm01.db")
dbLoadRecords("db/asynRecord.db","P=$(IOC_NAME):,R=Asyn, PORT=RADM_BSYH_843, ADDR=0, IMAX=0, OMAX=0")

# Restore PVs
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



