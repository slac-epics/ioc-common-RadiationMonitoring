#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Air Monitoring System Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 20-Aug-2021, Mike Dunning       (mdunning)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        28-Nov-2022, Z. Huang         (zyhuang):
#           migrate ioc to LCLS repo
#        19-Oct-2021, K. Luchini         (luchini):
#         move STARTUP to iocBoot/common/st.cmd.soft
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Z. Huang")
epicsEnvSet("LOCATION",    "Test Fac Server")
epicsEnvSet("IOC_NAME",    "SIOC:SITE:AMS05")

# Load common piece of startup script, this include STREAM_PROTOCOL_PATH
< ../common/st.cmd.soft

## Register all support components
dbLoadDatabase "dbd/RPMonitoring.dbd"
RPMonitoring_registerRecordDeviceDriver pdbbase

## Setup asyn connections PM1
drvAsynIPPortConfigure("AMS5-TS-AMS4","wb-site-ams05:5000")
drvAsynIPPortConfigure("AMS5-TS-ECO","wb-site-ams05:5001")

## Load record instances
# =====================================================================
# Load iocAdmin
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=SIOC:SITE:AMS05")
dbLoadRecords("db/iocRelease.db","IOC=SIOC:SITE:AMS05")

#Stations PM3
dbLoadRecords("db/ams-station.db","UNIT=05,APORT=AMS5-TS-AMS4,EPORT=AMS5-TS-ECO")

#Asyn Debugging Records
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:05:,R=AASYN,PORT=AMS5-TS-AMS4,ADDR=0,IMAX=100,OMAX=100")
dbLoadRecords("db/asynRecord.db","P=AMS:SITE:05:,R=EASYN,PORT=AMS5-TS-ECO,ADDR=0,IMAX=100,OMAX=100")


cd ${TOP}/iocBoot/${IOC}
iocInit

