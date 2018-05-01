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
drvAsynIPPortConfigure("TS-B024","ts-b024-pp01:2001")

## Load record instances
#General

#Station B24 Test Stand
#dbLoadRecords("db/ams-printer.template","P=RADM:AMS:B24,PORT=TS-B024")
#dbLoadRecords("db/asynRecord.db","P=RADM:AMS:B24:,R=ASYN,PORT=TS-B024,ADDR=1,IMAX=100,OMAX=100")

dbLoadRecords("db/eco-os-6.template","P=RADM:AMS:B24,PORT=TS-B024")
dbLoadRecords("db/asynRecord.db","P=RADM:AMS:B24:,R=ASYN,PORT=TS-B024,ADDR=1,IMAX=100,OMAX=100")

cd ${TOP}/iocBoot/${IOC}
iocInit

