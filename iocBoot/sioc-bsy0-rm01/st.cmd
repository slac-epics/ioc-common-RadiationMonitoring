#!../../bin/rhel6-x86_64/RadiationMonitoring
#==============================================================
#
#  Abs:  EPICS startup script for SYSW (LI00-LI10) Rad Mon devices
#
#  Name: st.cmd
#
#  Facility: Development Radiation Monitoring Controls
#
#  Auth: 14-Dec-2021, Garth Brown        (gwbrown)
#  Rev:  dd-mmm-yyyy, Reviewer's Name    (USERNAME)
#--------------------------------------------------------------
#  Mod:
#
#==============================================================

# Set environment variables
epicsEnvSet("ENGINEER",    "Garth Brown")
epicsEnvSet("LOCATION",    "lcls-srv01")
epicsEnvSet("IOC_NAME",    "SIOC:SYSM:RM01")

# Load common piece of startup script
< ../common/st.cmd.soft


#BSOLNE04
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:SPH:04:COM:,TS=ts-li29-nw06,TSPORT=2007,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOLNE05n
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:SPH:05N:COM,TS=ts-li29-nw06,TSPORT=2008,LOC=Gallery: LI29 Spreader line septa,DESC=BSOLNE05n")


# (P28-11) RDMLNE01 ts-li29-nw06:2009
#
epicsEnvSet("FHT_P",      "RADM:LI29:01")
epicsEnvSet("FHT_LOC",    "LI29, in area of SXR/HXR kickers/septa")
epicsEnvSet("FHT_TS",     "ts-li29-nw06")
epicsEnvSet("FHT_TSPORT", "2009")
epicsEnvSet("FHT_PORT",   "FHT")
### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT_PORT),TS=$(FHT_TS),TSPORT=$(FHT_TSPORT)")
# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT_P),PORT=$(FHT_PORT),LOC=$(FHT_LOC)")

#BSOLNE06
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:SPS:06:COM:,TS=ts-li29-nw06,TSPORT=2010,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:BSY0:01 "BSOBSY01 (B136)"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:BSY0:01:COM:,TS=ts-b136-nw06,TSPORT=2009,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:BSY0:02N "BSOBSY02N"
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:BSY0:02N:COM,TS=ts-b136-nw06,TSPORT=2010,LOC=B136 north of penetration 3,DESC=BSOBSY02n")

# RDMs
# BSY (RDMBSY01) BSY near STP-D2
# BSY (RDMBSY02)  BSY near BSYDUMP
# HX2 (RDMHX201)
# EBD/FEE (RDMEBD01)  EBD near BYD/BYDB
# EBD/FEE (RDMEBD02)  EBD near EBD dumps

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

