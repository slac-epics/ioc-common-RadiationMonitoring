#!../../bin/rhel7-x86_64/RadiationMonitoring
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
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "B34-273")
epicsEnvSet("IOC_NAME", "SIOC:B34:FC273")
#
epicsEnvSet("PORT",        "E5810")
epicsEnvSet("E5810_IP",    "134.79.217.164")
epicsEnvSet("3458A_ADDR",  "24")
epicsEnvSet("34970A_ADDR", "9")
epicsEnvSet("P",           "RADM:B34:273")

# Load common piece of startup script
< ../common/st.cmd.soft

# Configure communication port
# Devices are daisy-chained
# vxi11Configure("portName", "inet_addr", flags, "timeout", "vxiName", priority, noAutoConnect)
vxi11Configure("$(PORT)", "$(E5810_IP)", 0, "5.0", "gpib0", 0, 0)

# Asyn options/diagnostics
# The 3458A needs a very long timeout when it fetches data 
asynSetOption("$(PORT)", "$(3458A_ADDR)", "rpctimeout", 60)
#
asynSetTraceIOMask("$(PORT)", "$(3458A_ADDR)", 0x2)
#asynSetTraceMask("$(PORT)", "$(3458A_ADDR)", 0x9)
#
asynSetTraceIOMask("$(PORT)", "$(34970A_ADDR)", 0x2)
#asynSetTraceMask("$(PORT)", "$(34970A_ADDR)", 0x9)

# Load record instances
dbLoadRecords("db/agilent_3458a.db", "P=$(P):DVM,PORT=$(PORT),ADDR=$(3458A_ADDR),PROTOFILE=agilent_3458a.proto")
dbLoadRecords("db/asynRecord.db", "P=$(P):DVM:,R=Asyn,PORT=$(PORT),ADDR=$(3458A_ADDR),OMAX=0,IMAX=0")
#
dbLoadRecords("db/agilent_34970a.db", "P=$(P):DAQ,PORT=$(PORT),ADDR=$(34970A_ADDR),PROTOFILE=agilent_34970a.proto")
dbLoadRecords("db/asynRecord.db", "P=$(P):DAQ:,R=Asyn,PORT=$(PORT),ADDR=$(34970A_ADDR),OMAX=0,IMAX=0")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Streamdevice options
var streamError 1
#var streamDebug 1

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

