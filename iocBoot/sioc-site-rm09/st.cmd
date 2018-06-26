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

## Setup asyn connections PM9
drvAsynIPPortConfigure("PM9-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM9-TS","wb-site-rm01:5001")
drvFHTConfigure("PM9-FHT6020", "PM9-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM09")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM09")

#Stations PM9
dbLoadRecords("db/peripheral-station.db","UNIT=09,GPORT=PM9-GAMMA,NPORT=PM9-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=NASYN,PORT=PM9-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=GASYN,PORT=PM9-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

