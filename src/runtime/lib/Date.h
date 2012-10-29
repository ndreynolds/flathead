// Date.h
// --------

#ifndef JS_DATE_H
#define JS_DATE_H

#include "../runtime.h"

JSValue * date_new(JSValue *, JSArgs *, State *);
JSValue * date_now(JSValue *, JSArgs *, State *);
JSValue * date_parse(JSValue *, JSArgs *, State *);
JSValue * date_utc(JSValue *, JSArgs *, State *);

JSValue * date_proto_get_date(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_day(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_full_year(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_hours(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_milliseconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_minutes(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_month(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_seconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_time(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_timezone_offset(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_date(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_day(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_hours(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_milliseconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_minutes(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_month(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_utc_seconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_get_year(JSValue *, JSArgs *, State *);

JSValue * date_proto_set_date(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_full_year(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_hours(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_milliseconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_minutes(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_month(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_seconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_time(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_date(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_full_year(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_hours(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_milliseconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_minutes(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_month(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_utc_seconds(JSValue *, JSArgs *, State *);
JSValue * date_proto_set_year(JSValue *, JSArgs *, State *);

JSValue * date_proto_to_date_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_iso_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_json(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_gmt_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_locale_date_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_locale_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_locale_time_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_time_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_to_utc_string(JSValue *, JSArgs *, State *);
JSValue * date_proto_value_of(JSValue *, JSArgs *, State *);

double time_from_year(double);
double days_in_year(double);
double day_from_year(double);
double date_from_time(double);
double day(double);
long week_day(double);
double day_from_month(long, long);
double day_within_year(double);
long month_from_time(double);
double year_from_time(double);
double in_leap_year(double);
double local_time(double);
double utc_time(void);
double utc_offset(void);
long hour_from_time(double);
long min_from_time(double);
long sec_from_time(double);
long ms_from_time(double);

JSValue * utc_from_args(JSArgs *);
JSValue * make_time(JSValue *, JSValue *, JSValue *, JSValue *);
JSValue * make_day(JSValue *, JSValue *, JSValue *);
JSValue * make_date(JSValue *, JSValue *);
JSValue * time_clip(JSValue *);

JSValue * bootstrap_date(void);

#endif