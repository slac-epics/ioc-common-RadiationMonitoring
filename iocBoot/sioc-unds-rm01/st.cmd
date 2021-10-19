#!../../bin/rhel6-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  Startup Script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  LCLS2 Radiation Monitoring Controls
#
#  Auth: 17-Jul-2018, M. Dunning      (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini         (luchini):
#         add caPutLogInit after iocInit
#         add env var LOCATION
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "G. Brown")
epicsEnvSet("LOCATION", "lcls-daemon1")
epicsEnvSet("IOC_NAME", "SIOC:UNDS:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

# Set this to enable LOTS of stream module diagnostics
#var streamDebug 1

# Configure each device
epicsEnvSet( "DEV1", "RADM:UNDS:RM01:PORT" )
drvAsynIPPortConfigure( "$(DEV1)", "radm-unds-rm01:5025 TCP", 0, 0, 0 )


#/* traceMask definitions*/
#define ASYN_TRACE_ERROR     0x0001
#define ASYN_TRACEIO_DEVICE  0x0002
#define ASYN_TRACEIO_FILTER  0x0004
#define ASYN_TRACEIO_DRIVER  0x0008
#define ASYN_TRACE_FLOW      0x0010
asynSetTraceMask( "$(DEV1)", 0, 0x1f) # log everything

#/* traceIO mask definitions*/
#define ASYN_TRACEIO_NODATA 0x0000
#define ASYN_TRACEIO_ASCII  0x0001
#define ASYN_TRACEIO_ESCAPE 0x0002
#define ASYN_TRACEIO_HEX    0x0004
asynSetTraceIOMask( "$(DEV1)", 0, 0x2)

########################################################################
# BEGIN: Load the record databases
#######################################################################
dbLoadRecords("db/sioc-unds-rm01.db","PORT=$(DEV1)" )
dbLoadRecords("db/asynRecord.db","P=RADM:UNDS:RM01,R=:ASYN,PORT=$(DEV1),ADDR=0,OMAX=0,IMAX=108")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

asynSetTraceMask( "$(DEV1)", 0, 0x1) # log errors

# End of file



