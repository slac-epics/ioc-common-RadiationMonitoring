#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <epicsExport.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <recGbl.h>
#include <alarm.h>


static int time_diff(aSubRecord *prec)
{
  DBLINK *a_name = prec->a;
  DBLINK *t_timeout = prec->b;

  dbAddr *paddr;

  epicsTimeStamp now;
  epicsTimeGetCurrent(&now);

  struct dbAddr dbEntry;

  struct buf {
    DBRstatus
    DBRtime
    epicsFloat64 value;
  } buf;

  long options = DBR_STATUS | DBR_TIME;
  long noel = 1;

  dbNameToAddr(a_name, &dbEntry);
  int ret = dbGetField(&dbEntry, DBR_DOUBLE, &buf, &options, &noel, NULL);

  epicsFloat64 dt = epicsTimeDiffInSeconds(&now, &buf.time); 

  if (dt > *(double *)t_timeout && buf.status == firstEpicsAlarmCond) {
    recGblSetSevr(prec, TIMEOUT_ALARM, INVALID_ALARM); 
    recGblSetSevr(dbEntry.precord, TIMEOUT_ALARM, INVALID_ALARM);
    dbProcess(dbEntry.precord); 
  }

  return 0;
}

epicsRegisterFunction(time_diff);
