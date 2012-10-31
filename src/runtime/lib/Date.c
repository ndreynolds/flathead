// Date.c
// ------
// Date object implementation. 
// Portions borrowed or inspired from Mozilla Rhino.

#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "Date.h"


const int  hr_per_day  = 24;
const int  min_per_hr  = 60;
const int  sec_per_min = 60;
const int  ms_per_sec  = 1000;
const long ms_per_min  = 60000;
const long ms_per_hr   = 3600000;
const long ms_per_day  = 86400000;

// new Date()
// new Date(value)
// new Date(dateString)
// new Date(year, month, day[, hour, minute, second, millisecond])
JSValue *
date_new(JSValue *instance, JSArgs *args, State *state)
{
  JSValue *utc;
  int len = ARGLEN(args);

  // Date()
  if (len == 0)
    utc = JSNUM(utc_now());

  // Date(value|dateString)
  else if (len == 1) {
    JSValue *arg = ARG(args, 0);
    if (IS_NUM(arg))
      utc = JSNUM(arg->number.val);
    else
      assert(0);
  }

  // new Date(year, month, day[, hour, minute, second, millisecond])
  else {
    utc = JSNUM(utc_time(time_from_args(args)->number.val));
  }

  if (state->construct)
    state->this->object.wraps = utc;
  return JSSTR("TODO");
}

// Date.now()
JSValue *
date_now(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(utc_now());
}

