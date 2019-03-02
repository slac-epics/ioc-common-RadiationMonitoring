#!../../bin/linux-x86/RadFET
#==============================================================
#
#  Abs:  Startup script for Dump RadFet Radiation Monitor 
#
#  Name: st.cmd
#
#  Facility:  LCLS Radiation Monitoring Motion Controls 
#
#  Auth: 07-May-2014, Garth Brown         (GWBROWN) 
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       20-Jul-2017, K. Luchini           (luchini):
#         add standard header and load common scripts
#         from iocBoot/common. Use c-expression shell
#
#==============================================================
#
# Set environment variables
epicsEnvSet("IOC_NAME" ,"SIOC:DMP1:RM001")
epicsEnvSet("LOCATION" ,"lcls-daemon1")
epicsEnvSet("DEV"      ,"RADF:DMP1:RM01" )
epicsEnvSet("NODE"     ,"radf-dmp1-rm01")

# Load common startup
< ../iocBoot/common/st.cmd.soft
< iocBoot/common/init_dosfet.cmd

# End of file

