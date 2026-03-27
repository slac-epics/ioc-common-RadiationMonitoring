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
#==============================================================
 
# Environment variables
epicsEnvSet("ENGINEER" ,"")
epicsEnvSet("IOC_NAME" , sioc-b34-rm05")
epicsEnvSet("LOCATION" ,"B134")

# Load common piece of startup script
< ../common/st.cmd.soft

drvAsynIPPortConfigure("PM1","192.168.1.177:16388")

dbLoadRecords("db/ecogamma.template", "UNIT=273,PORT=PM1,LOCA=$(LOCATION)")
dbLoadRecords("db/asynRecord.db", "P=RADM:B34:273:,R=Asyn,PORT=PM1,ADDR=0,IMAX=100,OMAX=100")

iocInit

# End of file
