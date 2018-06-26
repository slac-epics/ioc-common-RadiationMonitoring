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

## Setup asyn connections PM6
drvAsynIPPortConfigure("PM6-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM6-TS","wb-site-rm01:5001")
drvFHTConfigure("PM6-FHT6020", "PM6-TS", 1, 1.5)

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:RM06")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:RM06")

#Stations PM6
dbLoadRecords("db/peripheral-station.db","UNIT=06,GPORT=PM6-GAMMA,NPORT=PM6-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:06:,R=NASYN,PORT=PM6-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:06:,R=GASYN,PORT=PM6-GAMMA,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

