#==============================================================
#
#  Abs:  Makefile to build EPICS Database components
#
#  Name: Makefile
#
#  Facility: LCLS
#
#  Auth: 21-Jul-2017, M. Dunning (MDUNNING)
#==============================================================

TOP=../../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install) databases, templates, substitutions
DB_INSTALLS += $(ASYN)/db/asynRecord.db

DB += slow_hist_pattern.db
DB += radfet_history_ch.db
DB += dosfet.db
DB += dosfetChannelWrapper.db
DB += 2612Common.template
DB += 2612Channel.template
DB += fht40G.template
DB += fhtChan.template
DB += fhtChanDisplay.template
DB += fhtCommon.template
DB += fht.db
DB += station.template
DB += ecogamma.template
DB += radm_info.template
DB += ams-printer.template
DB += eco-os-6.template
DB += agilent_3458a.db
DB += agilent_34970a.db
DB += hpi_6012.template
DB += gamma6032mon.db
DB += alarmClock.db
DB += dbSubTime.db
DB += decarad.db
DB_INSTALLS += $(ASYN)/db/asynRecord.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
