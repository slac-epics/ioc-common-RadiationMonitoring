#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  SLAC Site Radiation Monitoring Controls
#
#  Auth: 07-Jul-2021, Mike Dunning       (mdunning)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini         (luchini):
#         load st.cmd.soft
#
#==============================================================
# 
# Setup environment variables
epicsEnvSet("ENGINEER" ,"Jacob DeFilippis")
epicsEnvSet("IOC_NAME" , sioc-site-rm01")
epicsEnvSet("LOCATION" ,"SLAC Perimeter")

# Load common piece of startup script
< ../common/st.cmd.soft

## Setup asyn connections PM01-PM08
#drvAsynIPPortConfigure("PM1-GAMMA","wb-site-rm01:5000")
drvAsynIPPortConfigure("PM1-TS","wb-site-rm01:5001")
drvFHTConfigure("PM1-FHT6020", "PM1-TS", 1, 1.5)

#
#drvAsynIPPortConfigure("PM2-GAMMA","wb-site-rm02:5000")
#drvAsynIPPortConfigure("PM2-FHT6020","wb-site-rm02:5001")
#
#drvAsynIPPortConfigure("PM3-GAMMA","wb-site-rm03:5000")
drvAsynIPPortConfigure("PM3-TS","wb-site-rm03:5001")
drvFHTConfigure("PM3-FHT6020", "PM3-TS", 1, 1.5)
#
#drvAsynIPPortConfigure("PM4-GAMMA","wb-site-rm04:5000")
#drvAsynIPPortConfigure("PM4-FHT6020","wb-site-rm04:5001")
#
drvAsynIPPortConfigure("PM5-GAMMA","wb-site-rm05:5000")
drvAsynIPPortConfigure("PM5-TS","wb-site-rm05:5001")
drvFHTConfigure("PM5-FHT6020", "PM5-TS", 1, 1.5)

#drvAsynIPPortConfigure("PM6-GAMMA","wb-site-rm06:5000")
#drvAsynIPPortConfigure("PM6-FHT6020","wb-site-rm06:5001")

#Station 7 over cable
drvAsynIPPortConfigure("PM7-GAMMA","gamma-pm7-pp01:5000")
drvAsynIPPortConfigure("PM7-TS","ts-pm7-pp01:5001")
drvFHTConfigure("PM7-FHT6020", "PM7-TS", 1, 1.5)

drvAsynIPPortConfigure("PM8-GAMMA","wb-site-rm08:5000")
drvAsynIPPortConfigure("PM8-TS","wb-site-rm08:5001")
drvFHTConfigure("PM8-FHT6020", "PM8-TS", 1, 1.5)

#Prototype mobile station
drvAsynIPPortConfigure("PM9-GAMMA","moxa-test:5000")
drvAsynIPPortConfigure("PM9-TS","moxa-test:5001")
drvFHTConfigure("PM9-FHT6020", "PM9-TS", 1, 1.5)

#Stations PM01-PM8
dbLoadRecords("db/peripheral-station.db","UNIT=01,GPORT=PM1-GAMMA,NPORT=PM1-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=02,GPORT=PM2-GAMMA,NPORT=PM2-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=03,GPORT=PM3-GAMMA,NPORT=PM3-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=04,GPORT=PM4-GAMMA,NPORT=PM4-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=05,GPORT=PM5-GAMMA,NPORT=PM5-FHT6020")
#dbLoadRecords("db/peripheral-station.db","UNIT=06,GPORT=PM6-GAMMA,NPORT=PM6-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=07,GPORT=PM7-GAMMA,NPORT=PM7-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=08,GPORT=PM8-GAMMA,NPORT=PM8-FHT6020")
dbLoadRecords("db/peripheral-station.db","UNIT=09,GPORT=PM9-GAMMA,NPORT=PM9-FHT6020")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:08:,R=NASYN,PORT=PM8-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:08:,R=GASYN,PORT=PM8-GAMMA,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=NASYN,PORT=PM9-TS,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=RADM:SITE:09:,R=GASYN,PORT=PM9-GAMMA,ADDR=0,IMAX=100,OMAX=100")

dbLoadRecords("db/radm_info.template","P=RADM:SITE:07,LOC=07")
dbLoadRecords("db/radm_info.template","P=RADM:SITE:09,LOC=09")

cd ${TOP}/iocBoot/${IOC}
iocInit

# End of file
