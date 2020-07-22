#==============================================================
#
#  Abs:  Makefile to build EPICS Database components
#
#  Name: Makefile
#
#  Facility: Dev
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
DB_INSTALLS += $(IOCADMIN)/db/iocAdminSoft.db
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db
IOCRELEASE_DB += iocRelease.db

# Thermo FHT Rad. Mons.
# Dev
DB += sioc-b34-rm01.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