// Date.parse(dateString)
JSValue *
date_parse(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.UTC(year, month[, date[, hours[, minutes[, seconds[, ms]]]]])
JSValue *
date_utc(JSValue *instance, JSArgs *args, State *state)
{
  return time_from_args(args);
}

// Date.isDST()   Non-standard
JSValue *
date_is_dst(JSValue *instance, JSArgs *args, State *state)
{
  time_t now = time(NULL);
  struct tm *loc_tm = localtime(&now);
  return JSBOOL(loc_tm->tm_isdst);
}

// Date.prototype.getDate()
JSValue *
date_proto_get_date(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(date_from_time(local_time(instance->number.val)));
}

// Date.prototype.getDay()
JSValue *
date_proto_get_day(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(week_day(local_time(instance->number.val)));
}

// Date.prototype.getFullYear()
JSValue *
date_proto_get_full_year(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(year_from_time(local_time(instance->number.val)));
}

// Date.prototype.getHours()
JSValue *
date_proto_get_hours(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(hour_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMilliseconds()
JSValue *
date_proto_get_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(ms_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMinutes()
JSValue *
date_proto_get_minutes(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(min_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMonth()
JSValue *
date_proto_get_month(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(month_from_time(local_time(instance->number.val)));
}

// Date.prototype.getSeconds()
JSValue *
date_proto_get_seconds(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(sec_from_time(local_time(instance->number.val)));
}

// Date.prototype.getTime()
JSValue *
date_proto_get_time(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

// Date.prototype.getTimezoneOffset()
JSValue *
date_proto_get_timezone_offset(JSValue *instance, JSArgs *args, State *state)
{
  double t = instance->number.val;
  return JSNUM((t - local_time(t)) / ms_per_min);
}

// Date.prototype.getUTCDate()
JSValue *
date_proto_get_utc_date(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(date_from_time(instance->number.val));
}

// Date.prototype.getUTCDay()
JSValue *
date_proto_get_utc_day(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(week_day(instance->number.val));
}

// Date.prototype.getUTCFullYear()
JSValue *
date_proto_get_utc_full_year(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(year_from_time(instance->number.val));
}

// Date.prototype.getUTCHours()
JSValue *
date_proto_get_utc_hours(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(hour_from_time(instance->number.val));
}

// Date.prototype.getUTCMilliseconds()
JSValue *
date_proto_get_utc_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(ms_from_time(instance->number.val));
}

// Date.prototype.getUTCMinutes()
JSValue *
date_proto_get_utc_minutes(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(min_from_time(instance->number.val));
}

// Date.prototype.getUTCMonth()
JSValue *
date_proto_get_utc_month(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(month_from_time(instance->number.val));
}

// Date.prototype.getUTCSeconds()
JSValue *
date_proto_get_utc_seconds(JSValue *instance, JSArgs *args, State *state)
{
  return JSNUM(sec_from_time(instance->number.val));
}

// Date.prototype.getYear()
JSValue *
date_proto_get_year(JSValue *instance, JSArgs *args, State *state)
{
  int y = year_from_time(instance->number.val);
  return JSNUM(y - 1900);
}

// Date.prototype.setDate(dayValue)
JSValue *
date_proto_set_date(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setFullYear(yearValue[, monthValue[, dayValue]])
JSValue *
date_proto_set_full_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setHours(hourValue[, minutesValue[, secondsValue[, msValue]]])
JSValue *
date_proto_set_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setMilliseconds(msValue)
JSValue *
date_proto_set_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setMinutes(minutesValue[, secondsValue[, msValue]])
JSValue *
date_proto_set_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setMonth(monthValue)
JSValue *
date_proto_set_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setSeconds(secondsValue[, msValue])
JSValue *
date_proto_set_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setTime(timeValue)
JSValue *
date_proto_set_time(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCDate(dayValue)
JSValue *
date_proto_set_utc_date(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCFullYear(yearValue[, monthValue[, dayValue]])
JSValue *
date_proto_set_utc_full_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCHours(hoursValue[, minutesValue[, secondsValue[, msValue]]])
JSValue *
date_proto_set_utc_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCMilliseconds(msValue)
JSValue *
date_proto_set_utc_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCMinutes(
JSValue *
date_proto_set_utc_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCMonth(
JSValue *
date_proto_set_utc_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setUTCSeconds(
JSValue *
date_proto_set_utc_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.setYear(
JSValue *
date_proto_set_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toDateString()
JSValue *
date_proto_to_date_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toISOString()
JSValue *
date_proto_to_iso_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toJSON()
JSValue *
date_proto_to_json(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toGMTString()
JSValue *
date_proto_to_gmt_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toLocaleDateString()
JSValue *
date_proto_to_locale_date_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toLocaleString()
JSValue *
date_proto_to_locale_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toLocaleTimeString()
JSValue *
date_proto_to_locale_time_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toString()
JSValue *
date_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toTimeString()
JSValue *
date_proto_to_time_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.toUTCString()
JSValue *
date_proto_to_utc_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.prototype.valueOf()
JSValue *
date_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  return instance;
}

double
day(double t)
{
  return floor(t / ms_per_day);
}

long
week_day(double t)
{
  double res = fmod(day(t) + 4, 7);
  if (res < 0)
    res += 7;
  return (long)res;
}

double
days_in_year(double y)
{
  long yd = (long)y;
  if (yd % 4 != 0) return 365;
  if (yd % 100 != 0) return 366;
  if (yd % 400 != 0) return 365;
  return 366;
}

double
day_from_year(double y)
{
  return 365 * (y - 1970) + floor((y - 1969) / 4.0) - 
    floor((y - 1901) / 100.0) + floor((y - 1601) / 400.0);
}

double
date_from_time(double t)
{
  long y = year_from_time(t);
  long d = day(t) - day_from_year(y);

  d -= 31 + 28;
  if (d < 0)
    return d < -28 ? d + 31 + 28 + 1 : d + 28 + 1;

  if (in_leap_year(t)) {
    if (d == 0) return 29;
    --d;
  }

  long mdays, mstart;
  switch(d / 30) {
    case 0: return d + 1;
    case 1: mdays = 31; mstart = 31; break;
    case 2: mdays = 30; mstart = 31+30; break;
    case 3: mdays = 31; mstart = 31+30+31; break;
    case 4: mdays = 30; mstart = 31+30+31+30; break;
    case 5: mdays = 31; mstart = 31+30+31+30+31; break;
    case 6: mdays = 31; mstart = 31+30+31+30+31+31; break;
    case 7: mdays = 30; mstart = 31+30+31+30+31+31+30; break;
    case 8: mdays = 31; mstart = 31+30+31+30+31+31+30+31; break;
    case 9: mdays = 30; mstart = 31+30+31+30+31+31+30+31+30; break;
    case 10:
        return d - (31+30+31+30+31+31+30+31+30) + 1;
  }
  d -= mstart;
  if (d < 0) d += mdays;
  return d + 1;
}

double
day_from_month(long m, long y)
{
  long day = m * 30;

  if (m >= 7) day += m / 2 - 1;
  else if (m >= 2) day += (m - 1) / 2 - 1;
  else day += m;

  if (m >= 2 && in_leap_year(time_from_year(y))) 
    ++day;

  return day;
}

double
time_from_year(double y)
{
  return day_from_year(y) * ms_per_day;
}

double
day_within_year(double t)
{
  return day(t) - day_from_year(year_from_time(t));
}

long
month_from_time(double t)
{
  long leap = in_leap_year(t);
  long day = day_within_year(t);
  if (0 <= day && day < 31) return 0;
  if (31 <= day && day < 59 + leap) return 1;
  if (59 + leap <= day && day < 90 + leap) return 2;
  if (90 + leap <= day && day < 120 + leap) return 3;
  if (120 + leap <= day && day < 151 + leap) return 4;
  if (151 + leap <= day && day < 181 + leap) return 5;
  if (181 + leap <= day && day < 212 + leap) return 6;
  if (212 + leap <= day && day < 243 + leap) return 7;
  if (243 + leap <= day && day < 273 + leap) return 8;
  if (273 + leap <= day && day < 304 + leap) return 9;
  if (304 + leap <= day && day < 334 + leap) return 10;
  return 11;
}

double
year_from_time(double t)
{
  long lo = floor((t / ms_per_day) / 366) + 1970;
  long hi = floor((t / ms_per_day) / 365) + 1970;
  long mid;

  if (hi < lo) {
    long temp = lo;
    lo = hi;
    hi = temp;
  }

  while (hi > lo) {
    mid = (hi + lo) / 2;
    if (time_from_year(mid) > t)
      hi = mid - 1;
    else {
      lo = mid + 1;
      if (time_from_year(lo) > t)
        return mid;
    }
  }

  return lo;
}

long
hour_from_time(double t)
{
  double res = fmod(floor(t / ms_per_hr), hr_per_day);
  if (res < 0)
    res += hr_per_day;
  return res;
}

long
min_from_time(double t)
{
  double res = fmod(floor(t / ms_per_min), min_per_hr);
  if (res < 0)
    res += min_per_hr;
  return res;
}

long
sec_from_time(double t)
{
  double res = fmod(floor(t / ms_per_sec), sec_per_min);
  if (res < 0)
    res += sec_per_min;
  return res;
}

long
ms_from_time(double t)
{
  double res = fmod(t, ms_per_sec);
  if (res < 0)
    res += ms_per_sec;
  return res;
}

double
in_leap_year(double t)
{
  return days_in_year(year_from_time(t)) == 366;
}

double
local_time(double t)
{
  return t - utc_offset() - dst_offset(t);
}

double
utc_offset()
{
  time_t now = time(NULL);
  struct tm *loc_tm = localtime(&now);
  time_t gmt_sec = mktime(loc_tm);
  time_t loc_sec = mktime(gmtime(&now));

  return (loc_sec - gmt_sec) * ms_per_sec;
}

double
utc_time(double t)
{
  return t + utc_offset() + dst_offset(t + utc_offset());
}

double
utc_now()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}

double
dst_offset(double raw_t)
{
  time_t t = raw_t / 1000;
  struct tm *loc_tm = localtime(&t);
  return loc_tm->tm_isdst ? -ms_per_hr : 0;
}

// new Date(year, month, day[, hour, minute, second, millisecond])
// Date.UTC(year, month[, date[, hours[, minutes[, seconds[, ms]]]]])
JSValue *
time_from_args(JSArgs *args)
{
  JSValue *year    = ARG(args, 0),
          *month   = ARG(args, 1),
          *date    = ARG(args, 2),
          *hours   = ARG(args, 3),
          *minutes = ARG(args, 4),
          *seconds = ARG(args, 5),
          *ms      = ARG(args, 6);

  JSValue *y   = TO_NUM(year),
          *m   = TO_NUM(month),
          *dt  = IS_UNDEF(date) ? JSNUM(1) : TO_NUM(date),
          *h   = IS_UNDEF(hours) ? JSNUM(0) : TO_NUM(hours),
          *min = IS_UNDEF(minutes) ? JSNUM(0) : TO_NUM(minutes),
          *s   = IS_UNDEF(seconds) ? JSNUM(0) : TO_NUM(seconds),
          *mil = IS_UNDEF(ms) ? JSNUM(0) : TO_NUM(ms);

  // Resolve a 2-digit year to 4 digits, relative to the 20th century.
  long yi = floor(y->number.val);
  if (yi >= 0 && yi <= 99)
    yi = 1900 + yi;
  JSValue *yr = JSNUM(yi);

  JSValue *day = make_day(yr, m, dt);
  JSValue *time = make_time(h, min, s, mil);
  return time_clip(make_date(day, time));
}

JSValue *
make_time(JSValue *hour, JSValue *min, JSValue *sec, JSValue *ms)
{
  double h = TO_NUM(hour)->number.val, 
      m = TO_NUM(min)->number.val,
      s = TO_NUM(sec)->number.val,
      l = TO_NUM(ms)->number.val;

  return JSNUM(((h * min_per_hr + m) * sec_per_min + s) * ms_per_sec + l);
}

JSValue *
make_day(JSValue *year, JSValue *month, JSValue *date)
{
  if (IS_INF(year) || IS_INF(date))
    return JSNAN();

  double y = TO_NUM(year)->number.val, 
      m = TO_NUM(month)->number.val,
      dt = TO_NUM(date)->number.val,
      mn = fmod(m, 12);

  y += floor(mn / 12);
  if (mn < 0) mn += 12;

  double yd = floor(time_from_year(y) / ms_per_day);
  double md = day_from_month(mn, y);

  return JSNUM(yd + md + dt - 1);
}

JSValue *
make_date(JSValue *day, JSValue *time)
{
  if (IS_INF(day) || IS_INF(time))
    return JSNAN();

  double d = TO_NUM(day)->number.val,
      t = TO_NUM(time)->number.val;

  return JSNUM(d * ms_per_day + t);
}

JSValue *
time_clip(JSValue *time)
{
  if (IS_INF(time))
    return JSNAN();
  if (abs(time->number.val) > 8.64e15)
    return JSNAN();
  return TO_NUM(time);
}

JSValue *
bootstrap_date()
{
  JSValue *date = JSNFUNC(&date_new);
  JSValue *proto = JSOBJ();

  // Date
  // ----

  // Properties
  BUILTIN(date, "prototype", proto);

  // Methods
  BUILTIN(date, "now", JSNFUNC(&date_now));
  BUILTIN(date, "parse", JSNFUNC(&date_parse));
  BUILTIN(date, "UTC", JSNFUNC(&date_utc));
  BUILTIN(date, "isDST", JSNFUNC(&date_is_dst));

  // Date.prototype
  // --------------
  
  // Properties
  BUILTIN(proto, "constructor", JSNFUNC(&date_new));
  
  // Methods
  BUILTIN(proto, "getDate", JSNFUNC(&date_proto_get_date));
  BUILTIN(proto, "getDay", JSNFUNC(&date_proto_get_day));
  BUILTIN(proto, "getHours", JSNFUNC(&date_proto_get_hours));
  BUILTIN(proto, "getFullYear", JSNFUNC(&date_proto_get_full_year));
  BUILTIN(proto, "getMilliseconds", JSNFUNC(&date_proto_get_milliseconds));
  BUILTIN(proto, "getMinutes", JSNFUNC(&date_proto_get_minutes));
  BUILTIN(proto, "getMonth", JSNFUNC(&date_proto_get_month));
  BUILTIN(proto, "getSeconds", JSNFUNC(&date_proto_get_seconds));
  BUILTIN(proto, "getTime", JSNFUNC(&date_proto_get_time));
  BUILTIN(proto, "getTimezoneOffset", JSNFUNC(&date_proto_get_timezone_offset));
  BUILTIN(proto, "getUTCDate", JSNFUNC(&date_proto_get_utc_date));
  BUILTIN(proto, "getUTCDay", JSNFUNC(&date_proto_get_utc_day));
  BUILTIN(proto, "getUTCHours", JSNFUNC(&date_proto_get_utc_hours));
  BUILTIN(proto, "getUTCFullYear", JSNFUNC(&date_proto_get_utc_full_year));
  BUILTIN(proto, "getUTCMilliseconds", JSNFUNC(&date_proto_get_utc_milliseconds));
  BUILTIN(proto, "getUTCMinutes", JSNFUNC(&date_proto_get_utc_minutes));
  BUILTIN(proto, "getUTCMonth", JSNFUNC(&date_proto_get_utc_month));
  BUILTIN(proto, "getUTCSeconds", JSNFUNC(&date_proto_get_utc_seconds));
  BUILTIN(proto, "getYear", JSNFUNC(&date_proto_get_year));

  BUILTIN(proto, "setDate", JSNFUNC(&date_proto_set_date));
  BUILTIN(proto, "setFullYear", JSNFUNC(&date_proto_set_full_year));
  BUILTIN(proto, "setHours", JSNFUNC(&date_proto_set_hours));
  BUILTIN(proto, "setMilliseconds", JSNFUNC(&date_proto_set_milliseconds));
  BUILTIN(proto, "setMinutes", JSNFUNC(&date_proto_set_minutes));
  BUILTIN(proto, "setMonth", JSNFUNC(&date_proto_set_month));
  BUILTIN(proto, "setSeconds", JSNFUNC(&date_proto_set_seconds));
  BUILTIN(proto, "setTime", JSNFUNC(&date_proto_set_time));
  BUILTIN(proto, "setUTCDate", JSNFUNC(&date_proto_set_utc_date));
  BUILTIN(proto, "setUTCFullYear", JSNFUNC(&date_proto_set_utc_full_year));
  BUILTIN(proto, "setUTCHours", JSNFUNC(&date_proto_set_utc_hours));
  BUILTIN(proto, "setUTCMilliseconds", JSNFUNC(&date_proto_set_utc_milliseconds));
  BUILTIN(proto, "setUTCMinutes", JSNFUNC(&date_proto_set_utc_minutes));
  BUILTIN(proto, "setUTCMonth", JSNFUNC(&date_proto_set_utc_month));
  BUILTIN(proto, "setUTCSeconds", JSNFUNC(&date_proto_set_utc_seconds));
  BUILTIN(proto, "setYear", JSNFUNC(&date_proto_set_year));

  BUILTIN(proto, "toLocaleString", JSNFUNC(&date_proto_to_locale_string));
  BUILTIN(proto, "toLocaleTimeString", JSNFUNC(&date_proto_to_locale_time_string));
  BUILTIN(proto, "toString", JSNFUNC(&date_proto_to_string));
  BUILTIN(proto, "toTimeString", JSNFUNC(&date_proto_to_time_string));
  BUILTIN(proto, "toUTCString", JSNFUNC(&date_proto_to_utc_string));
  BUILTIN(proto, "valueOf", JSNFUNC(&date_proto_value_of));

  return date;
}
