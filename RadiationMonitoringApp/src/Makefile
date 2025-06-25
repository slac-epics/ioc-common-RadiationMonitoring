#==============================================================
#
#  Abs:  Makefile to build $(PROD_NAME) Application
#
#  Name: Makefile
#
#  Desc: This file generates a database and object file
#        for a soft ioc.
#
#  Facility:  LCLS
#
#  Auth: 21-Jul-2017, M. Dunning          (MDUNNING)
#==============================================================

TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# =====================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
# =====================================================
# Support Perl C regular expression library
USR_INCLUDES += -I$(PCRE_INCLUDE)

# ======================================================
# PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
# ======================================================
pcre_DIR = $(PCRE_LIB)
tirpc_DIR = $(TIRPC_LIB)
# ======================================================

# ======================================================
# LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
# =======================================================
USR_LIBS_Linux += pcre
ifeq ($(NEEDS_TIRPC),YES)
USR_LIBS_Linux += tirpc
endif
# =======================================================

# Build the ioc application
PROD_NAME = RadiationMonitoring
PROD_IOC = $(PROD_NAME)

DBD += $(PROD_NAME).dbd
$(PROD_NAME)_DBD += base.dbd
$(PROD_NAME)_DBD += system.dbd
$(PROD_NAME)_DBD += asyn.dbd
$(PROD_NAME)_DBD += stream.dbd
$(PROD_NAME)_DBD += drvAsynIPPort.dbd
$(PROD_NAME)_DBD += drvVxi11.dbd
$(PROD_NAME)_DBD += asSupport.dbd
$(PROD_NAME)_DBD += drvFHT.dbd
$(PROD_NAME)_DBD += calcSupport.dbd
$(PROD_NAME)_DBD += history.dbd
$(PROD_NAME)_DBD += caPutLog.dbd
$(PROD_NAME)_DBD += iocAdmin.dbd
$(PROD_NAME)_DBD += dbSubTime.dbd
$(PROD_NAME)_DBD += time_diff.dbd

# Add libraries
$(PROD_NAME)_LIBS += devIocStats
$(PROD_NAME)_LIBS += autosave
$(PROD_NAME)_LIBS += asyn
$(PROD_NAME)_LIBS += stream
$(PROD_NAME)_LIBS += calc
$(PROD_NAME)_LIBS += history
$(PROD_NAME)_LIBS += caPutLog

$(PROD_NAME)_SRCS += $(PROD_NAME)_registerRecordDeviceDriver.cpp
$(PROD_NAME)_SRCS += drvFHT.cpp
$(PROD_NAME)_SRCS += dbSubTime.c
$(PROD_NAME)_SRCS += time_diff.c
$(PROD_NAME)_SRCS_DEFAULT += $(PROD_NAME)Main.cpp

$(PROD_NAME)_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

# End of file



