// Date.c
// ------
// Date object implementation. 
// Portions borrowed or inspired from Mozilla Rhino.

#include <math.h>
#include <ctype.h>
#include <stdlib.h>
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
js_val *
date_new(js_val *instance, js_args *args, eval_state *state)
{
  js_val *utc;
  int len = ARGLEN(args);

  // Date()
  if (!state->construct)
    return date_format_loc(utc_now(), true, true);

  // new Date()
  if (len == 0)
    utc = JSNUM(utc_now());

  // new Date(value|dateString)
  else if (len == 1) {
    js_val *arg = ARG(args, 0);
    if (IS_NUM(arg))
      utc = JSNUM(arg->number.val);
    else 
      utc = date_parse_str(TO_STR(arg)->string.ptr);
  }

  // new Date(year, month, day[, hour, minute, second, millisecond])
  else {
    utc = JSNUM(utc_time(ms_from_args(args)));
  }

  fh_set_class(state->this, "Date");
  state->this->object.primitive = utc;
  return state->this;
}

// Date.now()
js_val *
date_now(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(utc_now());
}

// Date.parse(dateString)
js_val *
date_parse(js_val *instance, js_args *args, eval_state *state)
{
  return date_parse_str(TO_STR(ARG(args, 0))->string.ptr);
}

// Date.UTC(year, month[, date[, hours[, minutes[, seconds[, ms]]]]])
js_val *
date_utc(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(ms_from_args(args));
}

// Date.isDST()   Non-standard
js_val *
date_is_dst(js_val *instance, js_args *args, eval_state *state)
{
  time_t now = time(NULL);
  struct tm *loc_tm = localtime(&now);
  return JSBOOL(loc_tm->tm_isdst);
}

// Date.prototype.getDate()
js_val *
date_proto_get_date(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(date_from_time(local_time(instance->number.val)));
}

// Date.prototype.getDay()
js_val *
date_proto_get_day(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(week_day(local_time(instance->number.val)));
}

// Date.prototype.getFullYear()
js_val *
date_proto_get_full_year(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(year_from_time(local_time(instance->number.val)));
}

// Date.prototype.getHours()
js_val *
date_proto_get_hours(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(hour_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMilliseconds()
js_val *
date_proto_get_milliseconds(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(ms_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMinutes()
js_val *
date_proto_get_minutes(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(min_from_time(local_time(instance->number.val)));
}

// Date.prototype.getMonth()
js_val *
date_proto_get_month(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(month_from_time(local_time(instance->number.val)));
}

// Date.prototype.getSeconds()
js_val *
date_proto_get_seconds(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(sec_from_time(local_time(instance->number.val)));
}

// Date.prototype.getTime()
js_val *
date_proto_get_time(js_val *instance, js_args *args, eval_state *state)
{
  return instance;
}

// Date.prototype.getTimezoneOffset()
js_val *
date_proto_get_timezone_offset(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  return JSNUM((t - local_time(t)) / ms_per_min);
}

// Date.prototype.getUTCDate()
js_val *
date_proto_get_utc_date(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(date_from_time(instance->number.val));
}

// Date.prototype.getUTCDay()
js_val *
date_proto_get_utc_day(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(week_day(instance->number.val));
}

// Date.prototype.getUTCFullYear()
js_val *
date_proto_get_utc_full_year(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(year_from_time(instance->number.val));
}

// Date.prototype.getUTCHours()
js_val *
date_proto_get_utc_hours(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(hour_from_time(instance->number.val));
}

// Date.prototype.getUTCMilliseconds()
js_val *
date_proto_get_utc_milliseconds(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(ms_from_time(instance->number.val));
}

// Date.prototype.getUTCMinutes()
js_val *
date_proto_get_utc_minutes(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(min_from_time(instance->number.val));
}

// Date.prototype.getUTCMonth()
js_val *
date_proto_get_utc_month(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(month_from_time(instance->number.val));
}

// Date.prototype.getUTCSeconds()
js_val *
date_proto_get_utc_seconds(js_val *instance, js_args *args, eval_state *state)
{
  return JSNUM(sec_from_time(instance->number.val));
}

// Date.prototype.getYear()
js_val *
date_proto_get_year(js_val *instance, js_args *args, eval_state *state)
{
  int y = year_from_time(instance->number.val);
  return JSNUM(y - 1900);
}

// Date.prototype.setDate(dayValue)
js_val *
date_proto_set_date(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 2, 1)));
  return instance;
}

// Date.prototype.setFullYear(yearValue[, monthValue[, dayValue]])
js_val *
date_proto_set_full_year(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 0, 3)));
  return instance;
}

