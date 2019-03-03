#==============================================================
#
#  Abs:  Initalize serial driver for RadFET device
#
#  Name: init_asyn.cmd
#
#  Macros:
#       DEV  - Device name
#       NODE - Serial device name
#       PORT - Serial port nubmer       
#
#  Facility:  Radiation Monitoring Motion Controls 
#
#  Auth: 07-May-2014, Garth Brown         (GWBROWN) 
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#          comment
#
#==============================================================
#
# Set this to enable LOTS of stream module diagnostics
#var streamDebug 1

# Configure each device
drvAsynIPPortConfigure( "$(DEV):PORT", "$(NODE):$(PORT) TCP", 0, 0, 0 )

#/* traceMask definitions*/
#define ASYN_TRACE_ERROR     0x0001
#define ASYN_TRACEIO_DEVICE  0x0002
#define ASYN_TRACEIO_FILTER  0x0004
#define ASYN_TRACEIO_DRIVER  0x0008
#define ASYN_TRACE_FLOW      0x0010
asynSetTraceMask( "$(DEV):PORT", 0, 0x1f) # log everything

#/* traceIO mask definitions*/
#define ASYN_TRACEIO_NODATA 0x0000
#define ASYN_TRACEIO_ASCII  0x0001
#define ASYN_TRACEIO_ESCAPE 0x0002
#define ASYN_TRACEIO_HEX    0x0004
asynSetTraceIOMask( "$(DEV):PORT", 0, 0x1)

# End of file
