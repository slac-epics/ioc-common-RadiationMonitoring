#==============================================================
#
#  Abs:  Makefile to build EPICS Database components
#
#  Name: Makefile
#
#  Facility: LCLS2
#
#  Auth: 21-Jul-2017, M. Dunning (MDUNNING)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#       dd-mmm-yyyy, First Lastname    (USERNAME):
#         comment
#
#==============================================================

TOP=../../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
#
DB += history_ch.db
DB += dosfet.db
DB += dostfetChannelWrapper.db
DB += fht40G.template
DB += fhtChan.template
DB += fhtChanDisplay.template
DB += fhtCommon.template
DB += station.template
DB += ecogamma.template
DB += radm_info.template
DB += ams-printer.template
DB += eco-os-6.template
DB_INSTALLS += $(ASYN)/db/asynRecord.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
