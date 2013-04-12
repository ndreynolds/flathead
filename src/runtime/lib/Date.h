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

double utc_now(void);

js_val * bootstrap_date(void);

#endif
