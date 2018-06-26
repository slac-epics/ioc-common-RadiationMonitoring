#!../../bin/rhel6-x86_64/RadiationMonitoring

# Setup environment variables
< envPaths
epicsEnvSet("ENGINEER","Jacob DeFilippis")
epicsEnvSet("LOCATION","SLAC Perimeter")
epicsEnvSet("STREAM_PROTOCOL_PATH",".:${TOP}/protocols")

cd ${TOP}


## Register all support components
dbLoadDatabase "dbd/RadiationMonitoring.dbd"
RadiationMonitoring_registerRecordDeviceDriver pdbbase

## Setup asyn connections PM7
drvAsynIPPortConfigure("PM7-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM7-TS","wb-site-rm01:5001")
drvFHTConfigure("PM7-FHT6020", "PM7-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM07")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM07")

#Stations PM7
dbLoadRecords("db/peripheral-station.db","UNIT=07,GPORT=PM7-GAMMA,NPORT=PM7-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:07:,R=NASYN,PORT=PM7-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:07:,R=GASYN,PORT=PM7-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

