#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Air Monitoring System Rad Mon device(s)
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
epicsEnvSet("IOC_NAME",    "SIOC:SITE:AMS06")

# Load common piece of startup script, this include STREAM_PROTOCOL_PATH
< ../common/st.cmd.soft



## Setup asyn connections PM1
drvAsynIPPortConfigure("AMS4","wb-site-ams06:5000")
drvAsynIPPortConfigure("ECO","wb-site-ams06:5001")
#asynSetOption("AMS4",0, "disconnectOnReadTimeout", "Y")

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:AMS06")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:AMS06")

#Stations PM3
dbLoadRecords("db/ams-station.db","UNIT=06,APORT=AMS4,EPORT=ECO")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:06:,R=AASYN,PORT=AMS4,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:06:,R=EASYN,PORT=ECO,ADDR=0,IMAX=100,OMAX=100")
# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file


