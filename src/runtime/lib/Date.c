// Date.c
// ------

#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "Date.h"

const int hr_per_day  = 24;
const int min_per_hr  = 60;
const int sec_per_min = 60;
const int ms_per_sec  = 1000;
const int ms_per_min  = 60000;
const int ms_per_hr = 3600000;
const int ms_per_day  = 86400000;

JSValue *
date_new(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

// Date.now()
JSValue *
date_now(JSValue *instance, JSArgs *args, State *state)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  printf("%g\n", (double)(t.tv_sec + t.tv_usec / 1000));
  return JSNUM(t.tv_sec + t.tv_usec / 1000);
}

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

  int yi = floor(y->number.val);
  JSValue *yr = JSNUM((!IS_NAN(y) && 0 <= floor(yi) && floor(yi) <= 99) ?  
                      1900 + floor(yi) : y->number.val);

  return time_clip(make_date(make_day(yr, m, dt), make_time(h, min, s, mil)));
}

// Date.
JSValue *
date_proto_get_date(JSValue *instance, JSArgs *args, State *state)
{
  int t = fh_get(instance, "time")->number.val;
  // 
  return JSNUM(date_from_time(t));
}

JSValue *
date_proto_get_day(JSValue *instance, JSArgs *args, State *state)
{
  int t = fh_get(instance, "time")->number.val;
  return JSNUM(week_day(t));
}

