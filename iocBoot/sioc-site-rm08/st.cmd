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

## Setup asyn connections PM8
drvAsynIPPortConfigure("PM8-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM8-TS","wb-site-rm01:5001")
drvFHTConfigure("PM8-FHT6020", "PM8-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM08")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM08")

#Stations PM8
dbLoadRecords("db/peripheral-station.db","UNIT=08,GPORT=PM8-GAMMA,NPORT=PM8-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:08:,R=NASYN,PORT=PM8-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:08:,R=GASYN,PORT=PM8-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

