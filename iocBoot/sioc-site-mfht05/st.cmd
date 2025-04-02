#!../../bin/rhel7-x86_64/RadiationMonitoring

#==============================================================
#
#  Abs:  EPICS startup script for Moveable FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 02-Apr-2025, M. Dunning (mdunning)
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "M. Dunning")
epicsEnvSet("LOCATION",    "testfac-daemon1")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:MFHT05")

# Load common piece of startup script
< ../common/st.cmd.soft

## Set up asyn connections
drvAsynIPPortConfigure("MFHT5-TS","wb-site-mfht05:5000")

## Load record instances
dbLoadRecords("db/mfht-station.db","UNIT=MFHT05,FPORT=MFHT5-TS")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:MFHT05:,R=NASYN,PORT=MFHT5-TS,ADDR=0,IMAX=100,OMAX=100")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

