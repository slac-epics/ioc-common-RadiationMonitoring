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
#=============================================================

# Setup environment variables
epicsEnvSet("ENGINEER","Jacob DeFilippis")
epicsEnvSet("IOC_NAME","SIOC:B24:RM01")
epicsEnvSet("LOCATION","SLAC Perimeter")

# Load common piece of startup script
< ../common/st.cmd.soft

## Setup asyn connections PM01-PM08
drvAsynIPPortConfigure("B24-GAMMA","wb-site-rm08:5000")
drvAsynIPPortConfigure("B24-TS","wb-site-rm08:5001")
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("B24_FHT", "B24-TS", 1, 1.5)

## Load record instances
#General

#Station B24 Test Stand
dbLoadRecords("db/peripheral-station.db","UNIT=B24,GPORT=B24-GAMMA,NPORT=B24-FHT,M=1")

cd ${TOP}/iocBoot/${IOC}
iocInit

# End of file
