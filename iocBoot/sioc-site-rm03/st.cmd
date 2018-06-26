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

## Setup asyn connections PM3
drvAsynIPPortConfigure("PM3-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM3-TS","wb-site-rm01:5001")
drvFHTConfigure("PM3-FHT6020", "PM3-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM03")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM03")

#Stations PM3
dbLoadRecords("db/peripheral-station.db","UNIT=03,GPORT=PM3-GAMMA,NPORT=PM3-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:03:,R=NASYN,PORT=PM3-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:03:,R=GASYN,PORT=PM3-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

