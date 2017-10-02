#include <ext_mpi.h>

/******************************************************************************
 * @brief   Decompose the domain for MPI operations
 * @details This function sets up the arrays needed to scatter and gather
 *          data from and to the master process to the individual mpi
 *          processes.
 *
 * @param ncells total number of cells
 * @param mpi_size number of mpi processes
 * @param mpi_map_local_array_sizes address of integer array with number of
 *        cells assigned to each node (MPI_Scatterv:sendcounts and
 *        MPI_Gatherv:recvcounts)
 * @param mpi_map_global_array_offsets address of integer array with offsets
 *        for sending and receiving data (MPI_Scatterv:displs and
 *        MPI_Gatherv:displs)
 * @param mpi_map_mapping_array address of size_t array with indices to prepare
 *        an array on the master process for MPI_Scatterv or map back after
 *        MPI_Gatherv
 *****************************************************************************/
void
mpi_map_decomp_domain(size_t   ncells,
                      size_t   mpi_size,
                      int    **mpi_map_local_array_sizes,
                      int    **mpi_map_global_array_offsets,
                      size_t **mpi_map_mapping_array)
{
    extern ext_parameters_struct ext_param;
    extern ext_filenames_struct ext_filenames;
    extern basin_struct  basins;
    extern int mpi_decomposition;
    
    size_t i;
    size_t node_ids[mpi_size];
    
    if(mpi_decomposition == CALCULATE_DECOMPOSITION ||
            mpi_decomposition == BASIN_DECOMPOSITION){
        
        get_basins(&ext_filenames.routing, ext_filenames.info.direction_var, &basins);
        
        // decompose the mask by basin
        mpi_map_decomp_domain_basin(ncells, mpi_size,
                          mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets,
                          mpi_map_mapping_array,
                          &basins);
        
        if(mpi_decomposition == CALCULATE_DECOMPOSITION){

            for(i=0;i<mpi_size;i++){
                node_ids[i]=i;
            }
            sizet_sort2(node_ids,(*mpi_map_local_array_sizes),mpi_size,false); 
            
            // check if basin decomposition is effective
            if((*mpi_map_local_array_sizes)[node_ids[0]] * 
            ext_param.MPI_N_PROCESS_COST > ncells / mpi_size * 
            ext_param.MPI_E_PROCESS_COST){
                
                // decompose the mask at random
                mpi_map_decomp_domain_random(ncells, mpi_size,
                                  mpi_map_local_array_sizes,
                                  mpi_map_global_array_offsets,
                                  mpi_map_mapping_array);
                
                mpi_decomposition=RANDOM_DECOMPOSITION;
            }else{            
                mpi_decomposition=BASIN_DECOMPOSITION;
                
            }
        }
    }else{
        // decompose the mask at random
        mpi_map_decomp_domain_random(ncells, mpi_size,
                          mpi_map_local_array_sizes,
                          mpi_map_global_array_offsets,
                          mpi_map_mapping_array);
        
        mpi_decomposition=RANDOM_DECOMPOSITION;
    }
    
    if(mpi_decomposition == BASIN_DECOMPOSITION){
        debug("Basin mask decomposition for MPI");
    }else if(mpi_decomposition == RANDOM_DECOMPOSITION){    
        debug("Random mask decomposition for MPI");
    }
}

/******************************************************************************
 * @brief   Decompose the domain for MPI operations
 * @details This function sets up the arrays needed to scatter and gather
 *          data from and to the master process to the individual mpi
 *          processes.
 *
 * @param ncells total number of cells
 * @param mpi_size number of mpi processes
 * @param mpi_map_local_array_sizes address of integer array with number of
 *        cells assigned to each node (MPI_Scatterv:sendcounts and
 *        MPI_Gatherv:recvcounts)
 * @param mpi_map_global_array_offsets address of integer array with offsets
 *        for sending and receiving data (MPI_Scatterv:displs and
 *        MPI_Gatherv:displs)
 * @param mpi_map_mapping_array address of size_t array with indices to prepare
 *        an array on the master process for MPI_Scatterv or map back after
 *        MPI_Gatherv
 *****************************************************************************/