// Date.prototype.setHours(hourValue[, minutesValue[, secondsValue[, msValue]]])
js_val *
date_proto_set_hours(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 3, 4)));
  return instance;
}

// Date.prototype.setMilliseconds(msValue)
js_val *
date_proto_set_milliseconds(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 6, 1)));
  return instance;
}

// Date.prototype.setMinutes(minutesValue[, secondsValue[, msValue]])
js_val *
date_proto_set_minutes(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 4, 3)));
  return instance;
}

// Date.prototype.setMonth(monthValue[, dayValue])
js_val *
date_proto_set_month(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 1, 2)));
  return instance;
}

// Date.prototype.setSeconds(secondsValue[, msValue])
js_val *
date_proto_set_seconds(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);
  instance->number.val = utc_time(time_clip(make_date_from_args(args, t, 5, 2)));
  return instance;
}

// Date.prototype.setTime(timeValue)
js_val *
date_proto_set_time(js_val *instance, js_args *args, eval_state *state)
{
  instance->number.val = time_clip(TO_NUM(ARG(args, 0))->number.val);
  return instance;
}

// Date.prototype.setUTCDate(dayValue)
js_val *
date_proto_set_utc_date(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 2, 1));
  return instance;
}

// Date.prototype.setUTCFullYear(yearValue[, monthValue[, dayValue]])
js_val *
date_proto_set_utc_full_year(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 0, 3));
  return instance;
}

// Date.prototype.setUTCHours(hoursValue[, minutesValue[, secondsValue[, msValue]]])
js_val *
date_proto_set_utc_hours(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 3, 4));
  return instance;
}

// Date.prototype.setUTCMilliseconds(msValue)
js_val *
date_proto_set_utc_milliseconds(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 6, 1));
  return instance;
}

// Date.prototype.setUTCMinutes(minutesValue[, secondsValue[, msValue]])
js_val *
date_proto_set_utc_minutes(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 4, 3));
  return instance;
}

// Date.prototype.setUTCMonth(monthValue[, dayValue])
js_val *
date_proto_set_utc_month(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 1, 2));
  return instance;
}

// Date.prototype.setUTCSeconds(secondsValue[, msValue])
js_val *
date_proto_set_utc_seconds(js_val *instance, js_args *args, eval_state *state)
{
  double t = instance->number.val;
  instance->number.val = time_clip(make_date_from_args(args, t, 5, 2));
  return instance;
}

// Date.prototype.setYear(yearValue)
js_val *
date_proto_set_year(js_val *instance, js_args *args, eval_state *state)
{
  double t = local_time(instance->number.val);

  // Adjust the year argument and put it back
  int y = TO_NUM(ARG(args, 0))->number.val;
  if (y >= 0 && y <= 99)
    y += 1900;
  js_args *new_args = fh_new_args(JSNUM(y), 0, 0);

  // Same procedure as setFullYear, but with no additional parameters
  instance->number.val = utc_time(time_clip(make_date_from_args(new_args, t, 0, 1)));
  return instance;
}

// Date.prototype.toDateString()
js_val *
date_proto_to_date_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_format_loc(instance->number.val, true, false);
}

// Date.prototype.toISOString()
js_val *
date_proto_to_iso_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_format_iso(instance->number.val);
}

// Date.prototype.toLocaleDateString()
js_val *
date_proto_to_locale_date_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_proto_to_date_string(instance, args, state);
}

// Date.prototype.toLocaleString()
js_val *
date_proto_to_locale_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_proto_to_string(instance, args, state);
}

// Date.prototype.toLocaleTimeString()
js_val *
date_proto_to_locale_time_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_proto_to_time_string(instance, args, state);
}

// Date.prototype.toString()
js_val *
date_proto_to_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_format_loc(instance->number.val, true, true);
}

