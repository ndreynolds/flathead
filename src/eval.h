// eval.h
// ------

#include "nodes.h"

int jl_eval(JLVALUE *, JLNode *);
void jl_while(JLNode *, JLNode *);
JLVALUE * jl_eval_exp(JLNode *);
JLVALUE * jl_eval_postfix_exp(JLNode *);
JLVALUE * jl_eval_prefix_exp(JLNode *);
JLVALUE * jl_eval_bin_exp(JLNode *);
JLVALUE * jl_add(JLVALUE *, JLVALUE *);
JLVALUE * jl_sub(JLVALUE *, JLVALUE *);
JLVALUE * jl_mul(JLVALUE *, JLVALUE *);
JLVALUE * jl_div(JLVALUE *, JLVALUE *);
JLVALUE * jl_mod(JLVALUE *, JLVALUE *);
