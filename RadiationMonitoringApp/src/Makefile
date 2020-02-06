#==============================================================
#
#  Abs:  Makefile to build RadiationMonitoring Application
#
#  Name: Makefile
#
#  Desc: This file generates a database and object file
#        for a soft ioc.
#
#  Facility:  LCLS2
#
#  Auth: 21-Jul-2017, M. Dunning          (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name     (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       dd-mmm-yyyy, Name           (USERNAME):
#          comment
#
#==============================================================

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# Build the ioc application
PROD_IOC = RadiationMonitoring

DBD += RadiationMonitoring.dbd
RadiationMonitoring_DBD += base.dbd
RadiationMonitoring_DBD += system.dbd
RadiationMonitoring_DBD += asyn.dbd
RadiationMonitoring_DBD += stream.dbd
RadiationMonitoring_DBD += drvAsynIPPort.dbd
RadiationMonitoring_DBD += asSupport.dbd
RadiationMonitoring_DBD += drvFHT.dbd
RadiationMonitoring_DBD += calcSupport.dbd
RadiationMonitoring_DBD += history.dbd
RadiationMonitoring_DBD += caPutLog.dbd

# Needed for iocAdmin
RadiationMonitoring_DBD += iocAdmin.dbd

#RadiationMonitoring_DBD += dbRestore.dbd

# Add libraries
RadiationMonitoring_LIBS += devIocStats
RadiationMonitoring_LIBS += autosave
RadiationMonitoring_LIBS += asyn
RadiationMonitoring_LIBS += stream
RadiationMonitoring_LIBS += calc
RadiationMonitoring_LIBS += history
RadiationMonitoring_LIBS += caPutLog

RadiationMonitoring_SRCS += RadiationMonitoring_registerRecordDeviceDriver.cpp
RadiationMonitoring_SRCS += drvFHT.cpp
RadiationMonitoring_SRCS_DEFAULT += RadiationMonitoringMain.cpp

USR_SYS_LIBS += pcre

RadiationMonitoring_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

# End of file



