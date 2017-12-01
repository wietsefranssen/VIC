#include "ext_driver_shared_image.h"

/******************************************************************************
 * @brief    Initialize local structures
 *****************************************************************************/
void
initialize_ext_local_structures(void)
{  
    extern ext_option_struct ext_options;
    
    if(ext_options.GROUNDWATER){
        initialize_gw_local_structures();
    }
}

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_options(ext_option_struct *options)
{    
    initialize_gw_options(options);
}

/******************************************************************************
 * @brief    Initialize all filenames before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_filenames(ext_filenames_struct *filenames)
{    
    initialize_gw_filenames(filenames);
    
    initialize_ext_info(&filenames->info);
}

/******************************************************************************
 * @brief    Initialize all info before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_info(ext_info_struct *info){    
    initialize_gw_info(info);
}

/******************************************************************************
 * @brief    Initialize all parameters before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_ext_parameters(ext_parameters_struct *parameters)
{          
    
}

/******************************************************************************
 * @brief    Initialize global structures
 *****************************************************************************/
void
initialize_ext_global_structures(void)
{  
    extern ext_filenames_struct ext_filenames;
    extern ext_option_struct ext_options;
    extern ext_parameters_struct ext_param;
    extern int mpi_rank;
    
    if(mpi_rank == VIC_MPI_ROOT){    
        initialize_ext_options(&ext_options);
        initialize_ext_filenames(&ext_filenames);
        initialize_ext_parameters(&ext_param);
    }
}

/******************************************************************************
 * @brief    Initialize mpi structures
 *****************************************************************************/
void
initialize_ext_mpi()
{    
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    
    create_MPI_ext_option_struct_type(&mpi_ext_option_struct_type);
    create_MPI_ext_parameters_struct_type(&mpi_ext_param_struct_type);
}

/******************************************************************************
 * @brief    Initialize a dmy
 *****************************************************************************/
void
initialize_dmy(dmy_struct *dmy)
{           
    dmy->day = 0;
    dmy->day_in_year = 0;
    dmy->dayseconds = 0;
    dmy->month = 0;
    dmy->year = 0;
}

/******************************************************************************
 * @brief    Initialize a nameid
 *****************************************************************************/
void
initialize_nameid(nameid_struct *nameid){
    strcpy(nameid->nc_filename, MISSING_S);
    nameid->nc_id = MISSING;
}