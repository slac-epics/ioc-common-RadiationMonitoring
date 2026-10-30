#!../../bin/rhel9-x86_64/RadiationMonitoring

#==============================================================
#  Abs:  EPICS startup script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  LCLS Radiation Monitoring Controls
#
#  Auth: 22-Jan-2020, Mike Dunning       (mdunning)
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "lcls-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:LTU0:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

### Thermo FHT Radmon -------------------------------------------------------
epicsEnvSet("FHT_P",      "RADM:LTU0:998")
epicsEnvSet("FHT_PORT",   "RADM_LTU0_998")
epicsEnvSet("FHT_LOC",    "LTU near TDUND")
epicsEnvSet("FHT_TS",     "ts-b913-nw02")
epicsEnvSet("FHT_TSPORT", "2010")
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

