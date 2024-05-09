#!../../bin/rhel7-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 18-Mar-2018, Jacob DeFilippi    (jpdef)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        19-Oct-2021, K. Luchini         (luchin):
#         add header
#         load st.cmd.soft
#         add env var IOC_NAME
#
#==============================================================

# Setup environment variables
epicsEnvSet("ENGINEER", "Jacob DeFilippis")
epicsEnvSet("IOC_NAME", "SIOC:B34:RM01")
epicsEnvSet("LOCATION", "lcls-dev1")

# Load common piece of startup script
< ../common/st.cmd.soft

# Configure communication port
drvAsynIPPortConfigure("RADM_B34_1", "ts-b34-nw99:2017", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_B34_1", "RADM_B34_1", 1, 1.5)

# Load record instances
dbLoadRecords("db/sioc-b34-rm01.db", "P=RADM:B34:1, PORT=FHT_B34_1")
dbLoadRecords("db/asynRecord.db","P=RADM:B34:1:,R=Asyn, PORT=RADM_B34_1, ADDR=0, IMAX=0, OMAX=0")

iocInit()

# End of file
