#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  Startup Script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  LCLS2 Radiation Monitoring Controls
#
#  Auth: 17-Jul-2018, M. Dunning        (MDUNNING)
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "Garth Brown")
epicsEnvSet("LOCATION", "GUNB")
epicsEnvSet("IOC_NAME", "SIOC:GUNB:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

# RadFet
epicsEnvSet("DEV"      ,"RADF:GUNB:201" )
epicsEnvSet("NODE"     ,"radm-gunb-rm01")
epicsEnvSet("LOC"      ,"GUNB")
< iocBoot/common/init_dosfet.cmd
dbLoadRecords("db/dosfetChannelWrapper.db", "P=RADF:GUNB:201,SENSOR=A,USER_P=RADF:GUNB:622:")
dbLoadRecords("db/dosfetChannelWrapper.db", "P=RADF:GUNB:201,SENSOR=B,USER_P=RADF:GUNB:753:")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



