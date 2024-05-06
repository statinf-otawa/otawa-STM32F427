/* Generated by gliss-attr ($(date)) copyright (c) 2009 IRIT - UPS */

#include <$(proc)/api.h>
#include <$(proc)/id.h>
#include <$(proc)/macros.h>
#include <$(proc)/grt.h>

#ifdef __cplusplus
extern "C" {
#endif

m7f_time_t *M7F_time_return;
typedef void (*fun_t)($(proc)_inst_t *inst);
#define SET_TIME(x)	M7F_time_return = &x

/*** function definition ***/

void stm32_m7f_time_UNKNOWN($(proc)_inst_t *inst) {
	SET_TIME(M7F_time_unknown);
}

$(foreach instructions)
void stm32_m7f_time_$(IDENT)($(proc)_inst_t *inst) {
$(cortexM7F_time)
};
$(end)


/*** function table ***/
static fun_t time_funs[] = {
	stm32_m7f_time_UNKNOWN$(foreach instructions),
	stm32_m7f_time_$(IDENT)$(end)
};

/**
 * Get the stm32_m7f timing.
 * @return stm32_m7f timing.
 */
m7f_time_t *stm32M7F(void *_inst) {
	arm_inst_t *inst = static_cast<arm_inst_t *>(_inst);
	time_funs[inst->ident](inst);
	return M7F_time_return;
}

#ifdef __cplusplus
}
#endif
