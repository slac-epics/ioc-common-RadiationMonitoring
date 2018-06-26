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

## Setup asyn connections PM5
drvAsynIPPortConfigure("PM5-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM5-TS","wb-site-rm01:5001")
drvFHTConfigure("PM5-FHT6020", "PM5-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM05")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM05")

#Stations PM5
dbLoadRecords("db/peripheral-station.db","UNIT=05,GPORT=PM5-GAMMA,NPORT=PM5-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:05:,R=NASYN,PORT=PM5-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:05:,R=GASYN,PORT=PM5-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

