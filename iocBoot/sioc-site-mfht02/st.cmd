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
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Z. Huang")
epicsEnvSet("LOCATION",    "testfac-daemon1")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:MFHT02")
epicsEnvSet("EPICS_CA_SERVER_PORT", "5500")

# Load common piece of startup script
< ../common/st.cmd.soft

## Set up asyn connections
drvAsynIPPortConfigure("MFHT2-TS","wb-site-mfht02:5000")

## Load record instances
dbLoadRecords("db/mfht-station.db","UNIT=MFHT02,FPORT=MFHT2-TS")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:MFHT02:,R=NASYN,PORT=MFHT2-TS,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

