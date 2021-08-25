#!../../bin/rhel6-x86_64/RadiationMonitoring

# Set environment variables
epicsEnvSet("ENGINEER",    "M. Dunning")
epicsEnvSet("IOC_NAME",    "SIOC:B34:RM03")
epicsEnvSet("STARTUP",     "${EPICS_SITE_TOP}/iocCommon/${IOC}")
#
epicsEnvSet("FHT1_P",      "RADM:B34:301")
epicsEnvSet("FHT1_LOC",    "B34")
epicsEnvSet("FHT1_TS",     "ts-lclsdev01")
epicsEnvSet("FHT1_TSPORT", "4001")
epicsEnvSet("FHT1_PORT",   "FHT1")
#
epicsEnvSet("NBSOIC_P",      "BSOC:B34:302")
epicsEnvSet("NBSOIC_TS",     "ts-lclsdev01")
epicsEnvSet("NBSOIC_TSPORT", "4015")
epicsEnvSet("NBSOIC_PORT",   "NB1")
#
epicsEnvSet("BSOIC2_P",      "BSOIC:NEH1:121:")
epicsEnvSet("BSOIC2_TS",     "ts-lclsdev01")
epicsEnvSet("BSOIC2_TSPORT", "2016")
epicsEnvSet("BSOIC2_PORT",   "B2")

# Load common piece of startup script
< ../common/st.cmd.soft

### Thermo FHT Radmon -------------------------------------------------------
iocshLoad("$(TOP)/iocBoot/common/init_fht.iocsh", "PORT=$(FHT1_PORT),TS=$(FHT1_TS),TSPORT=$(FHT1_TSPORT)")

# Load record instances
dbLoadRecords("db/fht.db", "P=$(FHT1_P),PORT=$(FHT1_PORT),LOC=$(FHT1_LOC)")
dbLoadRecords("db/asynRecord.db","P=$(FHT1_P):,R=Asyn,PORT=$(FHT1_PORT),ADDR=0,IMAX=0,OMAX=0")
###--------------------------------------------------------------------------

###--- Neutron BSOIC ------------------------------------------------------
drvAsynIPPortConfigure("$(NBSOIC_PORT)", "$(NBSOIC_TS):$(NBSOIC_TSPORT)", 0,0,0)
asynSetOption("$(NBSOIC_PORT)", 0, "disconnectOnReadTimeout", "Y")
dbLoadRecords("db/hpi_6012.template", "P=$(NBSOIC_P),PORT=$(NBSOIC_PORT),PROTOFILE=hpi_6012.proto,LOC=$(FHT1_LOC)")
dbLoadRecords("db/asynRecord.db", "P=$(NBSOIC_P):,R=Asyn,PORT=$(NBSOIC_PORT),ADDR=0,IMAX=0,OMAX=0")
###-------------------------------------------------------------------------

###--- 6032 BSOIC ---------------------------------------------------------
drvAsynIPPortConfigure ("$(BSOIC2_PORT)","$(BSOIC2_TS):$(BSOIC2_TSPORT)",0,0,0)
asynSetOption("$(BSOIC2_PORT)", 0, "disconnectOnReadTimeout", "Y")
dbLoadRecords("db/gamma6032mon.db", "BL=$(BSOIC2_P),PORT=$(BSOIC2_PORT),IOC=$(IOC)")
dbLoadRecords("db/alarmClock.db", "sys=BSOIC:B34:ALL")
###-------------------------------------------------------------------------


# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

