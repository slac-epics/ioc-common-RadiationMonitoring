#==============================================================
#
#  Abs:  RadFET Radiation Monitor script
#
#  Name: init_dosfet.cmd
#
#  Rem: This script is loaded by a soft ioc to perform
#       the following:
#        1) initalize the serial driver used to communicate
#           with the radiation monitor over and RS232 line.
#        2) load the application databases
#        3) setup autosave
#        4) start EPICS
#        5) turn on caPutLogging
#        6) start the autosave process
#        7) tone down the asyn logging messages
#
#       Upon entry, this script expects to be at the TOP
#       of the IOC application. In addition, the following
#       macros are expected to be defined:
#
#          DEV  - device name
#          PORT - asyn port name
#          EPICS_CA_PUT_LOG_ADDR - channel access caput log address
#
#  Facility:  LCLS Radiation Monitoring Motion Controls
#
#  Auth: 07-May-2014, Garth Brown         (GWBROWN)
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       20-Jul-2017, K. Luchini           (LUCHINI):
#        extract from soft iocs
#
#==============================================================
#
# Initialize serial driver
epicsEnvSet("PORT","10001")
< iocBoot/common/init_asyn.cmd

#Load the application databases
dbLoadRecords("db/dosfet.db"    ,"P=$(DEV) ,PORT=$(DEV):PORT, LOC=$(LOC)")
dbLoadRecords("db/asynRecord.db","P=$(DEV):,R=ASYN,PORT=$(DEV):PORT,ADDR=0,OMAX=0,IMAX=108")

# Wait before turning the logging down when troubleshooting
#epicsThreadSleep(5)
# Tone down the asyn logging for normal operation
asynSetTraceMask("$(DEV):PORT", 0, 0x1)

# End of file

