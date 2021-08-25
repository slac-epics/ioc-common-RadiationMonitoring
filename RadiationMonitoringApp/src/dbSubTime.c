/*=============================================================================

  Filename: dbSubTime.c

  Abs:      This file contains all subroutine support for focus
            supplies and modulators.

  Rem:      All functions called by the subroutine record get passed one argument:

           Functions:               Description
           ------------            ----------------------------
           mySubInit               General subRecord Initialization
           myAsubInit              General genSubRecord Initialization
           myAsubProcess           24-hour timer goes off at midnight. 
           mySubProcess            Print record name to stdio if mySubDebug is set

           Generic Functions        Description
           -----------------       -----------------------------
           alarmPeriodInit         Alarm period initialization
           alarmClockInit          Alarm clock initialization
           alarmPeriod             Generate a soft event at a programmable period
           alarmClock              Generate a soft event at a programmable time
           dateTime                Out date and 24-hour time to a string
           doseStatus              Get does rate status
           isDataMissing           check if data is missing
           HVStatus                HV status message based on hpi status byte2

           All functions return a long integer.  0 = OK, -1 = ERROR.
           The subroutine record ignores the status returned by the Init
           routines.  For the calculation routines, the record status (STAT) is
           set to SOFT_ALARM (unless it is already set to LINK_ALARM due to
           severity maximization) and the severity (SEVR) is set to psub->brsv
           (BRSV - set by the user in the database though it is expected to
            be invalid)

  Side:  Place all functions called by the subroutine records in the
         file dbSubTime.dbd

  Auth:  dd-mmm-yyyy, First Lastname     (USERNAME):
  Rev:   dd-mmm-yyyy, Reviewer's Name    (USERNAME)
--------------------------------------------------------------
  Mod:
         27-Jan-2017, K. Luchini         (LUCHINI):
           add functions alarmClockInit and alarmClock
           add functions alarmPeriodInit and alarmPeriod
           add standard header
           add include files

==============================================================
*/

#include <dbDefs.h>
#include <registryFunction.h>
#include <stdio.h>
#include <stdlib.h>             /* for atol()   */
#include <string.h>             /* for strncpy  */
#include <time.h>

#include "dbScan.h"             /* for post_event()           */
#include "epicsTime.h"          /* for epicsTimeStamp         */
#include "subRecord.h"          /* for struct subRecord       */
#include "aSubRecord.h"         /* for struct aSubRecord      */
#include "epicsTypes.h"         /* for epicsUInt32            */
#include "alarm.h"              /* for INVALID_ALARM          */
#include "recGbl.h"             /* for recGblGetTimeStamp()   */
#include "registryFunction.h"   /* for epicsExport            */
#include "epicsExport.h"        /* for epicsRegisterFunction  */


#ifndef SUCCESS
#define SUCCESS  0
#endif
#ifndef ERROR
#define ERROR    -1
#endif

#define TIME_FORMAT       "%m/%d/%y %H:%M:%S"
#define TIME_ONLY_FORMAT  "%H:%M:%S"

#define HOURS_PER_DAY        24
#define MINUTES_PER_HOUR     60
#define SECONDS_PER_MINUTE   60

/* Global debug variable */
int mySubDebug;

static long mySubInit(subRecord *precord)
{
    if (mySubDebug)
      printf("Record %s called mySubInit(%p)\n", precord->name, (void*) precord); 
    return 0;
}

