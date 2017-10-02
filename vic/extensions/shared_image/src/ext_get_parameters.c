#include <ext_driver_shared_image.h>

void
validate_ext_parameters(){
    extern ext_parameters_struct ext_param;
    
    // Validate extension parameters
    if(!(ext_param.MPI_E_PROCESS_COST >= 0)) {
        log_err("MPI_E_PROCESS_COST must be defined on the interval [0, inf)");
    }
    if(!(ext_param.MPI_N_PROCESS_COST >= 0)) {
        log_err("MPI_N_PROCESS_COST must be defined on the interval [0, inf)");
    }
    if(!(ext_param.UH_FLOW_DIFFUSION >= 0)) {
        log_err("UH_FLOW_DIFFUSION must be defined on the interval [0, inf)");
    }
    if(!(ext_param.UH_FLOW_VELOCITY >= 0)) {
        log_err("UH_FLOW_VELOCITY must be defined on the interval [0, inf)");
    }
    if(!(ext_param.UH_PARTITIONS >= 0)) {
        log_err("UH_FLOW_VELOCITY must be defined on the interval [0, inf)");
    }
    if(!(ext_param.UH_MAX_LENGTH >= 0)) {
        log_err("UH_FLOW_VELOCITY must be defined on the interval [0, inf)");
    }
}