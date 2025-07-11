#!../../bin/rhel7-x86_64/RadiationMonitoring
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
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "G. Brown")
epicsEnvSet("LOCATION" ,"UNDH")
epicsEnvSet("IOC_NAME", "SIOC:UNDH:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

# Load record instances
dbLoadRecords("db/sioc-undh-rm01.db")

epicsEnvSet("DEV",  "RADM:UNDH:RM01" )
epicsEnvSet("NODE", "radm-undh-rm01" )
epicsEnvSet("LOC", "Top of rack under HXU14" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM02" )
epicsEnvSet("NODE", "radm-undh-rm02" )
epicsEnvSet("LOC", "Top of rack under HXU18" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM03" )
epicsEnvSet("NODE", "radm-undh-rm03" )
epicsEnvSet("LOC", "Top of rack under HXU22" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM04" )
epicsEnvSet("NODE", "radm-undh-rm04" )
epicsEnvSet("LOC", "Top of rack under HXU26" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM05" )
epicsEnvSet("NODE", "radm-undh-rm05" )
epicsEnvSet("LOC", "Top of rack under HXU30" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM06" )
epicsEnvSet("NODE", "radm-undh-rm06" )
epicsEnvSet("LOC", "Top of rack under HXU34" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM07" )
epicsEnvSet("NODE", "radm-undh-rm07" )
epicsEnvSet("LOC", "Top of rack under HXU38" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM08" )
epicsEnvSet("NODE", "radm-undh-rm08" )
epicsEnvSet("LOC", "Top of rack under HXU42" )
< iocBoot/common/init_dosfet.cmd
epicsEnvSet("DEV",  "RADM:UNDH:RM09" )
epicsEnvSet("NODE", "radm-undh-rm09" )
epicsEnvSet("LOC", "Top of rack under HXU46" )
< iocBoot/common/init_dosfet.cmd

# Configure autosave
< iocBoot/common/init_restore.cmd.soft

cd ${TOP}/iocBoot/${IOC}
iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< ../common/start_restore.cmd.soft

# End of file



