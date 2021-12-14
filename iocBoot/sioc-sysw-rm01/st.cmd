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
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:GUNB:03:,TS=ts-li00-nw05,TSPORT=2003,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:GUNB:04N,TS=ts-li00-nw05,TSPORT=2004,LOC=Outside InjW Chicane")
# (P02-05) RDMLNW01 ts-li02-nw02:2001
#
epicsEnvSet("FHT1_P",      "RADM:LI02:1")
epicsEnvSet("FHT1_LOC",    "P2-8")
epicsEnvSet("FHT1_TS",     "ts-li02-nw02")
epicsEnvSet("FHT1_TSPORT", "2001")
epicsEnvSet("FHT1_PORT",   "FHT1")
### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT1_PORT),TS=$(FHT1_TS),TSPORT=$(FHT1_TSPORT)")

# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT1_P),PORT=$(FHT1_PORT),LOC=$(FHT1_LOC)")

iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI02:05:,TS=ts-li02-nw02,TSPORT=2002,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI04:07:,TS=ts-li04-nw02,TSPORT=2001,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:LI04:14N,TS=ts-li04-nw02,TSPORT=2002,LOC=S4 Helium Penetration")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI04:06:,TS=ts-li04-nw02,TSPORT=2003,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI05:08:,TS=ts-li04-nw02,TSPORT=2004,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:LI05:15N,TS=ts-li04-nw02,TSPORT=2005,LOC=S5 Helium Penetration")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI07:09:,TS=ts-li07-nw02,TSPORT=2001,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")

# RDMLNW02  (P07-11) ts-li07-nw02:2002
#
epicsEnvSet("FHT2_P",      "RADM:LI07:2")
epicsEnvSet("FHT2_LOC",    "P7-12")
epicsEnvSet("FHT2_TS",     "ts-li07-nw02")
epicsEnvSet("FHT2_TSPORT", "2002")
epicsEnvSet("FHT2_PORT",   "FHT2")
### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT2_PORT),TS=$(FHT2_TS),TSPORT=$(FHT2_TSPORT)")

# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT2_P),PORT=$(FHT2_PORT),LOC=$(FHT2_LOC)")

iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI07:10:,TS=ts-li07-nw02,TSPORT=2003,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_hpi_6012.iocsh", "P=BSOC:LI07:11N,TS=ts-li07-nw02,TSPORT=2004,LOC=Penetration 7-12")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:LI10:12:,TS=ts-li10-nw02,TSPORT=2004,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")
iocshLoad("$(TOP)/iocBoot/common/init_gamma_6032.iocsh", "P=BSOC:IN10:03:,TS=ts-li10-nw02,TSPORT=2005,SYS=BSOIC:SYSM:ALL,IOC=${IOC}")

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

