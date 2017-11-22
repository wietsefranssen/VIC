/******************************************************************************
* @section DESCRIPTION
*
* Calculate infiltration and runoff from the surface, gravity driven drainage
* between all soil layers, and generates baseflow from the bottom layer.
*
* @section LICENSE
*
* The Variable Infiltration Capacity (VIC) macroscale hydrological model
* Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
* and Environmental Engineering, University of Washington.
*
* The VIC model is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
******************************************************************************/

#include <vic_run.h>
#include <groundwater.h>

/******************************************************************************
* @brief    Calculate infiltration and runoff from the surface, gravity driven
*           drainage between all soil layers, and generates baseflow from the
*           bottom layer.
******************************************************************************/
int
runoff(cell_data_struct  *cell,
       energy_bal_struct *energy,           
       gw_var_struct     *gw_var,
       soil_con_struct   *soil_con,
       gw_con_struct     *gw_con,
       double             ppt,
       double            *frost_fract,
       int                Nnodes)
{
    extern option_struct       options;
    extern ext_option_struct   ext_options;
    extern global_param_struct global_param;

    size_t                     lindex;
    size_t                     time_step;
//    int                        last_index;
    int                        tmplayer;
    int                        fidx;
    int                        ErrorFlag;
    double                     A;
    double                     tmp_runoff;
    double                     inflow;
    double                     Q01;
    double                     resid_moist[MAX_LAYERS]; // residual moisture (mm)
    double                     org_moist[MAX_LAYERS]; // total soil moisture (liquid and frozen) at beginning of this function (mm)
    double                     avail_liq[MAX_LAYERS][MAX_FROST_AREAS]; // liquid soil moisture available for evap/drainage (mm)
    double                     liq[MAX_LAYERS]; // current liquid soil moisture (mm)
    double                     ice[MAX_LAYERS]; // current frozen soil moisture (mm)
    double                     moist[MAX_LAYERS]; // current total soil moisture (liquid and frozen) (mm)
    double                     max_moist[MAX_LAYERS]; // maximum storable moisture (liquid and frozen) (mm)
    double                     Ksat[MAX_LAYERS];
    double                     Q12[MAX_LAYERS - 1];   
    double                     tmp_inflow;
    double                     tmp_moist;
    double                     tmp_moist_for_runoff[MAX_LAYERS];
    double                     tmp_liq;
    double                     dt_inflow;
    double                     dt_runoff;
    double                     runoff[MAX_FROST_AREAS];
    double                     tmp_dt_runoff[MAX_FROST_AREAS];
    double                     baseflow[MAX_FROST_AREAS];
    double                     dt_baseflow;
    double                     evap[MAX_LAYERS][MAX_FROST_AREAS];
    double                     sum_liq;
    double                     evap_fraction;
    double                     evap_sum;
    layer_data_struct         *layer;
    layer_data_struct          tmp_layer;
    unsigned short             runoff_steps_per_dt;

     
    double matric[MAX_LAYERS];
    double matric_expt[MAX_LAYERS];
    double Fp[MAX_LAYERS];
    double z[MAX_LAYERS];
    double z_node[MAX_LAYERS];
    double eff_porosity[MAX_LAYERS];
    double Kl[MAX_LAYERS];
    double recharge[MAX_FROST_AREAS];
    double zwt[MAX_FROST_AREAS];
    double Wa[MAX_FROST_AREAS];    
    double Wt[MAX_FROST_AREAS];
    
    double matric_avg;        
    double z_tmp;
    int lwt;
    int lbot;           
    double K1, K2, Ka;
    double K_avg;
    double dt_recharge;
    double dt_exchange;
    double Ws;      
    int new_lwt;
    double old_Wt;
    double old_Wa;
    
    /** Set Residual Moisture **/
    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        resid_moist[lindex] = soil_con->resid_moist[lindex] *
                              soil_con->depth[lindex] * MM_PER_M;
    }

    /** Allocate and Set Values for Soil Sublayers **/
    layer = cell->layer;

    cell->runoff = 0.0;
    cell->baseflow = 0.0;
    cell->asat = 0.0;

    runoff_steps_per_dt = global_param.runoff_steps_per_day /
                          global_param.model_steps_per_day;

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        baseflow[fidx] = 0.0;
        recharge[fidx] = 0.0;
        
        zwt[fidx] = gw_var->zwt;
        Wa[fidx] = gw_var->Wa;
        Wt[fidx] = gw_var->Wt;
    }
    
    gw_var->Qb = 0.0;
    gw_var->Qr = 0.0;
    gw_var->zwt = 0.0;
    gw_var->Wa = 0.0;   
    gw_var->Wt = 0.0;

    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        evap[lindex][0] = layer[lindex].evap / (double) runoff_steps_per_dt;
        org_moist[lindex] = layer[lindex].moist;
        layer[lindex].moist = 0;
        if (evap[lindex][0] > 0) { // if there is positive evaporation
            sum_liq = 0;
            // compute available soil moisture for each frost sub area.
            for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
                avail_liq[lindex][fidx] =
                    (org_moist[lindex] - layer[lindex].ice[fidx] -
                     resid_moist[lindex]);
                if (avail_liq[lindex][fidx] < 0) {
                    avail_liq[lindex][fidx] = 0;
                }
                sum_liq += avail_liq[lindex][fidx] *
                           frost_fract[fidx];
            }
            // compute fraction of available soil moisture that is evaporated
            if (sum_liq > 0) {
                evap_fraction = evap[lindex][0] / sum_liq;
            }
            else {
                evap_fraction = 1.0;
            }
            // distribute evaporation between frost sub areas by percentage
            evap_sum = evap[lindex][0];
            for (fidx = (int)options.Nfrost - 1; fidx >= 0; fidx--) {
                evap[lindex][fidx] = avail_liq[lindex][fidx] * evap_fraction;
                avail_liq[lindex][fidx] -= evap[lindex][fidx];
                evap_sum -= evap[lindex][fidx] * frost_fract[fidx];
            }
        }
        else {
            for (fidx = (int)options.Nfrost - 1; fidx > 0; fidx--) {
                evap[lindex][fidx] = evap[lindex][0];
            }
        }
    }

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        /** ppt = amount of liquid water coming to the surface **/
        inflow = ppt;

        /**************************************************
           Initialize Frostarea Variables
        **************************************************/
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            Ksat[lindex] = soil_con->Ksat[lindex] /
                           global_param.runoff_steps_per_day;

            /** Set Layer Liquid Moisture Content **/
            liq[lindex] = org_moist[lindex] - layer[lindex].ice[fidx];

            /** Set Layer Frozen Moisture Content **/
            ice[lindex] = layer[lindex].ice[fidx];

            /** Set Layer Maximum Moisture Content **/
            max_moist[lindex] = soil_con->max_moist[lindex];
                        
            /** Set fraction of permeable/non-frozen area (Nui & Yang 2006)**/
            Fp[lindex] = 1 - (exp(-soil_con->Fp_expt * 
                    (1 - ice[lindex] / max_moist[lindex])) - 
                    exp(-soil_con->Fp_expt));

            /** Set Matric Potential Exponent (Burdine model 1953) **/
            matric_expt[lindex] = (soil_con->expt[lindex] - 3.0) / 2.0;
            
            /** Set Depth of Layers and Nodes **/
            z_tmp += soil_con->depth[lindex];
            z[lindex] = z_tmp;
            z_node[lindex] = z_tmp - 0.5 * soil_con->depth[lindex];

            /** Set Effective Porosity **/
            eff_porosity[lindex] = ((soil_con->depth[lindex] * 
                    soil_con->porosity[lindex]) - ice[lindex]) /
                    soil_con->depth[lindex];
        }

        /******************************************************
           Runoff Based on Soil Moisture Level of Upper Layers
        ******************************************************/

        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        }
        compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, inflow, &A,
                                &(runoff[fidx]));

        // save dt_runoff based on initial runoff estimate,
        // since we will modify total runoff below for the case of completely saturated soil
        tmp_dt_runoff[fidx] = runoff[fidx] /
                              (double) runoff_steps_per_dt;

        /**************************************************
           Compute Flow Between Soil Layers ()
        **************************************************/
        dt_inflow = inflow / (double) runoff_steps_per_dt;
        dt_runoff = runoff[fidx] / (double) runoff_steps_per_dt;            
            
        /**************************************************
         Check WB
        **************************************************/ 
        double storage_gw_wa_start = 0.0;
        double storage_gw_wt_start = 0.0;
        double storage_ly_start = 0.0;
        double inflow_wb = 0.0;      
        double evap_wb = 0.0;
        for(lindex = 0; lindex < options.Nlayer; lindex ++){
            storage_ly_start += liq[lindex];
            evap_wb += evap[lindex][fidx];
        }
        storage_gw_wa_start += Wa[fidx];  
        storage_gw_wt_start += Wt[fidx];            
        inflow_wb += inflow;
        

        for (time_step = 0; time_step < runoff_steps_per_dt; time_step++) {            
            /**************************************************
               Initialize Timestep Variables
            **************************************************/
            for (lindex = 0; lindex < options.Nlayer; lindex++) {    
                
                // Set matric potential (based on moisture content and soil texture)
                tmp_liq = liq[lindex] - evap[lindex][fidx];
                if(tmp_liq > resid_moist[lindex]){
                    
                    /** Brooks & Corey relation for matric potential **/
                    matric[lindex] = soil_con->bubble[lindex] * 
                            pow((tmp_liq - resid_moist[lindex]) /
                            (max_moist[lindex] - resid_moist[lindex]), 
                            -matric_expt[lindex]);
                }else{
                    matric[lindex] = DRY_RESIST;
                }
            }
            
            /*************************************
               Compute Hydraulic Conductivity of Sublayers
            *************************************/
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                
                tmp_liq = liq[lindex] - evap[lindex][fidx];
                
                if (tmp_liq > resid_moist[lindex]) {
                    if(lindex < options.Nlayer - 1){
                        matric_avg = pow( 10, (soil_con->depth[lindex+1] 
                                         * log10(fabs(matric[lindex]))
                                         + soil_con->depth[lindex]
                                         * log10(fabs(matric[lindex+1])))
                                        / (soil_con->depth[lindex] 
                                           + soil_con->depth[lindex+1]) );

                        tmp_liq = resid_moist[lindex]
                          + ( soil_con->max_moist[lindex] - resid_moist[lindex] )
                          * pow( ( matric_avg / soil_con->bubble[lindex] ), -1/matric_expt[lindex] );
                    }
                    
                    /** Brooks & Corey relation for hydraulic conductivity **/
                    Kl[lindex] = Fp[lindex] * calc_Q12(Ksat[lindex], tmp_liq,
                                                    resid_moist[lindex],
                                                    soil_con->max_moist[lindex],
                                                    soil_con->expt[lindex]);
                }
                else {
                    Kl[lindex] = 0.;
                }
            }
            
            /*************************************
               Compute Drainage between Sublayers
            *************************************/
            for (lindex = 0; lindex < options.Nlayer - 1; lindex++) {
                Q12[lindex] = Kl[lindex];
            }
            
            /**************************************************
               Solve for Current Soil Layer Moisture, and
               Check Versus Maximum and Minimum Moisture Contents.
            **************************************************/
            Q01 = dt_inflow - dt_runoff;
            for (lindex = 0; lindex < options.Nlayer - 1; lindex++) {

                /** Update soil layer moisture content **/
                liq[lindex] = liq[lindex] + Q01 -
                              (Q12[lindex] + evap[lindex][fidx]);   

                /* transport moisture for all sublayers **/
                
                /** Verify that soil layer moisture is less than maximum **/
                tmp_inflow = 0.;
                if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
                    tmp_inflow = (liq[lindex] + ice[lindex]) -
                                 max_moist[lindex];
                    liq[lindex] = max_moist[lindex] - ice[lindex];

                    if (lindex == 0) {
                        Q12[lindex] += tmp_inflow;
                        tmp_inflow = 0;
                    }
                    else {
                        tmplayer = lindex;
                        while (tmp_inflow > 0) {
                            tmplayer--;
                            if (tmplayer < 0) {
                                /** If top layer saturated, add to runoff **/
                                runoff[fidx] += tmp_inflow;
                                tmp_inflow = 0;
                            }
                            else {
                                /** else add excess soil moisture to next higher layer **/
                                liq[tmplayer] += tmp_inflow;
                                if ((liq[tmplayer] + ice[tmplayer]) >
                                    max_moist[tmplayer]) {
                                    tmp_inflow =
                                        ((liq[tmplayer] +
                                          ice[tmplayer]) - max_moist[tmplayer]);
                                    liq[tmplayer] = max_moist[tmplayer] -
                                                    ice[tmplayer];
                                }
                                else {
                                    tmp_inflow = 0;
                                }
                            }
                        }
                    } /** end trapped excess moisture **/
                } /** end check if excess moisture in top layer **/

                /** verify that current layer moisture is greater than minimum **/
                if (liq[lindex] < 0) {
                    /** liquid cannot fall below 0 **/
                    Q12[lindex] += liq[lindex];
                    liq[lindex] = 0;
                }
                if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
                    /** moisture cannot fall below minimum **/
                    Q12[lindex] +=
                        (liq[lindex] + ice[lindex]) - resid_moist[lindex];
                    liq[lindex] = resid_moist[lindex] - ice[lindex];
                }

                Q01 = Q12[lindex];