void
mpi_map_decomp_domain_basin(size_t   ncells,
                      size_t   mpi_size,
                      int    **mpi_map_local_array_sizes,
                      int    **mpi_map_global_array_offsets,
                      size_t **mpi_map_mapping_array,
                      basin_struct *basins)
{
    size_t i;
    size_t j;
    size_t k;
    size_t l;
    
    size_t node_ids[mpi_size];
    size_t basin_to_node[basins->Nbasin];
    
    *mpi_map_local_array_sizes = calloc(mpi_size,
                                        sizeof(*(*mpi_map_local_array_sizes)));
    *mpi_map_global_array_offsets = calloc(mpi_size,
                                           sizeof(*(*
                                                    mpi_map_global_array_offsets)));
    *mpi_map_mapping_array = calloc(ncells, sizeof(*(*mpi_map_mapping_array)));
    
    for(i=0;i<mpi_size;i++){
        (*mpi_map_local_array_sizes)[i]=0;
        (*mpi_map_global_array_offsets)[i]=0;
    }
    for(i=0;i<ncells;i++){
        (*mpi_map_mapping_array)[i]=0;
    }
    
    // determine number of cells per node
    for(i=0;i<basins->Nbasin;i++){        
        //sort nodes by size
        for(j=0;j<mpi_size;j++){
            node_ids[j]=j;
        }
        sizet_sort2(node_ids,(*mpi_map_local_array_sizes),mpi_size,true);   
        
        // find node with lowest amount of cells and add the biggest basin
        (*mpi_map_local_array_sizes)[node_ids[0]] += basins->Ncells[basins->sorted_basins[i]];
        basin_to_node[i] = node_ids[0];
    }

    // determine offsets to use for MPI_Scatterv and MPI_Gatherv
    for (i = 1; i < mpi_size; i++) {
        for (j = 0; j < i; j++) {
            (*mpi_map_global_array_offsets)[i] +=
                (*mpi_map_local_array_sizes)[j];
        }
    }

    // set mapping array
    for (i = 0, l = 0; i < (size_t) mpi_size; i++) {
        for(j=0;j<basins->Nbasin;j++){
            if(basin_to_node[j]==i){
                for(k=0;k<basins->Ncells[j];k++){
                    (*mpi_map_mapping_array)[l++] = basins->catchment[j][k];
                }
            }
        }
    }
}

/******************************************************************************
 * @brief   Decompose the domain for MPI operations
 * @details This function sets up the arrays needed to scatter and gather
 *          data from and to the master process to the individual mpi
 *          processes.
 *
 * @param ncells total number of cells
 * @param mpi_size number of mpi processes
 * @param mpi_map_local_array_sizes address of integer array with number of
 *        cells assigned to each node (MPI_Scatterv:sendcounts and
 *        MPI_Gatherv:recvcounts)
 * @param mpi_map_global_array_offsets address of integer array with offsets
 *        for sending and receiving data (MPI_Scatterv:displs and
 *        MPI_Gatherv:displs)
 * @param mpi_map_mapping_array address of size_t array with indices to prepare
 *        an array on the master process for MPI_Scatterv or map back after
 *        MPI_Gatherv
 *****************************************************************************/
void
mpi_map_decomp_domain_random(size_t   ncells,
                      size_t   mpi_size,
                      int    **mpi_map_local_array_sizes,
                      int    **mpi_map_global_array_offsets,
                      size_t **mpi_map_mapping_array)
{
    size_t i;
    size_t j;
    size_t k;
    size_t n;

    *mpi_map_local_array_sizes = calloc(mpi_size,
                                        sizeof(*(*mpi_map_local_array_sizes)));
    *mpi_map_global_array_offsets = calloc(mpi_size,
                                           sizeof(*(*
                                                    mpi_map_global_array_offsets)));
    *mpi_map_mapping_array = calloc(ncells, sizeof(*(*mpi_map_mapping_array)));

    // determine number of cells per node
    for (n = ncells, i = 0; n > 0; n--, i++) {
        if (i >= mpi_size) {
            i = 0;
        }
        (*mpi_map_local_array_sizes)[i] += 1;
    }

    // determine offsets to use for MPI_Scatterv and MPI_Gatherv
    for (i = 1; i < mpi_size; i++) {
        for (j = 0; j < i; j++) {
            (*mpi_map_global_array_offsets)[i] +=
                (*mpi_map_local_array_sizes)[j];
        }
    }

    // set mapping array
    for (i = 0, k = 0; i < (size_t) mpi_size; i++) {
        for (j = 0; j < (size_t) (*mpi_map_local_array_sizes)[i]; j++) {
            (*mpi_map_mapping_array)[k++] = (size_t) (i + j * mpi_size);
        }
    }
}

