// eval.h
// ------

#include "nodes.h"

int jl_eval(JLVALUE *, JLNode *);
void jl_while(JLVALUE *, JLNode *, JLNode *);
JLVALUE * jl_obj(JLNode *);
JLVALUE * jl_eval_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_postfix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_prefix_exp(JLVALUE *, JLNode *);
JLVALUE * jl_eval_bin_exp(JLVALUE *, JLNode *);
JLVALUE * jl_add(JLVALUE *, JLVALUE *);
JLVALUE * jl_sub(JLVALUE *, JLVALUE *);
JLVALUE * jl_mul(JLVALUE *, JLVALUE *);
JLVALUE * jl_div(JLVALUE *, JLVALUE *);
JLVALUE * jl_mod(JLVALUE *, JLVALUE *);
