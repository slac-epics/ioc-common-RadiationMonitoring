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

## Setup asyn connections 
drvAsynIPPortConfigure("TS-B024","ts-b024-pp01:2101")

## Load record instances
#General

#Station B24 Test Stand
dbLoadRecords("db/asynRecord.db","P=RADM:AMS:B24,PORT=TS-B024")
dbLoadRecords("db/ams.template","P=RADM:AMS:B24,PORT=TS-B024")

cd ${TOP}/iocBoot/${IOC}
iocInit

