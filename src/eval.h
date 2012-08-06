// eval.h
// ------

#include "nodes.h"

int jl_eval(JLVALUE *, JLNode *);
void jl_while(JLVALUE *, JLNode *, JLNode *);
JLARGS * jl_build_args(JLVALUE *, JLNode *);
JLVALUE * jl_obj(JLNode *);
JLVALUE * jl_get(JLVALUE *, char *);
JLVALUE * jl_call(JLVALUE *, JLNode *);
JLVALUE * jl_function_call(JLVALUE *, JLVALUE *, JLNode *);
JLVALUE * jl_member(JLVALUE *, JLNode *);
JLVALUE * jl_eval_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_postfix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_prefix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_bin_exp(JLVALUE *, JLNode *);
JLVALUE * jl_add(JLVALUE *, JLVALUE *);
JLVALUE * jl_sub(JLVALUE *, JLVALUE *);
JLVALUE * jl_mul(JLVALUE *, JLVALUE *);
JLVALUE * jl_div(JLVALUE *, JLVALUE *);
JLVALUE * jl_mod(JLVALUE *, JLVALUE *);
