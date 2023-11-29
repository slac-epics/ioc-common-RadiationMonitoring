#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Moveable FHT Rad Mon device(s)
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
epicsEnvSet("IOC_NAME",    "SIOC:SITE:MFHT01")
epicsEnvSet("EPICS_CA_SERVER_PORT", "5501")

# Load common piece of startup script, this include STREAM_PROTOCOL_PATH
< ../common/st.cmd.soft



## Setup asyn connections PM1
drvAsynIPPortConfigure("MFHT1-TS","wb-site-mams01:5000")
#drvAsynIPPortConfigure("MFHT1-TS","wb-site-mfht01:5000")

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:MFHT01")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:MFHT01")

#Stations MFHT1
dbLoadRecords("db/mfht-station.db","UNIT=MFHT01,FPORT=MFHT1-TS")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:MFHT01:,R=NASYN,PORT=MFHT1-TS,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file


