typedef struct {
	int ex_cost;
    int result_latency;
    bool multi; // is to let know if the size of reglist is to be considered
	// bool dep;
	bool unknown;
} m7f_time_t;
static int P = 6; // The number of cycles required for a pipeline refill. 
static int s = 32; // s=number of significant bits

m7f_time_t M7F_time_alo_without_pc = {1, 2 + 5, false, false};
m7f_time_t M7F_time_alo_with_pc = {1 + P, 1 + P + 5, false, false};
m7f_time_t M7F_time_others_1c = {1, 1 + 5, false, false};
m7f_time_t M7F_time_others_2c = {2, 3 + 5, false, false};
m7f_time_t M7F_time_ld_st = {1, 1 + 5, true, false};
m7f_time_t M7F_time_branch = {1, 1 + 5, false, false};
m7f_time_t M7F_time_div = {4 + s/2, 4 + s/2 + 5, false, false};
m7f_time_t M7F_time_dsp_inst = {4, 4 + 2 + 5, false, false};
m7f_time_t M7F_time_others_sp = {1, 3 + 5, false, false};
m7f_time_t M7F_time_mul_sp = {1, 3 + 5, false, false};
m7f_time_t M7F_time_vdiv_sp = {1, 3 + 5, false, false};
m7f_time_t M7F_time_others_dp = {10, 10 + 5, false, false};
m7f_time_t M7F_time_vldm = {1 + 2*16, 1 + 2*16 + 5, false, false};
m7f_time_t M7F_time_vldr = {3, 3 + 5, false, false};
m7f_time_t M7F_time_vmov = {2, 2 + 5, false, false};
m7f_time_t M7F_time_vpop = {1+16, 1+16 + 5, false, false};
m7f_time_t M7F_time_vpush = {1+2*16, 1+2*16 + 5, false, false};
m7f_time_t M7F_time_vsqrt_32 = {14, 16 + 5, false, false};
m7f_time_t M7F_time_vstm = {1+2*16, 1+2*16 + 5, false, false};
m7f_time_t M7F_time_vstr = {3, 3 + 5, false, false};
m7f_time_t M7F_time_unknown = {25, 25, true, true};

#include "armCortexM7F_time.h"
