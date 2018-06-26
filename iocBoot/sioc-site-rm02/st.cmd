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

## Setup asyn connections PM2
drvAsynIPPortConfigure("PM2-GAMMA","wb-site-rm02:5000")
drvAsynIPPortConfigure("PM2-TS","wb-site-rm02:5001")
drvFHTConfigure("PM2-FHT6020", "PM2-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM02")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM02")

#Stations PM2
dbLoadRecords("db/peripheral-station.db","UNIT=02,GPORT=PM2-GAMMA,NPORT=PM2-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:02:,R=NASYN,PORT=PM2-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:02:,R=GASYN,PORT=PM2-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

