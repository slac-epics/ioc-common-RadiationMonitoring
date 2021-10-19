#!../../bin/rhel6-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  FACET-II Radiation Monitoring Controls
#
#  Auth: 07-Jul-2021, Mike Dunning       (mdunning)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini         (luchini):
#         add caPutLogInit after iocInit
#
#==============================================================
# 
# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "facet-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:LI20:RM01")
#
epicsEnvSet("FHT1_P",      "RADM:LI20:1")
epicsEnvSet("FHT1_LOC",    "FACET_LI20")
epicsEnvSet("FHT1_TS",     "ts-li20-nw03")
epicsEnvSet("FHT1_TSPORT", "2109")
epicsEnvSet("FHT1_PORT",   "FHT1")

# Load common piece of startup script
< ../common/st.cmd.soft

### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT1_PORT),TS=$(FHT1_TS),TSPORT=$(FHT1_TSPORT)")

# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT1_P),PORT=$(FHT1_PORT),LOC=$(FHT1_LOC)")
dbLoadRecords("db/asynRecord.db","P=$(FHT1_P):,R=Asyn,PORT=$(FHT1_PORT),ADDR=0,IMAX=0,OMAX=0")
###--------------------------------------------------------------------------

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

