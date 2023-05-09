#!../../bin/rhel6-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for temporary EcoGamma Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility: CM01 area Radiation Monitoring Controls
#
#  Auth: 18-Mar-2023, Garth Brown        (gwbrown)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Garth Brown")
epicsEnvSet("LOCATION",    "lcls-srv01")
epicsEnvSet("IOC_NAME",    "SIOC:HTR:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

drvAsynIPPortConfigure("EG1","RADM-HTR-MP01:16387")

# Load record instances
dbLoadRecords("db/asynRecord.db","P=RADM:HTR:1:,R=Asyn, PORT=EG1, ADDR=0, IMAX=0, OMAX=0")
dbLoadRecords("db/ecogamma.template", "LOCA=HTR,UNIT=1,PORT=EG1")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file


