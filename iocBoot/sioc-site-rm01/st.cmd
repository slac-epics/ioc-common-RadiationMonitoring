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

## Setup asyn connections PM01-PM08
#drvAsynIPPortConfigure("PM1-GAMMA","wb-site-rm01:5000")
#drvAsynIPPortConfigure("PM1-FHT6020","wb-site-rm01:5001")
#
#drvAsynIPPortConfigure("PM2-GAMMA","wb-site-rm02:5000")
#drvAsynIPPortConfigure("PM2-FHT6020","wb-site-rm02:5001")
#
#drvAsynIPPortConfigure("PM3-GAMMA","wb-site-rm03:5000")
#drvAsynIPPortConfigure("PM3-FHT6020","wb-site-rm03:5001")
#
#drvAsynIPPortConfigure("PM4-GAMMA","wb-site-rm04:5000")
#drvAsynIPPortConfigure("PM4-FHT6020","wb-site-rm04:5001")
#
#drvAsynIPPortConfigure("PM5-GAMMA","wb-site-rm05:5000")
#drvAsynIPPortConfigure("PM5-FHT6020","wb-site-rm05:5001")
#
#drvAsynIPPortConfigure("PM6-GAMMA","wb-site-rm06:5000")
#drvAsynIPPortConfigure("PM6-FHT6020","wb-site-rm06:5001")

#Station 7 over cable
drvAsynIPPortConfigure("PM7-GAMMA","gamma-pm7-pp01:5000")
drvAsynIPPortConfigure("PM7-TS","ts-pm7-pp01:5001")
drvFHTConfigure("PM7-FHT6020", "PM7-TS", 1, 1.5)

drvAsynIPPortConfigure("PM8-GAMMA","wb-site-rm08:5000")
drvAsynIPPortConfigure("PM8-FHT6020","wb-site-rm08:5001")
drvFHTConfigure("PM8-FHT6020", "PM8-TS", 1, 1.5)

#Prototype mobile station
drvAsynIPPortConfigure("PM9-GAMMA","moxa-test:5004")
drvAsynIPPortConfigure("PM9-TS","moxa-test:5003")
drvFHTConfigure("PM9-FHT6020", "PM9-TS", 1, 1.5)


## Load record instances
#General
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SYS8:RM01")

#Stations PM01-PM8
#dbLoadRecords("db/peripheral-station.db","UNIT=01,GPORT=PM1-GAMMA,NPORT=PM1-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=02,GPORT=PM2-GAMMA,NPORT=PM2-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=03,GPORT=PM3-GAMMA,NPORT=PM3-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=04,GPORT=PM4-GAMMA,NPORT=PM4-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=05,GPORT=PM5-GAMMA,NPORT=PM5-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=06,GPORT=PM6-GAMMA,NPORT=PM6-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=07,GPORT=PM7-GAMMA,NPORT=PM7-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=08,GPORT=PM8-GAMMA,NPORT=PM8-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=09,GPORT=PM9-GAMMA,NPORT=PM9-FHT6020")

dbLoadRecords("db/radm_info.template","P=RADM:SITE:07,LOC=07")
dbLoadRecords("db/radm_info.template","P=RADM:SITE:09,LOC=09")

dbLoadRecords("db/station.template","STATION=07")
dbLoadRecords("db/station.template","STATION=09")

cd ${TOP}/iocBoot/${IOC}
iocInit

