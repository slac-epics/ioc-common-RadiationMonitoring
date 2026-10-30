#!../../bin/rhel9-x86_64/RadiationMonitoring

#==============================================================
#  Abs:  Startup Script for Thermo FHT Rad Mon device(s)
#        BTH/UND (RDMBTH01)  BTH
#
#  Name: st.cmd
#
#  Facility:  LCLS Radiation Monitoring Controls
#
#  Auth: 21-Jul-2017, M. Dunning          (MDUNNING)
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "lcls-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:BSYH:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

### Thermo FHT Radmon -------------------------------------------------------
epicsEnvSet("FHT_P",      "RADM:BSYH:843")
epicsEnvSet("FHT_PORT",   "RADM_BSYH_843")
epicsEnvSet("FHT_LOC",    "BSYH D2")
epicsEnvSet("FHT_TS",     "ts-bsy0-pp01")
epicsEnvSet("FHT_TSPORT", "2001")
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT_PORT),TS=$(FHT_TS),TSPORT=$(FHT_TSPORT)")
# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT_P),PORT=$(FHT_PORT),LOC=$(FHT_LOC)")
dbLoadRecords("db/asynRecord.db", "P=$(FHT_P):,R=Asyn,PORT=$(FHT_PORT),ADDR=0,IMAX=0,OMAX=0")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

# End of file

