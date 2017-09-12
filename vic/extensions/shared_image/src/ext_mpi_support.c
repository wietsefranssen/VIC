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
                      size_t **mpi_map_mapping_array,
                      size_t **mpi_map_mapping_array_reverse)
{
    extern ext_parameters_struct ext_param;
    extern ext_filenames_struct ext_filenames;
    extern basin_struct  basins;
    extern int mpi_decomposition;
    
    size_t i;
    int j;
    size_t k;
    size_t node_ids[mpi_size];
    
    if(mpi_decomposition == CALCULATE_DECOMPOSITION ||
            mpi_decomposition == BASIN_DECOMPOSITION){
        
        get_basins(ext_filenames.routing, ext_filenames.info.direction_var, &basins);
        
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
            ext_param.MPI_N_PROCESS_COST){
                
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
    
    //set reverse map
    (*mpi_map_mapping_array_reverse) = malloc(ncells * sizeof (*(*mpi_map_mapping_array_reverse)));
    check_alloc_status((*mpi_map_mapping_array_reverse),"Memory allocation error");
            
    k=0;
    for (i = 0; i < mpi_size; i++) {
        for(j = 0; j < (*mpi_map_local_array_sizes)[i]; j++){
            (*mpi_map_mapping_array_reverse)[(*mpi_map_mapping_array)[k]] = j;              
            k++;
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