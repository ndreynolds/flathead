// gc.c
// ---------
// Non-standard utility functions for running and debugging garbage collection
//
// The `gc` object is available when flathead is compiled with GC exposed, which is
// currently the default setting. If access to the garbage collector is not desirable, these utilities can be disabled by compiling with:
//
//  make gcexpose=off
//
// As objects are instantiated by the interpreter to call a function, it's
// possible that calling one of these methods may trigger the GC.

#include "gc.h"

// gc.run()
//
// Runs garbage collection immediately and returns undefined.
js_val *
gc_run(js_val *instance, js_args *args, eval_state *state)
{
  fh_gc();
  return JSUNDEF();
}

// gc.info()
//
// Returns information about the last garbage collection.
js_val *
gc_info(js_val *instance, js_args *args, eval_state *state)
{
  js_val *info = JSOBJ();
  fh_set_prop(info, "arenas", JSNUM(fh->gc_num_arenas), P_DEFAULT);
  fh_set_prop(info, "arenaSize", JSNUM(SLOTS_PER_ARENA), P_DEFAULT);
  fh_set_prop(info, "runs", JSNUM(fh->gc_runs), P_DEFAULT);
  fh_set_prop(info, "lastStart", JSNUM(fh->gc_last_start), P_DEFAULT);
  fh_set_prop(info, "time", JSNUM(fh->gc_time), P_DEFAULT);
  return info;
}

// gc.spy(value)
//
// Flags a value for verbose GC logging. Because there's so much noise from
// allocating objects for the standard runtime, it can be difficult to follow
// the lifecycle of an individual value.
js_val *
gc_spy(js_val *instance, js_args *args, eval_state *state)
{
  if (ARGLEN(args) >= 1) {
    js_val *val = ARG(args, 0);
    val->flagged = true;
  }
  return JSUNDEF();
}

js_val *
bootstrap_gc()
{
  js_val *gc = JSOBJ();

  DEF(gc, "run", JSNFUNC(gc_run, 0));
  DEF(gc, "info", JSNFUNC(gc_info, 0));
  DEF(gc, "spy", JSNFUNC(gc_spy, 1));

  fh_attach_prototype(gc, fh->function_proto);

  return gc;
}
