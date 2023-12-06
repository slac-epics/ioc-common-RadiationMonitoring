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
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Z. Huang")
epicsEnvSet("LOCATION",    "testfac-daemon1")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:PMS09")

# Load common piece of startup script
< ../common/st.cmd.soft

## Set up asyn connections
drvAsynIPPortConfigure("PM9-GAMMA","wb-site-rm09:5000")
drvAsynIPPortConfigure("PM9-TS","wb-site-rm09:5001")

## Load record instances
dbLoadRecords("db/pms-station.db","UNIT=09,GPORT=PM9-GAMMA,FPORT=PM9-TS")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=NASYN,PORT=PM9-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=GASYN,PORT=PM9-GAMMA,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

