// Date.h
// --------

#ifndef JS_DATE_H
#define JS_DATE_H

#include "../runtime.h"

js_val * date_new(js_val *, js_args *, eval_state *);
js_val * date_now(js_val *, js_args *, eval_state *);
js_val * date_parse(js_val *, js_args *, eval_state *);
js_val * date_utc(js_val *, js_args *, eval_state *);

js_val * date_proto_get_date(js_val *, js_args *, eval_state *);
js_val * date_proto_get_day(js_val *, js_args *, eval_state *);
js_val * date_proto_get_full_year(js_val *, js_args *, eval_state *);
js_val * date_proto_get_hours(js_val *, js_args *, eval_state *);
js_val * date_proto_get_milliseconds(js_val *, js_args *, eval_state *);
js_val * date_proto_get_minutes(js_val *, js_args *, eval_state *);
js_val * date_proto_get_month(js_val *, js_args *, eval_state *);
js_val * date_proto_get_seconds(js_val *, js_args *, eval_state *);
js_val * date_proto_get_time(js_val *, js_args *, eval_state *);
js_val * date_proto_get_timezone_offset(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_date(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_day(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_full_year(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_hours(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_milliseconds(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_minutes(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_month(js_val *, js_args *, eval_state *);
js_val * date_proto_get_utc_seconds(js_val *, js_args *, eval_state *);
js_val * date_proto_get_year(js_val *, js_args *, eval_state *);

js_val * date_proto_set_date(js_val *, js_args *, eval_state *);
js_val * date_proto_set_full_year(js_val *, js_args *, eval_state *);
js_val * date_proto_set_hours(js_val *, js_args *, eval_state *);
js_val * date_proto_set_milliseconds(js_val *, js_args *, eval_state *);
js_val * date_proto_set_minutes(js_val *, js_args *, eval_state *);
js_val * date_proto_set_month(js_val *, js_args *, eval_state *);
js_val * date_proto_set_seconds(js_val *, js_args *, eval_state *);
js_val * date_proto_set_time(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_date(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_full_year(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_hours(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_milliseconds(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_minutes(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_month(js_val *, js_args *, eval_state *);
js_val * date_proto_set_utc_seconds(js_val *, js_args *, eval_state *);
js_val * date_proto_set_year(js_val *, js_args *, eval_state *);

js_val * date_proto_to_date_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_iso_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_json(js_val *, js_args *, eval_state *);
js_val * date_proto_to_gmt_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_locale_date_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_locale_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_locale_time_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_time_string(js_val *, js_args *, eval_state *);
js_val * date_proto_to_utc_string(js_val *, js_args *, eval_state *);
js_val * date_proto_value_of(js_val *, js_args *, eval_state *);

double time_from_year(double);
double time_within_day(double);
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
double utc_time(double);
double utc_now(void);
double utc_offset(void);
double dst_offset(double);
long hour_from_time(double);
long min_from_time(double);
long sec_from_time(double);
long ms_from_time(double);
const char * tz_string(double);
char * day_string(int);
char * month_string(int);

js_val * date_format_loc(double, bool, bool);
js_val * date_format_utc(double);
js_val * date_format_iso(double);

js_val * date_parse_str(char *);
bool date_verify_format(char *, char *);
bool date_parse_loc(char *, double *);
bool date_parse_utc(char *, double *);
bool date_parse_iso(char *, double *);

double ms_from_args(js_args *);

double make_time(double, double, double, double);
double make_day(double, double, double);
double make_date(double, double);
double make_part(double, int);
double make_date_from_args(js_args *, double, int, int);
double time_clip(double);

js_val * bootstrap_date(void);

#endif