// Date.prototype.toTimeString()
js_val *
date_proto_to_time_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_format_loc(instance->number.val, false, true);
}

// Date.prototype.toUTCString()
js_val *
date_proto_to_utc_string(js_val *instance, js_args *args, eval_state *state)
{
  return date_format_utc(instance->number.val);
}

// Date.prototype.valueOf()
js_val *
date_proto_value_of(js_val *instance, js_args *args, eval_state *state)
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
    default: return d - (31+30+31+30+31+31+30+31+30) + 1;
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
time_within_day(double t)
{
  double res = fmod(t, ms_per_day);
  if (res < 0)
    res += ms_per_day;
  return res;
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
double
ms_from_args(js_args *args)
{
  js_val *year    = ARG(args, 0),
          *month   = ARG(args, 1),
          *date    = ARG(args, 2),
          *hours   = ARG(args, 3),
          *minutes = ARG(args, 4),
          *seconds = ARG(args, 5),
          *ms      = ARG(args, 6);

  js_val *y   = TO_NUM(year),
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

  double day = make_day(yi, m->number.val, dt->number.val);
  double time = make_time(h->number.val, min->number.val, s->number.val, mil->number.val);
  return time_clip(make_date(day, time));
}

js_val *
date_format_loc(double ut, bool incl_date, bool incl_time)
{
  double t = local_time(ut);
  js_val *res = JSSTR("");
  char *fmt;

  if (incl_date) {
    int m = month_from_time(t);
    int y = year_from_time(t);
    int d = week_day(t);
    int dt = date_from_time(t);

    // e.g. Wed Oct 12 1984
    fmt = "%s %s %02d %d";

    size_t size = snprintf(NULL, 0, fmt, day_string(d), month_string(m), dt, y) + 1;
    char *date = malloc(size);
    snprintf(date, size, fmt, day_string(d), month_string(m), dt, y);

    res->string.ptr = fh_str_concat(res->string.ptr, date);
  }

  if (incl_date && incl_time)
    res->string.ptr = fh_str_concat(res->string.ptr, " ");

  if (incl_time) {
    int h = hour_from_time(t);
    int m = min_from_time(t);
    int s = sec_from_time(t);
    int offset = utc_offset() + dst_offset(t);
    int offset_h = hour_from_time(offset);
    int offset_m = min_from_time(offset);
    char sign = offset > 0 ? '-' : '+';
    const char *tz = tz_string(ut);

    // e.g. 12:31:19 GMT-0400 (EDT)
    fmt = "%02d:%02d:%02d GMT%c%02d%02d (%s)";

    size_t size = snprintf(NULL, 0, fmt, h, m, s, sign, offset_h, offset_m, tz) + 1;
    char *time = malloc(size);
    snprintf(time, size, fmt, h, m, s, sign, offset_h, offset_m, tz);

    res->string.ptr = fh_str_concat(res->string.ptr, time);
  }
  return res;
}

js_val *
date_format_utc(double t)
{
  int m = month_from_time(t);
  int y = year_from_time(t);
  int d = week_day(t);
  int dt = date_from_time(t);
  int h = hour_from_time(t);
  int mn = min_from_time(t);
  int s = sec_from_time(t);

  // e.g. Mon, 03 Jul 2001 23:21:48 GMT
  char *fmt = "%s, %02d %s %d %02d:%02d:%02d GMT";

  size_t size = snprintf(NULL, 0, fmt, day_string(d), 
                         dt, month_string(m), y, h, mn, s) + 1;
  char *str = malloc(size);
  snprintf(str, size, fmt, day_string(d), dt, month_string(m), y, h, mn, s);

  return JSSTR(str);
}

js_val *
date_format_iso(double t)
{
  int m = month_from_time(t) + 1;
  int y = year_from_time(t);
  int dt = date_from_time(t);
  int h = hour_from_time(t);
  int mn = min_from_time(t);
  int s = sec_from_time(t);
  int ms = ms_from_time(t);

  // e.g. 2011-10-05T14:48:00.000Z
  char *fmt = "%d-%02d-%02dT%02d:%02d:%02d.%03dZ";

  size_t size = snprintf(NULL, 0, fmt, y, m, dt, h, mn, s, ms) + 1;
  char *str = malloc(size);
  snprintf(str, size, fmt, y, m, dt, h, mn, s, ms);

  return JSSTR(str);
}

bool
date_verify_format(char *str, char *fmt)
{
  if (strlen(str) != strlen(fmt)) 
    return false;

  // Within the format string:
  //  a - any alphabetic character
  //  d - any digit character
  //
  // Everything else is taken literally.
  
  int i, n;
  for (i = 0, n = strlen(str); i < n; i++) {
    char c = str[i], f = fmt[i];

    if (f == 'a' && !isalpha(c)) 
      return false;
    if (f == 'd' && !isdigit(c)) 
      return false;
    if (f != 'a' && f != 'd' && c != f)
      return false;
  }

  return true;
}

js_val *
date_parse_str(char *str)
{
  double t;

  // Rely on short-circuiting here. We'll take the first one that parses.
  // ISO parse is least forgiving, so it goes first. 
  if (date_parse_iso(str, &t) || date_parse_utc(str, &t) || date_parse_loc(str, &t))
    return JSNUM(t);
  return JSNAN();
}

// The date_parse_x functions take a date string and a double pointer. They
// return true when a date (and time) can be parsed from the given string, and
// false otherwise. When true, the parsed UTC time in milliseconds is stored in
// the double.

bool
date_parse_loc(char *str, double *t)
{
  // FIXME: Incorrect assumptions being made:
  //
  // - Assumes TZ abbreviation is always 3 letters (not true!)
  // - Assumes this is a local time (could be another TZ)
  
  // Wed Oct 12 1984 12:31:19 GMT-0400 (EDT)
  char *fmt = "aaa aaa dd dddd dd:dd:dd GMT-dddd (aaa)";

  // Verify the format
  if (!date_verify_format(str, fmt))
    return false;

  char *mname = fh_str_slice(str, 4, 7);
  int date = atoi(fh_str_slice(str, 8, 10));
  int year = atoi(fh_str_slice(str, 11, 15));
  int hour = atoi(fh_str_slice(str, 16, 18));
  int min = atoi(fh_str_slice(str, 19, 21));
  int sec = atoi(fh_str_slice(str, 22, 24));

  // Get the month as an integer
  int month = 0;
  while(month < 12) {
    if (STREQ(month_string(month), mname)) break;
    month++;
  };
  // Invalid month
  if (month == 12) return false;

  double day = make_day(year, month, date);
  double time = make_time(hour, min, sec, 0);

  *t = time_clip(utc_time(make_date(day, time)));
  return true;
}

bool
date_parse_utc(char *str, double *t)
{
  // Mon, 03 Jul 2006 21:44:38 GMT
  char *fmt = "aaa, dd aaa dddd dd:dd:dd GMT";

  // Verify the format
  if (!date_verify_format(str, fmt))
    return false;

  // Pull out the parts
  int date = atoi(fh_str_slice(str, 5, 7));
  char *mname = fh_str_slice(str, 8, 11);
  int year = atoi(fh_str_slice(str, 12, 16));
  int hour = atoi(fh_str_slice(str, 17, 19));
  int min = atoi(fh_str_slice(str, 20, 22));
  int sec = atoi(fh_str_slice(str, 23, 25));

  int month = 0;
  while(month < 12) {
    if (STREQ(month_string(month), mname)) break;
    month++;
  };
  if (month == 12) return false;

  double day = make_day(year, month, date);
  double time = make_time(hour, min, sec, 0);

  *t = time_clip(make_date(day, time));
  return true;
}

bool
date_parse_iso(char *str, double *t)
{
  char *fmt = "dddd-dd-ddTdd:dd:dd.dddZ";

  // Verify the format
  if (!date_verify_format(str, fmt))
    return false;

  // Pull out the parts
  int year = atoi(fh_str_slice(str, 0, 4));
  int month = atoi(fh_str_slice(str, 5, 7)) - 1;
  int date = atoi(fh_str_slice(str, 8, 10));
  int hour = atoi(fh_str_slice(str, 11, 13));
  int min = atoi(fh_str_slice(str, 14, 16));
  int sec = atoi(fh_str_slice(str, 17, 19));
  int msec = atoi(fh_str_slice(str, 20, 23));

  double day = make_day(year, month, date);
  double time = make_time(hour, min, sec, msec);

  *t = time_clip(make_date(day, time));
  return true;
}


#if defined(_BSD_SOURCE)

const char *
tz_string(double t)
{
  time_t time = t / 1000;
  struct tm *loc_tm = localtime(&time);
  return loc_tm->tm_zone;
}

#else

const char *
tz_string(double t)
{
  static char buf[16];
  time_t time = t / 1000;
  strftime(buf, sizeof buf, "%Z", localtime(&time));
  buf[sizeof buf - 1] = '\0';
  return buf;
}

#endif


char *
day_string(int d) {
  switch(d) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    default: return "Sat";
  }
}

