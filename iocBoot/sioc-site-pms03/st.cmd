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
epicsEnvSet("IOC_NAME",    "SIOC:SITE:PMS03")

# Load common piece of startup script
< ../common/st.cmd.soft

## Set up asyn connections
drvAsynIPPortConfigure("PM3-GAMMA","wb-site-rm03:5000")
drvAsynIPPortConfigure("PM3-TS","wb-site-rm03:5001")

## Load record instances
dbLoadRecords("db/pms-station.db","UNIT=03,GPORT=PM3-GAMMA,FPORT=PM3-TS")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:03:,R=NASYN,PORT=PM3-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:03:,R=GASYN,PORT=PM3-GAMMA,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

