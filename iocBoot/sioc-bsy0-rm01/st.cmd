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
epicsEnvSet("IOC_NAME",    "SIOC:BSY0:RM01")

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
epicsEnvSet("FHT_PORT",   "RDMLNE01_FHT")
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

#BSOC:BSY0:06:COM "BSOBTW06" "B913"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:BSY0:06:COM:,TS=ts-b911-nw07,TSPORT=2001,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:09:COM "BSOBTH09" "RSY on South side of fence near B913"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:09:COM:,TS=ts-b911-nw07,TSPORT=2002,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:2:COM "BSOBTH02" "North Headhouse"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:2:COM:,TS=ts-b911-nw07,TSPORT=2003,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:10N "BSOBTH10n"
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:LTU0:10N:COM,TS=ts-b911-nw07,TSPORT=2004,LOC=North BTH Fence,DESC=BSOBTH10n")

#BSOC:LTU0:3:COM "BSOBTH03" "South Headhouse"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:3:COM:,TS=ts-b911-nw07,TSPORT=2005,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:04:COM "BSOBTH04" "Middle B911"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:04:COM:,TS=ts-b911-nw07,TSPORT=2006,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:6:COM "BSOBTH06" "Middle B913"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:6:COM:,TS=ts-b913-nw02,TSPORT=2011,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:07:COM "BSOBTH07" "North BTHE Maze"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:07:COM:,TS=ts-b913-nw02,TSPORT=2012,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:LTU0:11:COM "BSOBTH11" "B211 on fence"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LTU0:11:COM:,TS=ts-b913-nw02,TSPORT=2013,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")

#BSOC:UND0:02N "BSOUND02N"
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:UND0:02N:COM,TS=ts-b921-nw04,TSPORT=2004,LOC=Inside B921,DESC=BSOUND02n")

#BSOC:UND0:01:COM "BSOUND01" "Inside B921"
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:UND0:01:COM:,TS=ts-b921-nw04,TSPORT=2005,SYS=BSOIC:BSY0:ALL,IOC=${IOC}")


# HX2 (RDMHX201) Inside HX-2 housing
epicsEnvSet("FHT_P",      "RADM:B969:01")
epicsEnvSet("FHT_LOC",    "Inside HX-2 housing")
epicsEnvSet("FHT_TS",     "ts-b969-nw01")
epicsEnvSet("FHT_TSPORT", "2001")
epicsEnvSet("FHT_PORT",   "RDMHX201_FHT")
### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT_PORT),TS=$(FHT_TS),TSPORT=$(FHT_TSPORT)")
# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT_P),PORT=$(FHT_PORT),LOC=$(FHT_LOC)")

# RDMs
# BSY (RDMBSY02)  BSY near BSYDUMP
# EBD/FEE (RDMEBD01)  EBD near BYD/BYDB
# EBD/FEE (RDMEBD02)  EBD near EBD dumps
epicsEnvSet("FHT_P",      "RADM:DMP0:01")
epicsEnvSet("FHT_LOC",    "EBD near BYD/BYDB and dumps")
epicsEnvSet("FHT_TS",     "ts-b940-pp02")
epicsEnvSet("FHT_TSPORT", "2001")
epicsEnvSet("FHT_PORT",   "RDMEBD01-2_FHT")
### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT_PORT),TS=$(FHT_TS),TSPORT=$(FHT_TSPORT)")
# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT_P),PORT=$(FHT_PORT),LOC=$(FHT_LOC)")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