static long mySubProcess(subRecord *precord)
{
    time_t current_time;
    struct tm *pbroken_down_time;
    static struct tm broken_down_time_buf;

    /* printf("Record %s called mySubProcess(%p)\n",precord->name, (void*) precord); */

    time(&current_time);
    pbroken_down_time = localtime(&current_time);

    /*
    printf ("Current local time and date: %s\n", asctime (pbroken_down_time));
    printf ("Broken down time: day %d, month %d, year %d, sec %d, min %d, hour %d\n", 
             pbroken_down_time->tm_mday, pbroken_down_time->tm_mon, pbroken_down_time->tm_year, 
             pbroken_down_time->tm_sec, pbroken_down_time->tm_min, pbroken_down_time->tm_hour);
    */

    // Check to see if day has changed
    if (pbroken_down_time->tm_mday != broken_down_time_buf.tm_mday)
    //if (pbroken_down_time->tm_hour != broken_down_time_buf.tm_hour)
    //if (pbroken_down_time->tm_min != broken_down_time_buf.tm_min)
    {
        printf("***********************Day changed*****************************\n");
        precord->val = 1;
    }
    else
    {
        precord->val = 0;
    }

    // Update the buffered entry
    memcpy(&broken_down_time_buf, pbroken_down_time, sizeof(struct tm));

    return 0;
}

