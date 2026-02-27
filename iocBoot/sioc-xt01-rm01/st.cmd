#!../../bin/rhel9-x86_64/RadiationMonitoring

#==============================================================
#
#  Abs:  EPICS startup script
#
#  Name: st.cmd
#
#  Facility: Testfac Radiation Monitoring Controls
#
#  Auth: 27-Feb-2026, Mike Dunning       (mdunning)
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER", "M. Dunning")
epicsEnvSet("LOCATION", "testfac-srv09")
epicsEnvSet("IOC_NAME", "SIOC:XT01:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft

### DOSFET dosimeters -------------------------------------------------------
epicsEnvSet("DEV",  "RADF:XT01:1" )
epicsEnvSet("NODE", "radm-xt01-rm01")
epicsEnvSet("LOC",  "XT01")
< iocBoot/common/init_dosfet.cmd
###--------------------------------------------------------------------------

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Initialize caPutLog
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

