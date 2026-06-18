#!../../bin/rhel9-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  Startup Script for RadiationMonitoring IOC
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 25-Jun-2021, M. Dunning         (mdunning)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        dd-mm-yyyy, First Lastname      (USERNAME):
#         comment
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "K. Leleux")
epicsEnvSet("LOCATION", "B34-253")
epicsEnvSet("IOC_NAME", "SIOC:B34:RM253")
#
epicsEnvSet("PORT",         "LB115")
epicsEnvSet("LB115_IP",     "134.79.218.86")
epicsEnvSet("LB115_PORT",   "8123")
epicsEnvSet("LB115_ADDR",   "24")
epicsEnvSet("P",           "RADM:B34:253")

# Load common piece of startup script
< ../common/st.cmd.soft

drvAsynIPPortConfigure("LB115_IP", "$(LB115_IP):$(LB115_PORT)")
#asynSetTraceIOMask("LB115_IP", "$(LB115_ADDR)", 0x2)
asynSetTraceMask("LB115_IP", "$(LB115_ADDR)", ASYN_TRACE_ERROR)
asynSetOption("LB115_IP", 0, "disconnectOnReadTimeout", "Y")
lb115Configure("$(PORT)", "LB115_IP", 2)

# Load record instances
dbLoadRecords("db/lb115.db", "P=$(P), PORT=$(PORT)")
dbLoadRecords("db/asynRecord.db", "P=$(P),R=Asyn,PORT=LB115_IP,ADDR=$(LB115_ADDR),OMAX=0,IMAX=0")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