char *
month_string(int m) {
  switch(m) {
    case 0: return "Jan";
    case 1: return "Feb";
    case 2: return "Mar";
    case 3: return "Apr";
    case 4: return "May";
    case 5: return "Jun";
    case 6: return "Jul";
    case 7: return "Aug";
    case 8: return "Sep";
    case 9: return "Oct";
    case 10: return "Nov";
    default: return "Dec";
  }
}

double
make_time(double h, double m, double s, double ms)
{
  return ((h * min_per_hr + m) * sec_per_min + s) * ms_per_sec + ms;
}

double
make_day(double y, double m, double dt)
{
  m = fmod(m, 12);
  y += floor(m / 12);
  if (m < 0) m += 12;

  double yd = floor(time_from_year(y) / ms_per_day);
  double md = day_from_month(m, y);

  return yd + md + dt - 1;
}

double
make_date(double d, double t)
{
  return d * ms_per_day + t;
}

double
make_part(double t, int shift)
{
  switch(shift) {
    case 0: return year_from_time(t);
    case 1: return month_from_time(t);
    case 2: return date_from_time(t);
    case 3: return hour_from_time(t);
    case 4: return min_from_time(t);
    case 5: return sec_from_time(t);
    default: return ms_from_time(t);
  }
}

// This is intended to DRY up the setX and setUTCX method implementations.
// Each of the methods takes as parameters some subset of:
//
//   { year, month, date, hour, minute, seconds, milliseconds }
//
// always in order, and always requiring the first argument. An array of date
// parts is built using the shift from the start of the full set above and the
// maximum number of allowed args. When a date part is missing (and not
// required) or not part of the subset, the part is calculated using the UTC
// time.
double
make_date_from_args(js_args *args, double t, int shift, int max_args)
{
  double parts[7];
  int i, j = 0;
  js_val *arg;

  for (i = 0; i < 7; i++) {
    if (shift <= i && (i - shift) < max_args) {
      arg = ARG(args, j++);
      if (!IS_UNDEF(arg) || j == 1) {
        parts[i] = TO_NUM(arg)->number.val;
        continue;
      }
    }
    parts[i] = make_part(t, i);
  }

  double day = make_day(parts[0], parts[1], parts[2]);
  double time = make_time(parts[3], parts[4], parts[5], parts[6]);
  return make_date(day, time);
}