/******************************************************************************
 * @brief   Gather double precision variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_double(double *dvar, double *var_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    double *dvar_gathered = NULL;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_gathered =
                malloc(global_domain.ncells_active * sizeof (*dvar_gathered));
        check_alloc_status(dvar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(var_local, local_domain.ncells_active, MPI_DOUBLE,
            dvar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_DOUBLE,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (double), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, dvar_gathered, dvar);

        // cleanup
        free(dvar_gathered);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void gather_double_2d(double **dvar, double **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    
    double *tmp_global = NULL;
    double *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        for(j=0;j<local_domain.ncells_active;j++){
            tmp_local[j] = var_local[j][i];
        }

        gather_double(tmp_global,tmp_local);
            
            
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                dvar[j][i] = tmp_global[j];
            }
        }
    }
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_int(int *ivar, int *ivar_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    int *ivar_gathered = NULL;
    
    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_gathered =
                malloc(global_domain.ncells_active * sizeof (*ivar_gathered));
        check_alloc_status(ivar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(ivar_local, local_domain.ncells_active, MPI_INT,
            ivar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_INT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (int), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, ivar_gathered, ivar);

        // cleanup
        free(ivar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_sizet(size_t *svar, size_t *svar_local) {
    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    size_t *svar_gathered = NULL;
        
    if (mpi_rank == VIC_MPI_ROOT) {
        svar_gathered =
                malloc(global_domain.ncells_active * sizeof (*svar_gathered));
        check_alloc_status(svar_gathered, "Memory allocation error.");
    }
    
    // Gather the results from the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Gatherv(svar_local, local_domain.ncells_active, MPI_AINT,
            svar_gathered, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_AINT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");
    
    if (mpi_rank == VIC_MPI_ROOT) {
        // remap the array
        map(sizeof (size_t), global_domain.ncells_active, NULL,
                mpi_map_mapping_array, svar_gathered, svar);

        // cleanup
        free(svar_gathered);
    }
}

/******************************************************************************
 * @brief   Gather int variable
 * @details Values are gathered to the master node
 *****************************************************************************/