//                inflow = (Q12[lindex] + tmp_inflow);
//                Q12[lindex] += tmp_inflow;
            } /* end loop through soil layers */
            
//            /**************************************************
//             Check WB
//            **************************************************/
//            double storage_ly_end = 0.0;
//            double recharge_wb = 0.0;
//            double runoff_wb = 0.0;
//            for(lindex = 0; lindex < options.Nlayer; lindex ++){
//                storage_ly_end += liq[lindex];
//                debug("l %zu liq %.3f Q12 %.3f",lindex,liq[lindex],Q12[lindex]);
//            }
//            runoff_wb = runoff[fidx];
//            recharge_wb = Q12[lindex - 2];
//            
//            debug("\n----- SOIL CLOSURE -----\n"
//                    "storage difference %.10f\n"
//                    "flux difference %.10f\n"
//                    "closure %.10f\n"
//                    "--------------------",
//                    storage_ly_end - storage_ly_start,
//                    inflow_wb - runoff_wb - recharge_wb - evap_wb,
//                    (storage_ly_end - storage_ly_start) - 
//                    (inflow_wb - runoff_wb - recharge_wb - evap_wb));
//            
//            if(abs((storage_ly_end - storage_ly_start) - 
//                    (inflow_wb - runoff_wb - recharge_wb - evap_wb)) > 0.000001){
//                debug("error");
//            }
           
            /**************************************************
               Compute Baseflow
            **************************************************/      
            // Find layer with phreatic water level
            lwt = -1;
            lbot = options.Nlayer - 1;
            for (lindex = 0; lindex < options.Nlayer; lindex++) {
                if(zwt[fidx] <= z[lindex]){
                    lwt=lindex;
                    if(lindex == 0){
                        lbot = lwt;
                    }else{
                        lbot = lwt - 1;
                    }
                    break;
                }
            }

            /** Calculate baseflow **/
            if(lwt == -1){                        
                dt_baseflow = (soil_con->Dsmax / global_param.runoff_steps_per_day) * exp(-gw_con->Qb_expt * zwt[fidx]);
            }else{
                dt_baseflow = Fp[lwt] * (soil_con->Dsmax / global_param.runoff_steps_per_day) * exp(-gw_con->Qb_expt * zwt[fidx]);
            }

            /** Calculate recharge **/ 
            if (lwt == -1) {
                // Conductivity in bottom layer   
                K1 = Kl[lbot];
                // Conductivity in aquifer (exponentially decrease conductivity below soil column)
                K2 = K1 * (1 - exp(-gw_con->Ka_expt * (zwt[fidx] - z[lbot]))) / 
                        (gw_con->Ka_expt * (zwt[fidx] - z[lbot]));
                // Depth weighted average of conductivity
                Ka = ((z[lbot] - z_node[lbot]) * K1 +
                        (zwt[fidx] - z[lbot]) * K2) /
                        (zwt[fidx] - z_node[lbot]);

                dt_recharge = Ka;
            } else {
                dt_recharge = Q12[lbot];
            }
            
            /** Calculate soil moisture change **/ 
            if(lwt == -1){
                Q12[options.Nlayer - 1] = dt_recharge;
            }else{
                double tmp_baseflow;
                
                K_avg = 0.0;
                for(lindex = lwt; lindex < options.Nlayer; lindex ++){
                    if((int)lindex == lwt){
                        K_avg += (z[lindex] - zwt[fidx]) * Kl[lindex];
                    }else{
                        K_avg += soil_con->depth[lindex] * Kl[lindex];
                    }
                }
                
                tmp_baseflow = dt_baseflow;
                for(lindex = lwt; lindex < options.Nlayer - 1; lindex ++){
                    if((int)lindex == lwt){
                        liq[lindex] -= dt_baseflow * 
                                (((z[lindex] - zwt[fidx]) * Kl[lindex]) / 
                                K_avg);
                        tmp_baseflow -= dt_baseflow * 
                                (((z[lindex] - zwt[fidx]) * Kl[lindex]) / 
                                K_avg);
                    }else{
                        liq[lindex] -= dt_baseflow * 
                                ((soil_con->depth[lindex] * Kl[lindex]) / 
                                K_avg);
                        tmp_baseflow -= dt_baseflow * 
                                ((soil_con->depth[lindex] * Kl[lindex]) / 
                                K_avg);
                    }
                }
                Q12[options.Nlayer - 1] = tmp_baseflow;
            
                /** Verify that soil layer moisture is greater than minimum **/
                Q01 = 0;            
                for(lindex = lwt; lindex < options.Nlayer - 1; lindex ++){

                    liq[lindex] += Q01;                
                    if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
                        Q01 = liq[lindex] - (resid_moist[lindex] - ice[lindex]);
                        liq[lindex] = resid_moist[lindex] - ice[lindex];
                    } else {
                        Q01 = 0.0;
                    }
                    
                    Q12[lindex] += Q01;
                }
            }           
            
            /**************************************************
               Solve for Current Soil Layer Moisture, and
               Check Versus Maximum and Minimum Moisture Contents.
            **************************************************/             
            /** Update soil layer moisture content **/
            lindex = options.Nlayer - 1; 
            Q01 = Q12[lindex - 1];
            
            liq[lindex] = liq[lindex] + Q01 -
                          (Q12[lindex] + evap[lindex][fidx]);
            
            /* transport moisture for all sublayers **/
            /** Verify that soil layer moisture is less than maximum **/
            tmp_moist = 0.;
            if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
                /* soil moisture above maximum */
                tmp_moist = ((liq[lindex] + ice[lindex]) - max_moist[lindex]);
                liq[lindex] = max_moist[lindex] - ice[lindex];
                
                tmplayer = lindex;                
                while (tmp_moist > 0) {
                    tmplayer--;
                    if (tmplayer < 0) {
                        /** If top layer saturated, add to runoff **/
                        runoff[fidx] += tmp_moist;
                        tmp_moist = 0;
                    }
                    else {
                        /** else add excess soil moisture to next higher layer **/
                        liq[tmplayer] += tmp_moist;
                        if ((liq[tmplayer] + ice[tmplayer]) >
                            max_moist[tmplayer]) {
                            tmp_moist =
                                ((liq[tmplayer] +
                                  ice[tmplayer]) - max_moist[tmplayer]);
                            liq[tmplayer] = max_moist[tmplayer] - ice[tmplayer];
                        }
                        else {
                            tmp_moist = 0;
                        }
                    }
                } /** end trapped excess moisture **/
            } /** end check if excess moisture in top layer **/
            
            /** Verify that soil layer moisture is greater than minimum **/
            /** If liquid is below minimum, subtract from groundwater **/
            if (liq[lindex] < 0) {
                /** liquid cannot fall below 0 **/
                dt_recharge += liq[lindex];
                liq[lindex] = 0;
            }
            if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
                /** moisture cannot fall below minimum **/
                dt_recharge += 
                        (liq[lindex] + ice[lindex]) - resid_moist[lindex];
                liq[lindex] = resid_moist[lindex] - ice[lindex];
            }                      
            
            /**************************************************
               Compute Groundwater
            **************************************************/ 
            old_Wt = Wt[fidx];
            old_Wa = Wa[fidx];
            
            Wt[fidx] += dt_recharge - dt_baseflow;

            if(Wt[fidx] / gw_con->Sy / MM_PER_M < GW_REF_DEPTH - z[options.Nlayer - 1]){
                Wa[fidx] = Wt[fidx];
                zwt[fidx] = GW_REF_DEPTH
                        - Wt[fidx] / gw_con->Sy / MM_PER_M;
                new_lwt = -1;
            }else{
                Wa[fidx] = (GW_REF_DEPTH - z[options.Nlayer - 1]) * gw_con->Sy * MM_PER_M;
                Ws = (Wt[fidx] - Wa[fidx]) / MM_PER_M;
                for(lindex = options.Nlayer -1; (int)lindex >=0; lindex --){
                    if(Ws < soil_con->depth[lindex] * eff_porosity[lindex]){
                        zwt[fidx] = z[lindex] - Ws / eff_porosity[lindex];  
                        new_lwt = lindex;
                        break;
                    }
                    Ws -= soil_con->depth[lindex] * eff_porosity[lindex];
                }               
            }   
            
            /* Handle cases where water table crosses soil column */
            // Add water to bottom layer to compensate for 
            //  1) water table rising above soil column (adding the rise to the soil column)
            //  2) baseflow subtraction while water table was below soil column
            dt_exchange = 0.0;
            if(lwt != new_lwt){
                if(lwt == -1 || new_lwt == -1){ 
                    if(lwt == -1){ 
                        dt_exchange = Wt[fidx] - Wa[fidx];
                    }else if(new_lwt == -1){
                        dt_exchange = old_Wa - Wt[fidx];
                    }
                    lindex = options.Nlayer -1;
                    liq[lindex] += dt_exchange;
                    
                    /** Verify that soil layer moisture is less than maximum **/
                    tmp_moist = 0.;
                    if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
                        /* soil moisture above maximum */
                        tmp_moist = ((liq[lindex] + ice[lindex]) - max_moist[lindex]);
                        liq[lindex] = max_moist[lindex] - ice[lindex];

                        tmplayer = lindex;                
                        while (tmp_moist > 0) {
                            tmplayer--;
                            if (tmplayer < 0) {
                                /** If top layer saturated, add to runoff **/
                                runoff[fidx] += tmp_moist;
                                tmp_moist = 0;
                            }
                            else {
                                /** else add excess soil moisture to next higher layer **/
                                liq[tmplayer] += tmp_moist;
                                if ((liq[tmplayer] + ice[tmplayer]) >
                                    max_moist[tmplayer]) {
                                    tmp_moist =
                                        ((liq[tmplayer] +
                                          ice[tmplayer]) - max_moist[tmplayer]);
                                    liq[tmplayer] = max_moist[tmplayer] - ice[tmplayer];
                                }
                                else {
                                    tmp_moist = 0;
                                }
                            }
                        } /** end trapped excess moisture **/
                    } /** end check if excess moisture in top layer **/
                }
            }
            
            recharge[fidx] += dt_recharge; 
            baseflow[fidx] += dt_baseflow;
        } /* end of sub-dt time step loop */

            
            /**************************************************
             Check WB
            **************************************************/ 
            double storage_gw_wa_end = 0.0;
            double storage_gw_wt_end = 0.0;
            double storage_ly_end = 0.0;
            double baseflow_wb = 0.0;
            double recharge_wb = 0.0;
            double runoff_wb = 0.0;
            double balance = 0.0;
            for(lindex = 0; lindex < options.Nlayer; lindex ++){
                storage_ly_end += liq[lindex];
//                debug("l %zu liq %.3f Q12 %.3f",lindex,liq[lindex],Q12[lindex]);
            }
            storage_gw_wa_end += Wa[fidx];
            storage_gw_wt_end += Wt[fidx];             
            baseflow_wb += baseflow[fidx];
            recharge_wb += recharge[fidx];
            runoff_wb += runoff[fidx];
            
            balance = abs((storage_gw_wa_end + storage_ly_end - storage_gw_wa_start- storage_ly_start) - 
                (inflow_wb - baseflow_wb - runoff_wb - evap_wb));
            if(balance < 0.00000000001){
                if(lwt==-1){
                    balance = abs((storage_ly_end - storage_ly_start) - 
                            (inflow_wb + dt_exchange - runoff_wb - recharge_wb - evap_wb));
                } else {
                    balance = abs((storage_ly_end - storage_ly_start) - 
                            (inflow_wb + dt_exchange - runoff_wb - baseflow_wb - evap_wb));
                }
            }
            if(balance < 0.00000000001){
                balance = abs((storage_gw_wt_end - storage_gw_wt_start) - 
                        (recharge_wb - baseflow_wb));
            }
            
            if(balance > 0.00000000001){            
                debug("\n----- WB CLOSURE -----\n"
                        "storage difference %.10f\n"
                        "flux difference %.10f\n"
                        "closure %.10f\n"
                        "--------------------",
                        storage_gw_wa_end + storage_ly_end - storage_gw_wa_start- storage_ly_start,
                        inflow_wb - baseflow_wb - runoff_wb - evap_wb,
                        (storage_gw_wa_end + storage_ly_end - storage_gw_wa_start- storage_ly_start) - 
                        (inflow_wb - baseflow_wb - runoff_wb - evap_wb));
                if(lwt==-1){
                    debug("\n----- SOIL CLOSURE -----\n"
                            "storage difference %.10f\n"
                            "flux difference %.10f\n"
                            "closure %.10f\n"
                            "--------------------",
                            storage_ly_end - storage_ly_start,
                            inflow_wb + dt_exchange - runoff_wb - recharge_wb - evap_wb,
                            (storage_ly_end - storage_ly_start) - 
                            (inflow_wb + dt_exchange - runoff_wb - recharge_wb - evap_wb));
                }else{
                    debug("\n----- SOIL CLOSURE -----\n"
                            "storage difference %.10f\n"
                            "flux difference %.10f\n"
                            "closure %.10f\n"
                            "--------------------",
                            storage_ly_end - storage_ly_start,
                            inflow_wb + dt_exchange - runoff_wb - baseflow_wb - evap_wb,
                            (storage_ly_end - storage_ly_start) - 
                            (inflow_wb + dt_exchange - runoff_wb - baseflow_wb - evap_wb));
                }
                
                debug("\n----- GW CLOSURE -----\n"
                        "storage difference %.10f\n"
                        "flux difference %.10f\n"
                        "closure %.10f\n"
                        "--------------------",
                        storage_gw_wt_end - storage_gw_wt_start,
                        recharge_wb - baseflow_wb,
                        (storage_gw_wt_end - storage_gw_wt_start) - 
                        (recharge_wb - baseflow_wb));
                
                debug("LWT %d\t NEW_LWT %d",lwt,new_lwt);
                
                debug("WB NOT CLOSED");
            }
            
        /** If negative baseflow, reduce evap accordingly **/
        if (baseflow[fidx] < 0) {         
            log_err("Negative baseflow in groundwater mode? Impossible...");
            layer[lindex].evap += baseflow[fidx];
            baseflow[fidx] = 0;
        }

        /** Recompute Asat based on final moisture level of upper layers **/
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        }
        compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, 0, &A,
                                &tmp_runoff);

        /** Store tile-wide values **/
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            layer[lindex].moist +=
                ((liq[lindex] + ice[lindex]) * frost_fract[fidx]);
        }
        cell->asat += A * frost_fract[fidx];
        cell->runoff += runoff[fidx] * frost_fract[fidx];
        cell->baseflow += baseflow[fidx] * frost_fract[fidx];
        
        gw_var->Qr += recharge[fidx] * frost_fract[fidx];
        gw_var->Qb += baseflow[fidx] * frost_fract[fidx];
        gw_var->zwt += zwt[fidx] * frost_fract[fidx];
        gw_var->Wa += Wa[fidx] * frost_fract[fidx];
        gw_var->Wt += Wt[fidx] * frost_fract[fidx];
    }

    /** Compute water table depth **/
    wrap_compute_zwt(soil_con, cell);

    /** Recompute Thermal Parameters Based on New Moisture Distribution **/
    if (options.FULL_ENERGY || options.FROZEN_SOIL) {
        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_layer = cell->layer[lindex];
            moist[lindex] = tmp_layer.moist;
        }

        ErrorFlag = distribute_node_moisture_properties(energy->moist,
                                                        energy->ice,
                                                        energy->kappa_node,
                                                        energy->Cs_node,
                                                        soil_con->Zsum_node,
                                                        energy->T,
                                                        soil_con->max_moist_node,
                                                        soil_con->expt_node,
                                                        soil_con->bubble_node,
                                                        moist, soil_con->depth,
                                                        soil_con->soil_dens_min,
                                                        soil_con->bulk_dens_min,
                                                        soil_con->quartz,
                                                        soil_con->soil_density,
                                                        soil_con->bulk_density,
                                                        soil_con->organic, Nnodes,
                                                        options.Nlayer,
                                                        soil_con->FS_ACTIVE);
        if (ErrorFlag == ERROR) {
            return (ERROR);
        }
    }
    return (0);
}