double
time_clip(double t)
{
  if (abs(t) > 8.64e15)
    return 0;
  return t;
}

js_val *
bootstrap_date()
{
  js_val *date = JSNFUNC(date_new, 7);
  js_val *proto = JSOBJ();
  proto->proto = fh->object_proto;

  // Date
  // ----

  // Properties
  DEF(date, "prototype", proto);

  // Methods
  DEF(date, "now", JSNFUNC(date_now, 0));
  DEF(date, "parse", JSNFUNC(date_parse, 1));
  DEF(date, "UTC", JSNFUNC(date_utc, 7));
  DEF(date, "isDST", JSNFUNC(date_is_dst, 0));

  // Date.prototype
  // --------------
  
  // Properties
  DEF(proto, "constructor", JSNFUNC(date_new, 7));
  
  // Methods
  DEF(proto, "getDate", JSNFUNC(date_proto_get_date, 0));
  DEF(proto, "getDay", JSNFUNC(date_proto_get_day, 0));
  DEF(proto, "getHours", JSNFUNC(date_proto_get_hours, 0));
  DEF(proto, "getFullYear", JSNFUNC(date_proto_get_full_year, 0));
  DEF(proto, "getMilliseconds", JSNFUNC(date_proto_get_milliseconds, 0));
  DEF(proto, "getMinutes", JSNFUNC(date_proto_get_minutes, 0));
  DEF(proto, "getMonth", JSNFUNC(date_proto_get_month, 0));
  DEF(proto, "getSeconds", JSNFUNC(date_proto_get_seconds, 0));
  DEF(proto, "getTime", JSNFUNC(date_proto_get_time, 0));
  DEF(proto, "getTimezoneOffset", JSNFUNC(date_proto_get_timezone_offset, 0));
  DEF(proto, "getUTCDate", JSNFUNC(date_proto_get_utc_date, 0));
  DEF(proto, "getUTCDay", JSNFUNC(date_proto_get_utc_day, 0));
  DEF(proto, "getUTCHours", JSNFUNC(date_proto_get_utc_hours, 0));
  DEF(proto, "getUTCFullYear", JSNFUNC(date_proto_get_utc_full_year, 0));
  DEF(proto, "getUTCMilliseconds", JSNFUNC(date_proto_get_utc_milliseconds, 0));
  DEF(proto, "getUTCMinutes", JSNFUNC(date_proto_get_utc_minutes, 0));
  DEF(proto, "getUTCMonth", JSNFUNC(date_proto_get_utc_month, 0));
  DEF(proto, "getUTCSeconds", JSNFUNC(date_proto_get_utc_seconds, 0));
  DEF(proto, "getYear", JSNFUNC(date_proto_get_year, 0));

  DEF(proto, "setDate", JSNFUNC(date_proto_set_date, 1));
  DEF(proto, "setFullYear", JSNFUNC(date_proto_set_full_year, 3));
  DEF(proto, "setHours", JSNFUNC(date_proto_set_hours, 4));
  DEF(proto, "setMilliseconds", JSNFUNC(date_proto_set_milliseconds, 1));
  DEF(proto, "setMinutes", JSNFUNC(date_proto_set_minutes, 3));
  DEF(proto, "setMonth", JSNFUNC(date_proto_set_month, 2));
  DEF(proto, "setSeconds", JSNFUNC(date_proto_set_seconds, 2));
  DEF(proto, "setTime", JSNFUNC(date_proto_set_time, 1));
  DEF(proto, "setUTCDate", JSNFUNC(date_proto_set_utc_date, 1));
  DEF(proto, "setUTCFullYear", JSNFUNC(date_proto_set_utc_full_year, 3));
  DEF(proto, "setUTCHours", JSNFUNC(date_proto_set_utc_hours, 4));
  DEF(proto, "setUTCMilliseconds", JSNFUNC(date_proto_set_utc_milliseconds, 1));
  DEF(proto, "setUTCMinutes", JSNFUNC(date_proto_set_utc_minutes, 3));
  DEF(proto, "setUTCMonth", JSNFUNC(date_proto_set_utc_month, 2));
  DEF(proto, "setUTCSeconds", JSNFUNC(date_proto_set_utc_seconds, 2));
  DEF(proto, "setYear", JSNFUNC(date_proto_set_year, 1));

  DEF(proto, "toDateString", JSNFUNC(date_proto_to_date_string, 0));
  DEF(proto, "toGMTString", JSNFUNC(date_proto_to_utc_string, 0));
  DEF(proto, "toISOString", JSNFUNC(date_proto_to_iso_string, 0));
  DEF(proto, "toJSON", JSNFUNC(date_proto_to_iso_string, 1));
  DEF(proto, "toLocaleDateString", JSNFUNC(date_proto_to_locale_date_string, 0));
  DEF(proto, "toLocaleString", JSNFUNC(date_proto_to_locale_string, 0));
  DEF(proto, "toLocaleTimeString", JSNFUNC(date_proto_to_locale_time_string, 0));
  DEF(proto, "toString", JSNFUNC(date_proto_to_string, 0));
  DEF(proto, "toTimeString", JSNFUNC(date_proto_to_time_string, 0));
  DEF(proto, "toUTCString", JSNFUNC(date_proto_to_utc_string, 0));
  DEF(proto, "valueOf", JSNFUNC(date_proto_value_of, 0));

  fh_attach_prototype(proto, fh->function_proto);

  return date;
}