JSValue *
date_proto_get_full_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_time(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_timezone_offset(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_date(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_day(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_utc_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_get_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_date(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_full_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_time(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_date(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_full_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_hours(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_milliseconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_minutes(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_month(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_utc_seconds(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_set_year(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}


JSValue *
date_proto_to_date_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_iso_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_json(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_gmt_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_locale_date_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_locale_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_locale_time_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_time_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_to_utc_string(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

JSValue *
date_proto_value_of(JSValue *instance, JSArgs *args, State *state)
{
  // TODO
  return JSUNDEF();
}

int
day(int t)
{
  return floor(t / ms_per_day);
}

int
week_day(int t)
{
  return (day(t) + 4) % 7;
}

int
days_in_year(int y)
{
  if (y % 4 != 0) return 365;
  if (y % 100 != 0) return 366;
  if (y % 400 != 0) return 365;
  return 366;
}

int
day_from_year(int y)
{
  return 365 * (y - 1970) + floor((y - 1969) / 4) - 
    floor((y - 1901) / 100) + floor((y - 1601) / 400);
}

int
date_from_time(int t)
{
  int m = month_from_time(t);
  switch(m) {
    case 0: return day_within_year(t) + 1;
    case 1: return day_within_year(t) - 30;
    case 2: return day_within_year(t) - 58 - in_leap_year(t);
    case 3: return day_within_year(t) - 89 - in_leap_year(t);
    case 4: return day_within_year(t) - 119 - in_leap_year(t);
    case 5: return day_within_year(t) - 150 - in_leap_year(t);
    case 6: return day_within_year(t) - 180 - in_leap_year(t);
    case 7: return day_within_year(t) - 211 - in_leap_year(t);
    case 8: return day_within_year(t) - 242 - in_leap_year(t);
    case 9: return day_within_year(t) - 272 - in_leap_year(t);
    case 10: return day_within_year(t) - 303 - in_leap_year(t);
    default: return day_within_year(t) - 333 - in_leap_year(t);
  }
}

int
day_from_month(int m, int y)
{
  int day = m * 30;

  if (m >= 7) day += m / 2 - 1;
  else if (m >= 2) day += (m - 1) / 2 - 1;
  else day += m;

  if (m >= 2 && in_leap_year(time_from_year(y))) 
    ++day;

  return day;
}

int
time_from_year(int y)
{
  return day_from_year(y) * ms_per_day;
}

int
day_within_year(int t)
{
  return day(t) - day_from_year(year_from_time(t));
}

int
month_from_time(int t)
{
  int leap = in_leap_year(t);
  int day = day_within_year(t);
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

int
year_from_time(int t)
{
  int time, last = 1, y = 0;
  for (;y < INT_MAX && y < t; y++) {
    time = time_from_year(y);
    if (time > t)
      return last;
    else if (time == t)
      return time;
  }
  return INT_MAX - 1;
}

int
in_leap_year(int t)
{
  return days_in_year(year_from_time(t)) == 366;
}

JSValue *
make_time(JSValue *hour, JSValue *min, JSValue *sec, JSValue *ms)
{
  int h = TO_NUM(hour)->number.val, 
      m = TO_NUM(min)->number.val,
      s = TO_NUM(sec)->number.val,
      l = TO_NUM(ms)->number.val;

  return JSNUM(h * ms_per_hr + m * ms_per_min + s * ms_per_sec + l);
}

JSValue *
make_day(JSValue *year, JSValue *month, JSValue *date)
{
  if (IS_INF(year) || IS_INF(date))
    return JSNAN();

  int y = TO_NUM(year)->number.val, 
      m = TO_NUM(month)->number.val,
      dt = TO_NUM(date)->number.val,
      ym = y + floor(m / 12),
      mn = m % 12;

  int yd = floor(time_from_year(ym) / ms_per_day);
  int md = day_from_month(mn, ym);

  return JSNUM(yd + md + dt - 1);
}

JSValue *
make_date(JSValue *day, JSValue *time)
{
  if (IS_INF(day) || IS_INF(time))
    return JSNAN();

  int d = TO_NUM(day)->number.val,
      t = TO_NUM(time)->number.val;

  return JSNUM(d * ms_per_day + t);
}

JSValue *
time_clip(JSValue *time)
{
  if (IS_INF(time))
    return JSNAN();
  if (abs(time->number.val) > 8.64E15)
    return JSNAN();
  return TO_NUM(time);
}

JSValue *
bootstrap_date()
{
  JSValue *date = JSOBJ();
  JSValue *proto = JSOBJ();

  // Date
  // ----

  // Properties
  BUILTIN(date, "prototype", proto);

  // Methods
  BUILTIN(date, "now", JSNFUNC(&date_now));
  BUILTIN(date, "parse", JSNFUNC(&date_parse));
  BUILTIN(date, "UTC", JSNFUNC(&date_utc));

  // Date.prototype
  // --------------
  
  // Properties
  BUILTIN(proto, "constructor", JSNFUNC(&date_new));
  
  // Methods
  BUILTIN(proto, "getDate", JSNFUNC(&date_proto_get_date));
  BUILTIN(proto, "getDay", JSNFUNC(&date_proto_get_day));
  BUILTIN(proto, "getFullYear", JSNFUNC(&date_proto_get_full_year));
  BUILTIN(proto, "getMonth", JSNFUNC(&date_proto_get_month));
  BUILTIN(proto, "getSeconds", JSNFUNC(&date_proto_get_seconds));
  BUILTIN(proto, "getTime", JSNFUNC(&date_proto_get_time));
  BUILTIN(proto, "getTimezoneOffset", JSNFUNC(&date_proto_get_timezone_offset));
  BUILTIN(proto, "getUTCDate", JSNFUNC(&date_proto_get_utc_date));
  BUILTIN(proto, "getUTCMilliseconds", JSNFUNC(&date_proto_get_utc_milliseconds));
  BUILTIN(proto, "getUTCMinutes", JSNFUNC(&date_proto_get_utc_minutes));
  BUILTIN(proto, "getUTCMonth", JSNFUNC(&date_proto_get_utc_month));
  BUILTIN(proto, "getUTCSeconds", JSNFUNC(&date_proto_get_utc_seconds));
  BUILTIN(proto, "getYear", JSNFUNC(&date_proto_get_year));
  BUILTIN(proto, "setDate", JSNFUNC(&date_proto_set_date));
  BUILTIN(proto, "setFullYear", JSNFUNC(&date_proto_set_full_year));
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