static long myAsubInit(aSubRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called myAsubInit(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

static long myAsubProcess(aSubRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called myAsubProcess(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

/*=============================================================================

  Name: alarmPeriodInit

  Abs:  Initalization for alarmPeriod function

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        subRecord *         psub        read/write pointer to subroutine record

        Input:
        --------
        A = period in minutes, defaults is 60 minutes.  <dev>:PERIOD

        Outputs:
        ---------
        L = timestamp (secPastEpoc) when we last chimed

  Rem:  Check the default chime period, if less then 1 minute, then set to the default
        of 60 minutes, before setting the prompt string to the target name.

  Side: None

  Ret:  int
           0 - Successful (always)

==============================================================================*/
static long alarmPeriodInit(subRecord *psub)
{
  long           status=SUCCESS;
  epicsTimeStamp now;

  if (psub->a < 1.0) psub->a = MINUTES_PER_HOUR;   /* Default to chiming hourly */

  epicsTimeGetCurrent(&now);
  psub->l = now.secPastEpoch;
  return(status);
}


/*=============================================================================

  Name: alarmPeriod

  Abs:  Generate a soft event at a programmable period in minutes.

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        subRecord *         psub        read/write pointer to subroutine record

        Input:
        --------
        A = chime period in minutes, defaults to 60 if not set   <dev>:PERIOD
        B = event number

        Outputs:
        ---------
        L = timestamp (secPastEpoc) when we last event
        VAL = Not Used

  Rem: Generate a soft event at the specified period n A. This will trigger
       any records with SCAN=Event and EVNT=<n> on the IOC. With this subroutine
       record you can instantiante as many records as you need periods, by setting
       A to the period in minutes and B to the event number.

  Side: None

  Ret:  long
          0 - Successful (always)

   Auth: Andrew Johnson (Argonne Nantional Lab)

==============================================================================*/
static long alarmPeriod(subRecord *psub)
{
    long           status = SUCCESS;
    epicsUInt32    period, next;
    epicsTimeStamp now;

    if (psub->a < 1.0) psub->a = 1.0;   /* Chime at most once a minute */
    period = 60 * (epicsUInt32) psub->a;
    next = period * (1 + (epicsUInt32) psub->l / period);

    epicsTimeGetCurrent(&now);
    if (now.secPastEpoch >= next) {    /* Bong bong bong bong */
        psub->l = now.secPastEpoch;
        if ( (epicsInt32)psub->b )
          post_event((int)psub->b);
    }
    return(status);
}

/*=============================================================================

  Name: alarmClockInit

  Abs:  Initialization for alarmClock function

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *         psub        read/write pointer to subroutine record


        Input:
        --------
        A = hour to wake up,    range: 0-23  <dev>:FLT_RESET_HR
        B = minute to wake up,  range: 0-59  <dev>:FLT_RESET_MIN
        C = seconds to wake up, range: 0-59  <dev>:FLT_RESET_SEC
        D = event number                     <dev>:FLT_RESET_EVNT
        E = debug flag

        Outputs:
        ---------
        VALA = timestamp when alarm clock last sounded      (FROM)
        VALB = timestamp when alarm clock will sound next   (TO)
        Note: timestamp string format
                02/02/2010 10:19:25
                0123456789012345678
        VALC = alarm clock chime (0=sleep, 1=wake_up)
        VALD = timestamp when alarm clock last sounded in seconds
        VALE = timestamp when alarm clock will sound next in seconds
        Note: timestamp in seconds is since 0000 Jan 1, 1990

        VAL = Not Used

  Rem:

  Side: None

  Ret:  int
           0 - Successful (always)

==============================================================================*/
static long alarmClockInit(aSubRecord *psub)
{
   long           status=SUCCESS;

  /*
   * General purpose initialization required since all subroutine records
   * require a non-NULL init routine even if no initialization is required.
    * Note that most subroutines in this file use this routine as an init
   * routine.  If init logic is needed for a specific subroutine, create a
   * new routine for it - don't modify this one.
   */
   return(status);
}


/*=============================================================================

  Name: alarmClock

  Abs:  Generate a soft event at a programmable time of day, specified
        in hours and minutes.

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *         psub        read/write pointer to subroutine record

        Input:
        --------
        A = hour to wake up,    range: 0-23  <dev>:AlarmClockHour
        B = minute to wake up,  range: 0-59  <dev>:AlarmClockMinute
        C = seconds to wake up, range: 0-59  <dev>:AlarmClockSecond
        D = event number                     <dev>:AlarmClockEvent
        E = debug flag


        Outputs:
        ---------
        VALA = timestamp when alarm clock sounded
        VALB = timestamp when alarm clock last sounded
        Note: timestamp string format
                02/02/2010 10:19:25
                0123456789012345678
        VALC = alarm clock chime (0=sleep, 1=wake_up)
        VALD = timestamp when alarm clock sounded in seconds
        VALE = timestamp when alarm clock last sounded in seconds
        Note: timestamp in seconds is since 0000 Jan 1, 1990

        VALF = timestamp when next alarm clock will sound
        VALG = timestamp when next alarm clock will sound in seconds

        VAL = Not Used

  Rem:

  Side: None

  Ret:  long
        OK    - Successful
        ERROR - String length for reset date and  is too long.

   Auth: Andrew Johnson (Argonne Nantional Lab)

==============================================================================*/
static long alarmClock(aSubRecord *psub)
{
    long           status = SUCCESS;
    epicsTimeStamp now_s;
    epicsUInt32    alarm_hour,alarm_minute,alarm_second,nsec;
    epicsUInt32    now_hour,now_minute,now_second;
    char           now_hour_a[MAX_STRING_SIZE];
    char           now_minute_a[MAX_STRING_SIZE];
    char           now_second_a[MAX_STRING_SIZE];
    char           now_a[MAX_STRING_SIZE];
    static const epicsUInt32   seconds_per_day=(SECONDS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY);

    /* get alarm hour */
    if (*(epicsUInt32 *)psub->a < 0)
      alarm_hour = 0;
    else if (*(epicsUInt32 *)psub->a >= HOURS_PER_DAY)
      alarm_hour =HOURS_PER_DAY-1;
    else
      alarm_hour=*(epicsUInt32 *)psub->a;

    /* get alarm minute */
    if (*(epicsUInt32 *)psub->b < 0)
      alarm_minute=0;
    else if (*(epicsUInt32 *)psub->b > MINUTES_PER_HOUR)
      alarm_minute=MINUTES_PER_HOUR-1;
    else
      alarm_minute=*(epicsUInt32 *)psub->b;

    /* get alarm second */
    if (*(epicsUInt32 *)psub->c < 0)
      alarm_second=0;
    else if (*(epicsUInt32 *)psub->c > SECONDS_PER_MINUTE)
      alarm_second=SECONDS_PER_MINUTE-1;
    else
      alarm_second=*(epicsUInt32 *)psub->c;

    epicsTimeGetCurrent(&now_s);
    epicsTimeToStrftime(now_hour_a,sizeof(now_hour_a),"%H", &now_s);
    epicsTimeToStrftime(now_minute_a,sizeof(now_minute_a),"%M", &now_s);
    epicsTimeToStrftime(now_second_a,sizeof(now_second_a),"%S", &now_s);
    now_hour   = atol(now_hour_a);
    now_minute = atol(now_minute_a);
    now_second = atol(now_second_a);

    /* 
     * Get the time that has passed since the alarm was sounded.
     * This will handle the use case where the ioc was off, when the reset 
     * should have occured.
     */
    nsec =now_s.secPastEpoch - *(epicsUInt32 *)psub->vald; 

    /* Is it time to sound the alarm? */
    if (((now_hour==alarm_hour) && (now_minute==alarm_minute) && (now_second==alarm_second)) ||
	(nsec > seconds_per_day))
    {
       /* debug flag set? */
       if ( *(epicsInt32 *)psub->e ) 
       {  
          epicsTimeToStrftime(now_a,sizeof(now_a),TIME_FORMAT,&now_s);
          printf("...ding ding ding:  %.20s\n",now_a);
          printf("\tCurrent 24-hour period From: %s To: %s\n",(char *)psub->valb,(char *)psub->vala);
       }

        /* Save the last alarm clock time as a sring and in seconds */
        strncpy((char *)psub->valb,(char *)psub->vala,MAX_STRING_SIZE);
        *(epicsUInt32 *)psub->vale=*(epicsUInt32 *)psub->vald;

        /* Get the current reset time as a string and in seconds (from Jan 1, 1990) */
        *(epicsUInt32 *)psub->vald = now_s.secPastEpoch;
        epicsTimeToStrftime((char *)psub->vala,MAX_STRING_SIZE,TIME_FORMAT,&now_s);

        /* Get the next reset time */
        now_s.secPastEpoch += seconds_per_day;  /* add 24-hours in seconds */
        *(epicsUInt32 *)psub->valg = now_s.secPastEpoch;
        epicsTimeToStrftime((char *)psub->valf,MAX_STRING_SIZE,TIME_FORMAT,&now_s);

        *(epicsInt16 *)psub->valc = 1;                      /* sound alarm */
        if (* (epicsInt32 *)psub->d) post_event(*(int *)psub->d);  /* process all pvs this event id */

        if ( *(epicsInt32 *)psub->e)
            printf("\tPrevious 24-hour period From: %.20s To: %.20s\tNext Alarm: %.20s\tevent num=%d\n",
                   (char *)psub->valb,(char *)psub->vala,(char *)psub->valf,*(epicsInt32 *)psub->d);
    }
    else
      *(epicsInt16 *)psub->valc = 0;  /* silence alarm */

    return(status);
}


/*=============================================================================

  Name: dateTime

  Abs:  Output a date and time string based on user inputs, 
        using the format dd-mmm-yyyy hh:mm:ss

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *       psub        read/write pointer to subroutine record


        Input:
        --------
        A = hour          range: 0-23  <dev>:TOTALDOSE_HR
        B = minute        range: 0-59  <dev>:TOTALDOSE_MIN
        C = seconds       range: 0-59  <dev>:TOTALDOSE_SEC
        D = day           range  1-31  <dev>:TOTALDOSE_DAY
        E = month         range  1-31  <dev>:TOTALDOSE_MONTH
        F = year          range  1-31  <dev>:TOTALDOSE_YEAR

        Outputs:
        ---------
        VALA = timestamp when hpi was installed and commissioned
        VALB = flag date valid (0=invalid,1=valid)
        VALC = message string

        Note: timestamp string format
                02/02/2010 10:19:25
                0123456789012345678
        VAL = Not Used

  Rem:

  Side: None

  Ret:  int
           0 - Successful (always)

==============================================================================*/
static long dateTime(aSubRecord *psub)
{
    long           status=SUCCESS;
    epicsUInt32    date_ok=0;
    epicsUInt32    hour,minute,second;
    epicsUInt32    day,month,year;
    epicsUInt32    daysinmonth[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    hour  =*(epicsUInt32 *)psub->a;
    minute=*(epicsUInt32 *)psub->b;
    second=*(epicsUInt32 *)psub->c;
    day   =*(epicsUInt32 *)psub->d;
    month =*(epicsUInt32 *)psub->e;
    year  =*(epicsUInt32 *)psub->c; 

    if (!year || (month>12)) {
      sprintf( (char *)psub->valb,"Date entered is invalid");
      *(epicsUInt32 *)psub->c = 0; 
      return(status);
    }

   /* leap year checking, if ok add 29 days to february */
   if(((year % 400) == 0) || (((year % 100) != 0) && ((year % 4) == 0)))
          daysinmonth[1]=29;

   /* days in month checking */
   if (!year && !month && (month<12))
   {
      if( day <= daysinmonth[month-1] )
        date_ok=1;
   }

   if (date_ok && (hour<HOURS_PER_DAY) && (minute<MINUTES_PER_HOUR) && (second<SECONDS_PER_MINUTE))
      sprintf((char *)psub->vala,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d",day,month,year,hour,minute,second);
   else
      sprintf((char *)psub->c,"It's not a valid date!");
 
    *(epicsUInt32 *)psub->b=date_ok;
    return(status);
}

/*=============================================================================

  Name: doseStatus

  Abs:  Dose rate status

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *         psub        read/write pointer to subroutine record


        Input:
        --------
        INPA = dose rate                                    <bl>GammaDoseRate
        INPB = severity of dose rate data pv                <bl>GammaDoseRate.SEVR
        INPC = event number to post timestamp pv as string  <bl>GammaDoseRateTimeEvent
        INPD = debug flag (Off=0 On=1)                      <bl>GammaDebug
        INPE = max consecutive neg. dose rate               <nl>GammaNegErrLimit
        INPF = hpi online status                            <bl>GammaCommLink
        INPG = neg dose cnt                                 <bl>GammaNegCnt
        INPH = dose rate lower limit                        <bl>GammaDoseLowLimit

        Outputs:
        ---------
        VALA = time when last data was received (timestamp)
        VALB = time when last data was received (string)
        VALC = time when last data was received (nsec)
        VALD = time diff in seconds between current and last data read (nsec)
        VALE = count negative consecutive 
        VALF = negative does rate error (if VALE > F then VALF=1 else VALF=0)
        VALG = gamma dose over-range status (0=ok, 1=over range)

        Note: timestamp string format
                02/02/2010 10:19:25
                0123456789012345678
        VAL  = Not Used

  Rem:

  Side: None

  Ret:  int
           0 - Successful 
          -1 - Failure, due to invalid timestamp

==============================================================================*/
static long doseStatus(aSubRecord *psub)
{
    long  status = SUCCESS;
    int   len = 0;
    static const double max_dose = 1000.;                    /* maximum dose in mRem/hr        */
    static const epicsInt32  max_positive_num = 0x7fffffff;  /* largest postive 32-bit number  */


    if ( *(epicsInt16 *)psub->d ) 
        printf("Record %s called doseStatus(%p)\n", psub->name, (void*) psub);

    recGblGetTimeStamp(psub);
    /* 
     * Is the first time data was received? If yes, then
     * we are unable to determine the time since we last read
     * data so set the delta time to 1 second.
     */
    if (*(double *)psub->valc==0.0) 
      *(double *)psub->vald = 1.0; 
    else
      *(double *)psub->vald = epicsTimeDiffInSeconds(&psub->time,(epicsTimeStamp *)psub->vala);

    /* save current timestamp of when data was received */
    memcpy((char *)psub->vala,(char *)&psub->time,sizeof(psub->time));
   
    /* save current timestamp in seconds  */
    *(double *)psub->valc = psub->time.secPastEpoch + (double)psub->time.nsec * 1e-9;

    /* save current timestamp as a string */
    len = epicsTimeToStrftime((char *)psub->valb,MAX_STRING_SIZE,TIME_FORMAT,&psub->time);
    if (len > MAX_STRING_SIZE) {
        if (*(epicsInt16 *)psub->d) printf("string length too long %d\n",len);
        psub->udf = TRUE;
        recGblSetSevr(psub, UDF_ALARM, INVALID_ALARM);
    }
    else {
      psub->udf = FALSE;  
      if (*(epicsInt32 *)psub->c)  post_event(*(int *)psub->c); /* process all pvs this event id */
    }

    /* if hpi is offline then we're done */
    if (*(epicsInt16 *)psub->f==0) {
      *(epicsUInt32 *)psub->vale = 0;
      *(epicsInt16  *)psub->valf = 0;
    }
    else {
      /*
       * Check if dose is over range, which indicates a
       * "Over 1000 mrem/h". The hpi display actually
       * goes slightly over 1000 mrem/h, to allow
       * veification of full scale response.
       * see hpi manual p5, for details.
       */
      if  ( *(double *)psub->a >max_dose )
         *(epicsInt16 *)psub->valg = 1;  /* set dose over range */
      else
         *(epicsInt16 *)psub->valg = 0;
      /* 
       * Count for consecutive neg. dose rates.
       * if count goes above a maximum then the
       * hpi is broken. 
       * Note: the does rate shoud be a postive 
       *       if a source has been installed.
       */
      if (*(double *)psub->a <= *(double *)psub->h) { /* is dose below allowed low limit?  */
        /* 
	 * keep count of the number of consecutive neg. dose rates.
	 * Note: the count should be positive check first to see
	 * if we have reached the largest 32-bit positive number
	 * possible. If so, then leave then reset the counter
	 * to the max allowed and continue counting.
         */
        if ( *(epicsInt32 *)psub->g == max_positive_num )
	   *(epicsInt32 *)psub->vale = *(epicsInt32 *)psub->e; /* reset to minimum cnt for  a neg dose error */
        else
           *(epicsInt32 *)psub->vale = *(epicsInt32 *)psub->g + 1; 

        /* have we reached the limit allowed? */
        if ( *(epicsInt32 *)psub->vale >= *(epicsInt32 *)psub->e)
           *(epicsInt32 *)psub->valf = 1;
	else 
           *(epicsInt32 *)psub->valf = 0;
      }
      else { 
        *(epicsUInt32 *)psub->vale = 0;
        *(epicsInt16  *)psub->valf = 0;
      } 
    }     
    if ( *(epicsInt16 *)psub->d)  printf("\tExiting doseStatus\n");
    return(status);
}

/*=============================================================================

  Name: isDatatMissing

  Abs:  Check for missing data based on the maximum time difference allowed
        since the data was last received.

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *        psub        read/write pointer to subroutine record


        Input:
        --------
        INPA = dose rate time               
        INPB = max seconds allowed between data reads, before data is considered missing
        INPC = event number to post timestamp pv as string
        INPD = debug flag (0=Off,1=On)

        Outputs:
        ---------
        VALA = time diff in seconds between current time and last data read (sec)
        VALB = data missing flag (0=data not missing, 1=data missing, time lapse exceeded)
        VALC = time when data when first when missing (string)       
        VAL  = Not Used

  Rem:

  Side: None

  Ret:  int
           0 - Successful (always)

==============================================================================*/
static long isDataMissing(aSubRecord *psub)
{

    long  status = SUCCESS;
    int   len = 0;

    if ( *(epicsInt16 *)psub->d ) 
        printf("Record %s called doseStatus(%p)\n", psub->name, (void*) psub);

    /* How long since data was last read */
    recGblGetTimeStamp(psub);
    *(double *)psub->vala = epicsTimeDiffInSeconds(&psub->time,(epicsTimeStamp *)psub->a);

   /* 
    * if we haven't received any data for more than then allotted of time, 
    * consider data missing. However, only post an event if last time we
    * checked data wasn't considered missing. 
    */
    if (*(double *)psub->vala < *(double *)psub->b) 
       *(epicsInt16 *)psub->valb = 0; 
    else if  (*(epicsInt16 *)psub->valb==0) {
       *(epicsInt16 *)psub->valb = 1;   /* flag data is missing */
       /* get date when data went missing */
       len = epicsTimeToStrftime((char *)psub->valc,MAX_STRING_SIZE,TIME_FORMAT,&psub->time);
       if ( *(epicsInt32 *)psub->c) 
         post_event(*(int *)psub->c);      /* process all pvs this event id   */
       if ( *(epicsInt16 *)psub->d ) 
         printf("\tRecord %s: data  missing\n", psub->name);
    }
    return(status);
}


/*=============================================================================

  Name: HVStatus

  Abs:  Check HV Status

  Args: Type                Name        Access     Description
        ------------------- ----------- ---------- ----------------------------
        aSubRecord *        psub        read/write pointer to subroutine record


        Input:
        --------
        INPA = HV Status   (5-bits)          <bl>GammaHVPulseStatus
        INPB = Dose Status (5-bits)          <bl>GammaDoseStatus
        INPC = alarm severity                <bl>GammaAlarmSeverity 
        IPPD = debug flag                    <bl>GammaDebug  (0=debug off, 1=debug on)


        Outputs:
        ---------
        VALA = HV status msg bitmask         
        VALB = HV Failure status             0=no failure, 1=failure

  Rem:

  Side: None

  Ret:  int
           0 - Successful (always)

==============================================================================*/
static long HVStatus(aSubRecord *psub)
{
#define HV_STATUS_OK            0x0
#define HV_STATUS_TRIP_LATCH    0x08
#define HV_STATUS_FAIL          0x10
#define HV_STATUS_NOHV          0x11
#define HV_STATUS_NOPULSE       0x12
#define HV_STATUS_TRIP          0x14
#define HV_STATUS_RESET         0x1c
#define HV_STATUS_MASK          0x1f
#define DOSE_STATUS_TRIP        0x04

    long         status = SUCCESS;
    epicsUInt16  hv_stat   =  *(epicsUInt16 *)psub->a; 
    epicsUInt16  dose_stat =  *(epicsUInt16 *)psub->b;
   
    /* determine hv fail status */
    if ( hv_stat &HV_STATUS_MASK ) {
        *(epicsUInt16 *)psub->valb = 0;               /* fail bit in hv status byte */
        
       /* determine type of hv failure, set psub->val for mbbo hv status summary pv */
       if ( hv_stat == HV_STATUS_NOHV )
          *(epicsUInt16 *)psub->vala = 2;                     /* hv failure ,not HV on detector  */
       else if ( hv_stat == HV_STATUS_NOPULSE )
          *(epicsUInt16 *)psub->vala = 3;                     /* hv pulse failure                */
       else if (((hv_stat & HV_STATUS_TRIP)==HV_STATUS_TRIP) || (dose_stat & DOSE_STATUS_TRIP))
          *(epicsUInt16 *)psub->vala = 4;                     /* low level alarm tripped         */     
       else if ( hv_stat == HV_STATUS_RESET )
          *(epicsUInt16 *)psub->vala = 5;                     /* waiting for reset, failure      */
    }
    else {
        *(epicsUInt16 *)psub->valb = 0;         /* fail bit in hv status byte */

       if ( hv_stat == HV_STATUS_OK )
          *(epicsUInt16 *)psub->vala = HV_STATUS_OK;    /* normal operations  */
       else if ( hv_stat == HV_STATUS_TRIP_LATCH )
          *(epicsUInt16 *)psub->vala = 1;              /* waiting for reset   */
       else 
          *(epicsUInt16 *)psub->vala = 6;              /* other               */
    }

    return(status);
}



/* Register these symbols for use by IOC code: */

epicsExportAddress(int,mySubDebug);
epicsRegisterFunction(mySubInit);
epicsRegisterFunction(mySubProcess);
epicsRegisterFunction(myAsubInit);
epicsRegisterFunction(myAsubProcess);
epicsRegisterFunction(alarmClock);
epicsRegisterFunction(alarmClockInit);
epicsRegisterFunction(alarmPeriod);
epicsRegisterFunction(alarmPeriodInit);
epicsRegisterFunction(dateTime);
epicsRegisterFunction(doseStatus);
epicsRegisterFunction(isDataMissing);
epicsRegisterFunction(HVStatus);

