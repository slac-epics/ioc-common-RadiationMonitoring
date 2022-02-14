#!../../bin/rhel6-x86_64/RadiationMonitoring

# Set environment variables
epicsEnvSet("ENGINEER", "J. Nelson")
epicsEnvSet("IOC_NAME", "SIOC:SYS0:RM01")

# Load common piece of startup script, this include STREAM_PROTOCOL_PATH
< ../common/st.cmd.soft

# Configure communication port
drvAsynIPPortConfigure("decarad1","ts-sys0-nw01:2101",0,0,0)
drvAsynIPPortConfigure("decarad2","ts-sys0-nw02:2101",0,0,0)

# Load record instances
dbLoadRecords("db/decarad.db","U=100,PORT=decarad1")
dbLoadRecords("db/decarad.db","U=200,PORT=decarad2")
dbLoadRecords("db/asynRecord.db","P=RADM:SYS0:100:,R=Asyn,PORT=decarad1,ADDR=0,IMAX=0,OMAX=0")
dbLoadRecords("db/asynRecord.db","P=RADM:SYS0:200:,R=Asyn,PORT=decarad2,ADDR=0,IMAX=0,OMAX=0")
###--------------------------------------------------------------------------

# Configure autosave
< $(TOP)/iocBoot/common/init_restore.cmd.soft

iocInit()

# Start autosave
< $(TOP)/iocBoot/common/start_restore.cmd.soft

# End of file

