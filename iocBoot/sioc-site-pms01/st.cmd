#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Peripheral Monitoring Station Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 29-Nov-2023, Ziyu Huang       (zyuang)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        28-Nov-2022, Z. Huang         (zyhuang):
#           migrate ioc to LCLS repo
#        19-Oct-2021, K. Luchini         (luchini):
#         move STARTUP to iocBoot/common/st.cmd.soft
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Z. Huang")
epicsEnvSet("LOCATION",    "testfac-daemon1")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:PMS01")

# Load common piece of startup script, this include STREAM_PROTOCOL_PATH
< ../common/st.cmd.soft



## Setup asyn connections PM1
drvAsynIPPortConfigure("PM1-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM1-TS","wb-site-rm01:5001")

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM01")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM01")

#Stations PM1
dbLoadRecords("db/pms-station.db","UNIT=01,GPORT=PM1-GAMMA,FPORT=PM1-TS")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:01:,R=NASYN,PORT=PM1-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:01:,R=GASYN,PORT=PM1-GAMMA,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file


