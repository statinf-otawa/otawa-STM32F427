typedef struct {
	int ex_cost;
    bool multi; // is to let know if the size of reglist is to be considered
	bool dep;
	bool unknown;
} m4f_time_t;
static int P = 3; // The number of cycles required for a pipeline refill. 
                     // This ranges from 1 to 3 depending on the alignment and width of the target instruction, 
                     // and whether the processor manages to speculate the address early. We assume P = 3 
static int B = 10;  // The number of cycles required to perform the barrier operation. 
                       // For DSB and DMB, the minimum number of cycles is zero. 
                       // For ISB, the minimum number of cycles is equivalent to the number required for a pipeline refill.
                       // We assume B = 10 (overestimation) since the trm doesn't provide the max value

m4f_time_t M4F_time_mov_without_pc = {1, false, false, false};
m4f_time_t M4F_time_mov_with_pc    = {1 + P, false, false, false};
m4f_time_t M4F_time_add_without_pc = {1, false, false, false};
m4f_time_t M4F_time_add_with_pc    = {1 + P, false, false, false};
m4f_time_t M4F_time_sub            = {1, false, false, false};
m4f_time_t M4F_time_mul_1c         = {1, false, false, false};
m4f_time_t M4F_time_mul_2c         = {2, false, false, false};
m4f_time_t M4F_time_div            = {12, false, false, false};
m4f_time_t M4F_time_others_1c      = {1, false, false, false};
m4f_time_t M4F_time_ld             = {2, false, false, false};
m4f_time_t M4F_time_ldm            = {1, true, false, false};
m4f_time_t M4F_time_ld_pc          = {2 + P, false, false, false};
m4f_time_t M4F_time_ldm_pc         = {1 + P, true, false, false}; // LDM and STM cannot be pipelined with preceding or following instructions
m4f_time_t M4F_time_stm            = {1, true, true, false}; // nothing can be pipelined after the store
m4f_time_t M4F_time_st             = {2, false, true, false}; // nothing can be pipelined after the store
m4f_time_t M4F_time_strd_ldrd      = {1 + 2, false, true}; // LDRD and STRD cannot be pipelined with preceding or following instructions.
m4f_time_t M4F_time_push           = {1 + 16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_pop_pc         = {1 + P + 16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_pop            = {1+ 16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_semaphore      = {2, false, false, false};
m4f_time_t M4F_time_branch_normal  = {1 + P, false, false, false};
m4f_time_t M4F_time_baht_branch    = {2 + 1 + P, false, false, false};  // TBB and TBH are blocking operations. These are at least two cycles for the load, one
                                          // cycle for the add, and three cycles for the pipeline reload. This means at least six cycles,
                                          // or more if stalled on the load or the fetch.
m4f_time_t M4F_time_msr            = {2, false, false, false};
m4f_time_t M4F_time_barriers       = {2 + B, false, false, false};
m4f_time_t M4F_time_dsp_inst       = {1, false, false, false};
m4f_time_t M4F_time_v_1c           = {1, false, false, false};
m4f_time_t M4F_time_vdiv           = {14, false, false, false};
m4f_time_t M4F_time_vldm           = {1 + 2*16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_vldr           = {3, false, false, false};
m4f_time_t M4F_time_vmov           = {2, false, false, false};
m4f_time_t M4F_time_vmul           = {3, false, false, false};
m4f_time_t M4F_time_vpop           = {1 + 2*16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_vpush          = {1 + 2*16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_vsqrt_32       = {14, false, false, false};
m4f_time_t M4F_time_vstm           = {1 + 2*16, false, false, false}; // actually multi but len(reglist) cannot be defined for now. We consider len(reglist) = max = 16
m4f_time_t M4F_time_vstr           = {3, false, false, false};
m4f_time_t M4F_time_unknown        = {25, true, true, true};
#include "armCortexM4F_time.h"