/******************************************************************************
* @brief    Calculate the saturated area and runoff
******************************************************************************/
void
compute_runoff_and_asat(soil_con_struct *soil_con,
                        double          *moist,
                        double           inflow,
                        double          *A,
                        double          *runoff)
{
    extern option_struct options;
    double               top_moist; // total moisture (liquid and frozen) in topmost soil layers (mm)
    double               top_max_moist; // maximum storable moisture (liquid and frozen) in topmost soil layers (mm)
    size_t               lindex;
    double               ex;
    double               max_infil;
    double               i_0;
    double               basis;

    top_moist = 0.;
    top_max_moist = 0.;
    for (lindex = 0; lindex < options.Nlayer - 1; lindex++) {
        top_moist += moist[lindex];
        top_max_moist += soil_con->max_moist[lindex];
    }
    if (top_moist > top_max_moist) {
        top_moist = top_max_moist;
    }

    /** A as in Wood et al. in JGR 97, D3, 1992 equation (1) **/
    ex = soil_con->b_infilt / (1.0 + soil_con->b_infilt);
    *A = 1.0 - pow((1.0 - top_moist / top_max_moist), ex);

    max_infil = (1.0 + soil_con->b_infilt) * top_max_moist;
    i_0 = max_infil * (1.0 - pow((1.0 - *A), (1.0 / soil_con->b_infilt)));

    /** equation (3a) Wood et al. **/

    if (inflow == 0.0) {
        *runoff = 0.0;
    }
    else if (max_infil == 0.0) {
        *runoff = inflow;
    }
    else if ((i_0 + inflow) > max_infil) {
        *runoff = inflow - top_max_moist + top_moist;
    }
    /** equation (3b) Wood et al. (wrong in paper) **/
    else {
        basis = 1.0 - (i_0 + inflow) / max_infil;
        *runoff = (inflow - top_max_moist + top_moist +
                   top_max_moist *
                   pow(basis, 1.0 * (1.0 + soil_con->b_infilt)));
    }
    if (*runoff < 0.) {
        *runoff = 0.;
    }
}

/******************************************************************************
* @brief    Calculate drainage between two layers
******************************************************************************/
double
calc_Q12(double Ksat,
         double init_moist,
         double resid_moist,
         double max_moist,
         double expt)
{
    double Q12;

    Q12 = init_moist - pow(pow(init_moist - resid_moist, 1.0 - expt) -
                           Ksat /
                           pow(max_moist - resid_moist, expt) * (1.0 - expt),
                           1.0 / (1.0 - expt)) - resid_moist;

    return Q12;
}