void gather_sizet_2d(size_t **svar, size_t **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    
    size_t *tmp_global = NULL;
    size_t *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        for(j=0;j<local_domain.ncells_active;j++){
            tmp_local[j] = var_local[j][i];
        }

        gather_sizet(tmp_global,tmp_local);
            
            
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                svar[j][i] = tmp_global[j];
            }
        }
    }
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_double(double *dvar, double *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    double *dvar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        dvar_mapped =
                malloc(global_domain.ncells_active * sizeof (*dvar_mapped));
        check_alloc_status(dvar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (double), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, dvar, dvar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(dvar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_DOUBLE,
            var_local, local_domain.ncells_active, MPI_DOUBLE,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(dvar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter double precision variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_double_2d(double **dvar, double **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
        
    double *tmp_global = NULL;
    double *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                tmp_global[j] = dvar[j][i];
            }
        }
        
        scatter_double(tmp_global, tmp_local);
                        
        for(j=0;j<local_domain.ncells_active;j++){
            var_local[j][i] = tmp_local[j];
        }
    } 
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

/******************************************************************************
 * @brief   Scatter integer variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_int(int *ivar, int *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    int *ivar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        ivar_mapped =
                malloc(global_domain.ncells_active * sizeof (*ivar_mapped));
        check_alloc_status(ivar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (int), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, ivar, ivar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(ivar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_INT,
            var_local, local_domain.ncells_active, MPI_INT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(ivar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter size_t variable
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_sizet(size_t *svar, size_t *var_local) {

    extern MPI_Comm MPI_COMM_VIC;
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
    extern int *mpi_map_global_array_offsets;
    extern int *mpi_map_local_array_sizes;
    extern size_t *mpi_map_mapping_array;
    int status;
    size_t *svar_mapped = NULL;

    if (mpi_rank == VIC_MPI_ROOT) {
        svar_mapped =
                malloc(global_domain.ncells_active * sizeof (*svar_mapped));
        check_alloc_status(svar_mapped, "Memory allocation error.");

        // map to prepare for MPI_Scatterv
        map(sizeof (size_t), global_domain.ncells_active, mpi_map_mapping_array,
                NULL, svar, svar_mapped);
    }

    // Scatter the results to the nodes, result for the local node is in the
    // array *var (which is a function argument)
    status = MPI_Scatterv(svar_mapped, mpi_map_local_array_sizes,
            mpi_map_global_array_offsets, MPI_AINT,
            var_local, local_domain.ncells_active, MPI_AINT,
            VIC_MPI_ROOT, MPI_COMM_VIC);
    check_mpi_status(status, "MPI error.");

    if (mpi_rank == VIC_MPI_ROOT) {
        free(svar_mapped);
    }
}

/******************************************************************************
 * @brief   Scatter size_t variable recursive
 * @details values from master node are scattered to the local nodes
 *****************************************************************************/
void scatter_sizet_2d(size_t **svar, size_t **var_local, int depth) {
    extern domain_struct global_domain;
    extern domain_struct local_domain;
    extern int mpi_rank;
        
    size_t *tmp_global = NULL;
    size_t *tmp_local = NULL;
    
    size_t i;
    size_t j;
    
    if(mpi_rank == VIC_MPI_ROOT){
        tmp_global = malloc(global_domain.ncells_active * sizeof(*tmp_global));
        check_alloc_status(tmp_global, "Memory allocation error");
    }
    tmp_local = malloc(local_domain.ncells_active * sizeof(*tmp_local));
    check_alloc_status(tmp_local, "Memory allocation error");
    
    for(i = 0; i < (size_t) depth; i++){        
        if(mpi_rank == VIC_MPI_ROOT){
            for(j=0;j<global_domain.ncells_active;j++){
                tmp_global[j] = svar[j][i];
            }
        }
        
        scatter_sizet(tmp_global, tmp_local);
                        
        for(j=0;j<local_domain.ncells_active;j++){
            var_local[j][i] = tmp_local[j];
        }
    }    
    
    if(mpi_rank == VIC_MPI_ROOT){
        free(tmp_global);
    }
    free(tmp_local);
}

void 
create_MPI_ext_option_struct_type(MPI_Datatype *mpi_type){
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;
    
    nitems = 2;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");

    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");

    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.");
    
    // none of the elements in location_struct are arrays.
    for(i=0; i< (size_t) nitems; i++){
        blocklengths[i]=1;
    }
    
    //reset i
    i=0;
    
    //bool ROUTING;    
    offsets[i] = offsetof(ext_option_struct, ROUTING);
    mpi_types[i++] = MPI_C_BOOL;    
    //int UH_PARAMETERS;
    offsets[i] = offsetof(ext_option_struct, UH_PARAMETERS);
    mpi_types[i++] = MPI_INT;
        
    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }

    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");

    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void 
create_MPI_ext_parameters_struct_type(MPI_Datatype *mpi_type){
    extern MPI_Comm MPI_COMM_VIC;
    int             nitems; // number of elements in struct
    int             status;
    int            *blocklengths;
    size_t          i;
    MPI_Aint       *offsets;
    MPI_Datatype   *mpi_types;
    
    nitems = 6;
    blocklengths = malloc(nitems * sizeof(*blocklengths));
    check_alloc_status(blocklengths, "Memory allocation error.");

    offsets = malloc(nitems * sizeof(*offsets));
    check_alloc_status(offsets, "Memory allocation error.");

    mpi_types = malloc(nitems * sizeof(*mpi_types));
    check_alloc_status(mpi_types, "Memory allocation error.");
      
    // most the elements in location_struct are not arrays.
    for(i=0; i< (size_t) nitems; i++){
        blocklengths[i]=1;
    }
    
    //reset i
    i=0;
    
    //double MPI_N_PROCESS_COST;
    offsets[i] = offsetof(ext_parameters_struct, MPI_N_PROCESS_COST);
    mpi_types[i++] = MPI_DOUBLE;
    //double MPI_E_PROCESS_COST;
    offsets[i] = offsetof(ext_parameters_struct, MPI_E_PROCESS_COST);
    mpi_types[i++] = MPI_DOUBLE;
    //double UH_FLOW_VELOCITY;
    offsets[i] = offsetof(ext_parameters_struct, UH_FLOW_VELOCITY);
    mpi_types[i++] = MPI_DOUBLE;
    //double UH_FLOW_DIFFUSION;
    offsets[i] = offsetof(ext_parameters_struct, UH_FLOW_DIFFUSION);
    mpi_types[i++] = MPI_DOUBLE;
    //int UH_MAX_LENGTH;
    offsets[i] = offsetof(ext_parameters_struct, UH_MAX_LENGTH);
    mpi_types[i++] = MPI_INT;
    //int UH_PARTITIONS;
    offsets[i] = offsetof(ext_parameters_struct, UH_PARTITIONS);
    mpi_types[i++] = MPI_INT;    
    
    // make sure that the we have the right number of elements
    if (i != (size_t) nitems) {
        log_err("Miscount: %zd not equal to %d.", i, nitems);
    }

    status = MPI_Type_create_struct(nitems, blocklengths, offsets, mpi_types,
                                    mpi_type);
    check_mpi_status(status, "MPI error.");

    status = MPI_Type_commit(mpi_type);
    check_mpi_status(status, "MPI error.");

    // cleanup
    free(blocklengths);
    free(offsets);
    free(mpi_types);
}

void
initialize_ext_mpi(){    
    extern int mpi_decomposition;
    
    extern MPI_Datatype mpi_ext_option_struct_type;
    extern MPI_Datatype mpi_ext_param_struct_type;
    
    create_MPI_ext_option_struct_type(&mpi_ext_option_struct_type);
    create_MPI_ext_parameters_struct_type(&mpi_ext_param_struct_type);
    
    mpi_decomposition = RANDOM_DECOMPOSITION;    
}