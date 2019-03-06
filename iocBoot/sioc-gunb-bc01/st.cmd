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
epicsEnvSet("IOC_NAME", "SIOC:GUNB:RM02")
epicsEnvSet("LOCATION" ,"lcls-daemon1")
epicsEnvSet("DEV"      ,"RADF:DMP1:BC01" )
epicsEnvSet("NODE"     ,"radf-gunb-bc01")

# Load common piece of startup script
< ../common/st.cmd.soft
< iocBoot/common/init_dosfet.cmd

epicsEnvSet("STARTUP",  "${EPICS_SITE_TOP}/iocCommon/${IOC}")

# Asyn diagnostics
#asynSetTraceMask("RADM_GUNB_1", -1, 0x9)
#asynSetTraceIOMask("RADM_GUNB_1", -1, 0x5)

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



