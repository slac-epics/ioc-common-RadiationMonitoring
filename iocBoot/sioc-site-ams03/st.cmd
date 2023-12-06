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
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Z. Huang")
epicsEnvSet("LOCATION",    "testfac-daemon1")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:AMS03")

# Load common piece of startup script
< ../common/st.cmd.soft

## Set up asyn connections
drvAsynIPPortConfigure("AMS3-TS-AMS4","wb-site-ams03:5000")
drvAsynIPPortConfigure("AMS3-TS-ECO","wb-site-ams03:5001")

## Load record instances
dbLoadRecords("db/ams-station.db","UNIT=03,APORT=AMS3-TS-AMS4,EPORT=AMS3-TS-ECO")
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:03:,R=AASYN,PORT=AMS3-TS-AMS4,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:03:,R=EASYN,PORT=AMS3-TS-ECO,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

