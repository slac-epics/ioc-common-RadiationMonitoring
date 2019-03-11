#!../../bin/rhel6-x86_64/RadiationMonitoring

#==============================================================
#
#  Abs:  Startup Script for Thermo FHT Rad Mon device(s)
#
#  Name: st.cmd
#
#  Facility:  LCLS2
#
#  Auth: 17-Jul-2018, M. Dunning      (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        dd-mmm-yyyy, Name       (USERNAME):
#           comment
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("IOC_NAME", "SIOC:GUNB:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

# Configure communication port
drvAsynIPPortConfigure("RADM_GUNB_1", "ts-li00-nw05:2002", 0,0,0)

# Initialize asyn driver
# drvFHTConfigure(const char* port, const char* IOport, int addr, double timeout) {
#/*------------------------------------------------------------------------------
# * EPICS iocsh callable function to call constructor for the drvFHT class.
# *  port    The name of the asyn port driver to be created.
# *  IOport  The communication port name.
# *  addr    The hardware address.
# *  timeout The timeout for I/O (optional, default = 1.0).
# *----------------------------------------------------------------------------*/
drvFHTConfigure("FHT_GUNB_1", "RADM_GUNB_1", 1, 1.5)

# Asyn diagnostics
#asynSetTraceMask("RADM_GUNB_1", -1, 0x9)
#asynSetTraceIOMask("RADM_GUNB_1", -1, 0x5)

# Load record instances
dbLoadRecords("db/sioc-gunb-rm01.db", "P=RADM:GUNB:1, PORT=FHT_GUNB_1")

epicsEnvSet("LOCATION" ,"GUNB")
epicsEnvSet("DEV"      ,"RADF:GUNB:201" )
epicsEnvSet("NODE"     ,"radf-gunb-rm01")
< iocBoot/common/init_dosfet.cmd
dbLoadRecords("db/dosfetChannelAlias.db", "P=RADF:GUNB:201,SENSOR=A,USER_P=RADF:GUNB:201:")
dbLoadRecords("db/dosfetChannelAlias.db", "P=RADF:GUNB:201,SENSOR=B,USER_P=RADF:GUNB:621:")

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



