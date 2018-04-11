/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for vic_driver_shared_all routines
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
 *****************************************************************************/

#ifndef VIC_GENERAL_H
#define VIC_GENERAL_H

#include <vic_mpi.h>

#define VIC_DRIVER "VIC-WUR"

#define MAXDIMS 10
#define AREA_SUM_ERROR_THRESH 1e-10

// Define maximum array sizes for driver level objects
#define MAX_OUTPUT_STREAMS 20

// Output compression setting
#define COMPRESSION_LVL_UNSET -1
#define COMPRESSION_LVL_DEFAULT 5

// Default ouput values
#define OUT_MULT_DEFAULT 0  // Why is this not 1?
#define OUT_ASCII_FORMAT_DEFAULT "%.4f"

// Default snow band setting
#define SNOW_BAND_TRUE_BUT_UNSET 99999

// Max counter for root distribution iteration
#define MAX_ROOT_ITER 9999

/******************************************************************************
 * @brief   NetCDF file types
 *****************************************************************************/
enum
{
    NC_HISTORY_FILE,
    NC_STATE_FILE,
};

/******************************************************************************
 * @brief   File formats
 *****************************************************************************/
enum
{
    UNSET_FILE_FORMAT,
    ASCII,
    BINARY,
    NETCDF3_CLASSIC,
    NETCDF3_64BIT_OFFSET,
    NETCDF4_CLASSIC,
    NETCDF4
};

/******************************************************************************
 * @brief   endian flags
 *****************************************************************************/
enum
{
    LITTLE,  /**< little-endian flag */
    BIG      /**< big-endian flag */
};

/******************************************************************************
 * @brief   Veg param sources
 *****************************************************************************/
enum
{
    FROM_DEFAULT,
    FROM_VEGLIB,
    FROM_VEGPARAM,
    FROM_VEGHIST
};


/******************************************************************************
 * @brief   Output Variable Types
 *****************************************************************************/
enum
{
    // Water Balance Terms - state variables
    OUT_ASAT,             /**< Saturated Area Fraction */
    OUT_LAKE_AREA_FRAC,   /**< lake surface area as fraction of the grid cell area [fraction] */
    OUT_LAKE_DEPTH,       /**< lake depth (distance between surface and deepest point) [m] */
    OUT_LAKE_ICE,         /**< moisture stored as lake ice [mm over lake ice area] */
    OUT_LAKE_ICE_FRACT,   /**< fractional coverage of lake ice [fraction] */
    OUT_LAKE_ICE_HEIGHT,  /**< thickness of lake ice [cm] */
    OUT_LAKE_MOIST,       /**< liquid water and ice stored in lake [mm over grid cell] */
    OUT_LAKE_SURF_AREA,   /**< lake surface area [m2] */
    OUT_LAKE_SWE,         /**< liquid water equivalent of snow on top of lake ice [m over lake ice area] */
    OUT_LAKE_SWE_V,       /**< volumetric liquid water equivalent of snow on top of lake ice [m3] */
    OUT_LAKE_VOLUME,      /**< lake volume [m3] */
    OUT_ROOTMOIST,        /**< root zone soil moisture  [mm] */
    OUT_SMFROZFRAC,       /**< fraction of soil moisture (by mass) that is ice, for each soil layer */
    OUT_SMLIQFRAC,        /**< fraction of soil moisture (by mass) that is liquid, for each soil layer */
    OUT_SNOW_CANOPY,      /**< snow interception storage in canopy  [mm] */
    OUT_SNOW_COVER,       /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH,       /**< depth of snow pack [cm] */
    OUT_SOIL_ICE,         /**< soil ice content  [mm] for each soil layer */
    OUT_SOIL_LIQ,         /**< soil liquid content  [mm] for each soil layer */
    OUT_SOIL_ICE_FRAC,    /**< soil ice content fraction of column volume [1] for each soil layer */
    OUT_SOIL_LIQ_FRAC,    /**< soil liquid content fraction of column volume [1] for each soil layer */
    OUT_SOIL_MOIST,       /**< soil total moisture content  [mm] for each soil layer */
    OUT_SOIL_EFF_SAT,     /**< soil effective saturation [-] for each soil layer */
    OUT_SOIL_WET,         /**< vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
    OUT_SURFSTOR,         /**< storage of liquid water and ice (not snow) on surface (ponding) [mm] */
    OUT_SURF_FROST_FRAC,  /**< fraction of soil surface that is frozen [fraction] */
    OUT_SWE,              /**< snow water equivalent in snow pack (including vegetation-intercepted snow)  [mm] */
    OUT_WDEW,             /**< total moisture interception storage in canopy [mm] */
    OUT_ZWT,              /**< water table position [cm] (zwt within lowest unsaturated layer) */
    OUT_ZWT_LUMPED,       /**< lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */
    // Water Balance Terms - fluxes
    OUT_BASEFLOW,         /**< baseflow out of the bottom layer  [mm] */
    OUT_DELINTERCEPT,     /**< change in canopy interception storage  [mm] */
    OUT_DELSOILMOIST,     /**< change in soil water content  [mm] */
    OUT_DELSURFSTOR,      /**< change in surface liquid water storage  [mm] */
    OUT_DELSWE,           /**< change in snow water equivalent  [mm] */
    OUT_EVAP,             /**< total net evaporation [mm] */
    OUT_EVAP_BARE,        /**< net evaporation from bare soil [mm] */
    OUT_EVAP_CANOP,       /**< net evaporation from canopy interception [mm] */
    OUT_INFLOW,           /**< moisture that reaches top of soil column [mm] */
    OUT_LAKE_BF_IN,       /**< incoming baseflow from lake catchment [mm] */
    OUT_LAKE_BF_IN_V,     /**< incoming volumetric baseflow from lake catchment [m3] */
    OUT_LAKE_BF_OUT,      /**< outgoing baseflow from lake [mm] */
    OUT_LAKE_BF_OUT_V,    /**< outgoing volumetric baseflow from lake [m3] */
    OUT_LAKE_CHAN_IN,     /**< channel inflow into lake [mm] */
    OUT_LAKE_CHAN_IN_V,   /**< volumetric channel inflow into lake [m3] */
    OUT_LAKE_CHAN_OUT,    /**< channel outflow from lake [mm] */
    OUT_LAKE_CHAN_OUT_V,  /**< volumetric channel outflow from lake [m3] */
    OUT_LAKE_DSTOR,       /**< change in lake moisture storage (liquid plus ice cover) [mm] */
    OUT_LAKE_DSTOR_V,     /**< volumetric change in lake moisture storage (liquid plus ice cover) [m3] */
    OUT_LAKE_DSWE,        /**< change in swe on top of lake ice [mm] */
    OUT_LAKE_DSWE_V,      /**< volumetric change in swe on top of lake ice [m3] */
    OUT_LAKE_EVAP,        /**< net evaporation from lake surface [mm] */
    OUT_LAKE_EVAP_V,      /**< net volumetric evaporation from lake surface [m3] */
    OUT_LAKE_PREC_V,      /**< volumetric precipitation over lake surface [m3] */
    OUT_LAKE_RCHRG,       /**< recharge from lake to surrounding wetland [mm] */
    OUT_LAKE_RCHRG_V,     /**< volumetric recharge from lake to surrounding wetland [m3] */
    OUT_LAKE_RO_IN,       /**< incoming runoff from lake catchment [mm] */
    OUT_LAKE_RO_IN_V,     /**< incoming volumetric runoff from lake catchment [m3] */
    OUT_LAKE_VAPFLX,      /**< outgoing sublimation from snow on top of lake ice [mm] */
    OUT_LAKE_VAPFLX_V,    /**< outgoing volumetric sublimation from snow on top of lake ice [m3] */
    OUT_PET,              /**< Potential evapotranspiration (= area-weighted sum of potential transpiration and potential soil evaporation).  Potential transpiration is computed using the Penman-Monteith eqn with architectural resistance and LAI of the current veg cover. [mm] */
    OUT_PREC,             /**< incoming precipitation [mm] */
    OUT_RAINF,            /**< rainfall  [mm] */
    OUT_REFREEZE,         /**< refreezing of water in the snow  [mm] */
    OUT_RUNOFF,           /**< surface runoff [mm] */
    OUT_SNOW_MELT,        /**< snow melt  [mm] */
    OUT_SNOWF,            /**< snowfall  [mm] */
    OUT_SUB_BLOWING,      /**< net sublimation of blowing snow [mm] */
    OUT_SUB_CANOP,        /**< net sublimation from snow stored in canopy [mm] */
    OUT_SUB_SNOW,         /**< total net sublimation from snow pack (surface and blowing) [mm] */
    OUT_SUB_SURFACE,      /**< net sublimation from snow pack surface [mm] */
    OUT_TRANSP_VEG,       /**< net transpiration from vegetation [mm] */
    OUT_WATER_ERROR,      /**< water budget error [mm] */
    // Energy Balance Terms - state variables
    OUT_ALBEDO,           /**< average surface albedo [fraction] */
    OUT_BARESOILT,        /**< bare soil surface temperature [C] */
    OUT_FDEPTH,           /**< depth of freezing fronts [cm] */
    OUT_LAKE_ICE_TEMP,    /**< temperature of lake ice [C] */
    OUT_LAKE_SURF_TEMP,   /**< lake surface temperature [C] */
    OUT_RAD_TEMP,         /**< average radiative surface temperature [K] */
    OUT_SALBEDO,          /**< snow pack albedo [fraction] */
    OUT_SNOW_PACK_TEMP,   /**< snow pack temperature [C] */
    OUT_SNOW_SURF_TEMP,   /**< snow surface temperature [C] */
    OUT_SNOWT_FBFLAG,     /**< snow surface temperature fallback flag */
    OUT_SOIL_TEMP,        /**< soil temperature [C] */
    OUT_SOIL_TNODE,       /**< soil temperature [C] */
    OUT_SOIL_TNODE_WL,    /**< soil temperature [C] */
    OUT_SOILT_FBFLAG,     /**< soil temperature flag for each soil thermal node */
    OUT_SURF_TEMP,        /**< average surface temperature [C] */
    OUT_SURFT_FBFLAG,     /**< surface temperature flag */
    OUT_TCAN_FBFLAG,      /**< Tcanopy flag */
    OUT_TDEPTH,           /**< depth of thawing fronts [cm] */
    OUT_TFOL_FBFLAG,      /**< Tfoliage flag */
    OUT_VEGT,             /**< average vegetation canopy temperature [C] */
    // Energy Balance Terms - fluxes
    OUT_ADV_SENS,         /**< net sensible flux advected to snow pack [W/m2] */
    OUT_ADVECTION,        /**< advected energy [W/m2] */
    OUT_DELTACC,          /**< rate of change in cold content in snow pack [W/m2] */
    OUT_DELTAH,           /**< rate of change in heat storage [W/m2] */
    OUT_ENERGY_ERROR,     /**< energy budget error [W/m2] */
    OUT_FUSION,           /**< net energy used to melt/freeze soil moisture [W/m2] */
    OUT_GRND_FLUX,        /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG,          /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT,           /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB,       /**< net upward latent heat flux from sublimation [W/m2] */
    OUT_MELT_ENERGY,      /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET,            /**< net downward longwave flux [W/m2] */
    OUT_SWNET,            /**< net downward shortwave flux [W/m2] */
    OUT_R_NET,            /**< net downward radiation flux [W/m2] */
    OUT_RFRZ_ENERGY,      /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE,         /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_FLUX,        /**< energy flux through snow pack [W/m2] */
    // Miscellaneous Terms
    OUT_AERO_COND,        /**< "scene" aerodynamic conductance [m/s] (tiles with overstory contribute overstory conductance; others contribute surface conductance) */
    OUT_AERO_COND1,       /**< surface aerodynamic conductance [m/s] */
    OUT_AERO_COND2,       /**< overstory aerodynamic conductance [m/s] */
    OUT_AERO_RESIST,      /**< "scene"canopy aerodynamic resistance [s/m]  (tiles with overstory contribute overstory resistance; others contribute surface resistance)*/
    OUT_AERO_RESIST1,     /**< surface aerodynamic resistance [s/m] */
    OUT_AERO_RESIST2,     /**< overstory aerodynamic resistance [s/m] */
    OUT_AIR_TEMP,         /**< air temperature [C] */
    OUT_CATM,             /**< atmospheric CO2 concentrtaion [ppm]*/
    OUT_DENSITY,          /**< near-surface atmospheric density [kg/m3]*/
    OUT_FCANOPY,          /**< fractional area covered by plant canopy [fraction] */
    OUT_FDIR,             /**< fraction of incoming shortwave that is direct [fraction]*/
    OUT_LAI,              /**< leaf area index [m2/m2] */
    OUT_LWDOWN,           /**< incoming longwave [W/m2] */
    OUT_PAR,              /**< incoming photosynthetically active radiation [W/m2] */
    OUT_PRESSURE,         /**< near surface atmospheric pressure [kPa] */
    OUT_QAIR,             /**< specific humidity [kg/kg] */
    OUT_REL_HUMID,        /**< relative humidity [%]*/
    OUT_SWDOWN,           /**< incoming shortwave [W/m2] */
    OUT_SURF_COND,        /**< surface conductance [m/s] */
    OUT_VP,               /**< near surface vapor pressure [kPa] */
    OUT_VPD,              /**< near surface vapor pressure deficit [kPa] */
    OUT_WIND,             /**< near surface wind speed [m/s] */
    // Band-specific quantities
    OUT_ADV_SENS_BAND,    /**< net sensible heat flux advected to snow pack [W/m2] */
    OUT_ADVECTION_BAND,   /**< advected energy [W/m2] */
    OUT_ALBEDO_BAND,      /**< average surface albedo [fraction] */
    OUT_DELTACC_BAND,     /**< change in cold content in snow pack [W/m2] */
    OUT_GRND_FLUX_BAND,   /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG_BAND,     /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT_BAND,      /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB_BAND,  /**< net upward latent heat flux due to sublimation [W/m2] */
    OUT_MELT_ENERGY_BAND, /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET_BAND,       /**< net downward longwave flux [W/m2] */
    OUT_SWNET_BAND,       /**< net downward shortwave flux [W/m2] */
    OUT_RFRZ_ENERGY_BAND, /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE_BAND,    /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_CANOPY_BAND, /**< snow interception storage in canopy [mm] */
    OUT_SNOW_COVER_BAND,  /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH_BAND,  /**< depth of snow pack [cm] */
    OUT_SNOW_FLUX_BAND,   /**< energy flux through snow pack [W/m2] */
    OUT_SNOW_MELT_BAND,   /**< snow melt [mm] */
    OUT_SNOW_PACKT_BAND,  /**< snow pack temperature [C] */
    OUT_SNOW_SURFT_BAND,  /**< snow surface temperature [C] */
    OUT_SWE_BAND,         /**< snow water equivalent in snow pack [mm] */
    // Carbon-Cycling Terms
    OUT_APAR,             /**< absorbed PAR [W/m2] */
    OUT_GPP,              /**< gross primary productivity [g C/m2d] */
    OUT_RAUT,             /**< autotrophic respiration [g C/m2d] */
    OUT_NPP,              /**< net primary productivity [g C/m2d] */
    OUT_LITTERFALL,       /**< flux of carbon from living biomass into soil [g C/m2d] */
    OUT_RHET,             /**< soil respiration (heterotrophic respiration) [g C/m2d] */
    OUT_NEE,              /**< net ecosystem exchange (=NPP-RHET) [g C/m2d] */
    OUT_CLITTER,          /**< Carbon density in litter pool [g C/m2] */
    OUT_CINTER,           /**< Carbon density in intermediate pool [g C/m2] */
    OUT_CSLOW,            /**< Carbon density in slow pool [g C/m2] */
    // Timing and Profiling Terms
    OUT_TIME_VICRUN_WALL, /**< Wall time spent inside vic_run [seconds] */
    OUT_TIME_VICRUN_CPU,  /**< Wall time spent inside vic_run [seconds] */
    
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_OUTVAR_TYPES        /**< used as a loop counter*/
};

/******************************************************************************
 * @brief   Output state variable.
 *****************************************************************************/
enum
{
    STATE_SOIL_MOISTURE,               /**<  total soil moisture */
    STATE_SOIL_ICE,                    /**<  ice content */
    STATE_CANOPY_WATER,                /**<  dew storage: tmpval = veg_var[veg][band].Wdew; */
    STATE_ANNUALNPP,                   /**<  cumulative NPP: tmpval = veg_var[veg][band].AnnualNPP; */
    STATE_ANNUALNPPPREV,               /**<  previous NPP: tmpval = veg_var[veg][band].AnnualNPPPrev; */
    STATE_CLITTER,                     /**<  litter carbon: tmpval = cell[veg][band].CLitter; */
    STATE_CINTER,                      /**<  intermediate carbon: tmpval = cell[veg][band].CInter; */
    STATE_CSLOW,                       /**<  slow carbon: tmpval = cell[veg][band].CSlow; */
    STATE_SNOW_AGE,                    /**<  snow age: snow[veg][band].last_snow */
    STATE_SNOW_MELT_STATE,             /**<  melting state: (int)snow[veg][band].MELTING */
    STATE_SNOW_COVERAGE,               /**<  snow covered fraction: snow[veg][band].coverage */
    STATE_SNOW_WATER_EQUIVALENT,       /**<  snow water equivalent: snow[veg][band].swq */
    STATE_SNOW_SURF_TEMP,              /**<  snow surface temperature: snow[veg][band].surf_temp */
    STATE_SNOW_SURF_WATER,             /**<  snow surface water: snow[veg][band].surf_water */
    STATE_SNOW_PACK_TEMP,              /**<  snow pack temperature: snow[veg][band].pack_temp */
    STATE_SNOW_PACK_WATER,             /**<  snow pack water: snow[veg][band].pack_water */
    STATE_SNOW_DENSITY,                /**<  snow density: snow[veg][band].density */
    STATE_SNOW_COLD_CONTENT,           /**<  snow cold content: snow[veg][band].coldcontent */
    STATE_SNOW_CANOPY,                 /**<  snow canopy storage: snow[veg][band].snow_canopy */
    STATE_SOIL_NODE_TEMP,              /**<  soil node temperatures: energy[veg][band].T[nidx] */
    STATE_FOLIAGE_TEMPERATURE,         /**<  Foliage temperature: energy[veg][band].Tfoliage */
    STATE_ENERGY_LONGUNDEROUT,         /**<  Outgoing longwave from understory: energy[veg][band].LongUnderOut */
    STATE_ENERGY_SNOW_FLUX,            /**<  Thermal flux through the snow pack: energy[veg][band].snow_flux */
    STATE_LAKE_SOIL_MOISTURE,          /**<  total soil moisture */
    STATE_LAKE_SOIL_ICE,               /**<  ice content */
    STATE_LAKE_CLITTER,                /**<  litter carbon: tmpval = lake_var.soil.CLitter; */
    STATE_LAKE_CINTER,                 /**<  intermediate carbon: tmpval = lake_var.soil.CInter; */
    STATE_LAKE_CSLOW,                  /**<  slow carbon: tmpval = lake_var.soil.CSlow; */
    STATE_LAKE_SNOW_AGE,               /**<  snow age: lake_var.snow.last_snow */
    STATE_LAKE_SNOW_MELT_STATE,        /**<  melting state: (int)lake_var.snow.MELTING */
    STATE_LAKE_SNOW_COVERAGE,          /**<  snow covered fraction: lake_var.snow.coverage */
    STATE_LAKE_SNOW_WATER_EQUIVALENT,  /**<  snow water equivalent: lake_var.snow.swq */
    STATE_LAKE_SNOW_SURF_TEMP,         /**<  snow surface temperature: lake_var.snow.surf_temp */
    STATE_LAKE_SNOW_SURF_WATER,        /**<  snow surface water: lake_var.snow.surf_water */
    STATE_LAKE_SNOW_PACK_TEMP,         /**<  snow pack temperature: lake_var.snow.pack_temp */
    STATE_LAKE_SNOW_PACK_WATER,        /**<  snow pack water: lake_var.snow.pack_water */
    STATE_LAKE_SNOW_DENSITY,           /**<  snow density: lake_var.snow.density */
    STATE_LAKE_SNOW_COLD_CONTENT,      /**<  snow cold content: lake_var.snow.coldcontent */
    STATE_LAKE_SNOW_CANOPY,            /**<  snow canopy storage: lake_var.snow.snow_canopy */
    STATE_LAKE_SOIL_NODE_TEMP,         /**<  soil node temperatures: lake_var.energy.T[nidx] */
    STATE_LAKE_ACTIVE_LAYERS,          /**<  lake active layers: lake_var.activenod */
    STATE_LAKE_LAYER_DZ,               /**<  lake layer thickness: lake_var.dz */
    STATE_LAKE_SURF_LAYER_DZ,          /**<  lake surface layer thickness: lake_var.surfdz */
    STATE_LAKE_DEPTH,                  /**<  lake depth: lake_var.ldepth */
    STATE_LAKE_LAYER_SURF_AREA,        /**<  lake layer surface areas: lake_var.surface[ndix] */
    STATE_LAKE_SURF_AREA,              /**<  lake surface area: lake_var.sarea */
    STATE_LAKE_VOLUME,                 /**<  lake volume: lake_var.volume */
    STATE_LAKE_LAYER_TEMP,             /**<  lake layer temperatures: lake_var.temp[nidx] */
    STATE_LAKE_AVERAGE_TEMP,           /**<  vertical average lake temperature: lake_var.tempavg */
    STATE_LAKE_ICE_AREA_FRAC,          /**<  lake ice area fraction: lake_var.areai */
    STATE_LAKE_ICE_AREA_FRAC_NEW,      /**<  new lake ice area fraction: lake_var.new_ice_area */
    STATE_LAKE_ICE_WATER_EQUIVALENT,   /**<  lake ice water equivalent: lake_var.ice_water_eq */
    STATE_LAKE_ICE_HEIGHT,             /**<  lake ice height: lake_var.hice */
    STATE_LAKE_ICE_TEMP,               /**<  lake ice temperature: lake_var.tempi */
    STATE_LAKE_ICE_SWE,                /**<  lake ice snow water equivalent: lake_var.swe */
    STATE_LAKE_ICE_SNOW_SURF_TEMP,     /**<  lake ice snow surface temperature: lake_var.surf_temp */
    STATE_LAKE_ICE_SNOW_PACK_TEMP,     /**<  lake ice snow pack temperature: lake_var.pack_temp */
    STATE_LAKE_ICE_SNOW_COLD_CONTENT,  /**<  lake ice snow coldcontent: lake_var.coldcontent */
    STATE_LAKE_ICE_SNOW_SURF_WATER,    /**<  lake ice snow surface water: lake_var.surf_water */
    STATE_LAKE_ICE_SNOW_PACK_WATER,    /**<  lake ice snow pack water: lake_var.pack_water */
    STATE_LAKE_ICE_SNOW_ALBEDO,        /**<  lake ice snow albedo: lake_var.SAlbedo */
    STATE_LAKE_ICE_SNOW_DEPTH,         /**<  lake ice snow depth: lake_var.sdepth */
    STATE_AVG_ALBEDO,                  /**<  gridcell-averaged albedo: gridcell_avg.avg_albedo */
        
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_STATE_VARS                       /**< used as a loop counter*/
};


/******************************************************************************
 * @brief   Output BINARY format types
 *****************************************************************************/
enum
{
    OUT_TYPE_DEFAULT,  /**< Default data type */
    OUT_TYPE_CHAR,     /**< char */
    OUT_TYPE_SINT,     /**< short int */
    OUT_TYPE_USINT,    /**< unsigned short int */
    OUT_TYPE_INT,      /**< int */
    OUT_TYPE_FLOAT,    /**< single-precision floating point */
    OUT_TYPE_DOUBLE    /**< double-precision floating point */
};

/******************************************************************************
 * @brief   Output aggregation method types
 *****************************************************************************/
enum
{
    AGG_TYPE_DEFAULT, /**< Default aggregation type */
    AGG_TYPE_AVG,     /**< average over agg interval */
    AGG_TYPE_BEG,     /**< value at beginning of agg interval */
    AGG_TYPE_END,     /**< value at end of agg interval */
    AGG_TYPE_MAX,     /**< maximum value over agg interval */
    AGG_TYPE_MIN,     /**< minimum value over agg interval */
    AGG_TYPE_SUM      /**< sum over agg interval */
};

/******************************************************************************
 * @brief   Frequency flags for raising alarms/flags
 *****************************************************************************/
enum
{
    FREQ_NEVER,      /**< Flag for never raising alarm */
    FREQ_NSTEPS,     /**< Flag for raising alarm every nsteps */
    FREQ_NSECONDS,   /**< Flag for raising alarm every nseconds */
    FREQ_NMINUTES,   /**< Flag for raising alarm every nminutes */
    FREQ_NHOURS,     /**< Flag for raising alarm every nhours */
    FREQ_NDAYS,      /**< Flag for raising alarm every ndays */
    FREQ_NMONTHS,    /**< Flag for raising alarm every nmonths */
    FREQ_NYEARS,     /**< Flag for raising alarm every nyears */
    FREQ_DATE,       /**< Flag for raising alarm on a specific date */
    FREQ_END         /**< Flag for raising alarm at the end of a simulation */
};

/******************************************************************************
 * @brief   Codes for displaying version information
 *****************************************************************************/
enum
{
    DISP_VERSION,
    DISP_COMPILE_TIME,
    DISP_ALL
};

/******************************************************************************
 * @brief   Codes for calendar option.
 *****************************************************************************/
enum calendars
{
    CALENDAR_STANDARD,
    CALENDAR_GREGORIAN,
    CALENDAR_PROLEPTIC_GREGORIAN,
    CALENDAR_NOLEAP,
    CALENDAR_365_DAY,
    CALENDAR_360_DAY,
    CALENDAR_JULIAN,
    CALENDAR_ALL_LEAP,
    CALENDAR_366_DAY
};

/******************************************************************************
 * @brief   Codes for time units option.
 *****************************************************************************/
enum time_units
{
    TIME_UNITS_SECONDS,
    TIME_UNITS_MINUTES,
    TIME_UNITS_HOURS,
    TIME_UNITS_DAYS
};

/******************************************************************************
 * @brief   Codes for timers
 *****************************************************************************/
enum timers
{
    TIMER_VIC_ALL,
    TIMER_VIC_INIT,
    TIMER_VIC_RUN,
    TIMER_VIC_FINAL,
    TIMER_VIC_FORCE,
    TIMER_VIC_WRITE,
    N_TIMERS
};

/******************************************************************************
 * @brief    Stores forcing file input information.
 *****************************************************************************/
typedef struct {
    size_t N_ELEM; /**< number of elements per record; for LAI and ALBEDO,
                        1 element per veg tile; for others N_ELEM = 1; */
    bool SIGNED;
    bool SUPPLIED;
    double multiplier;
    char varname[MAXSTRING];
} force_type_struct;

/******************************************************************************
 * @brief    This structure records the parameters set by the forcing file
             input routines.  Those filled, are used to estimate the paramters
             needed for the model run in initialize_atmos.c.
 *****************************************************************************/
typedef struct {
    force_type_struct TYPE[N_FORCING_TYPES];
    double FORCE_DT[N_FORCING_TYPES];    /**< forcing file time step */
    size_t force_steps_per_day[N_FORCING_TYPES];    /**< forcing file timesteps per day */
    unsigned short int FORCE_ENDIAN[N_FORCING_TYPES];  /**< endian-ness of input file, used for
                                            DAILY_BINARY format */
    int FORCE_FORMAT[N_FORCING_TYPES];            /**< ASCII or BINARY */
    int FORCE_INDEX[N_FORCING_TYPES][1];
    size_t N_TYPES[N_FORCING_TYPES];
} param_set_struct;

/******************************************************************************
 * @brief   This structure stores alarm information
 *****************************************************************************/
typedef struct {
    unsigned int count;  /**< current alarm count */
    dmy_struct next_dmy; /**< next dmy to raise alarm at */
    int next_count;      /**< next count to raise alarm at */
    unsigned int freq;   /**< enum value to describing alarm frequency */
    int n;               /**< variable that provides additional information with respect to alarm_freq */
    bool is_subdaily;    /**< flag denoting if alarm will be raised more than once per day */
} alarm_struct;

/******************************************************************************
 * @brief   This structure stores output information for one output stream.
 *****************************************************************************/
typedef struct {
    size_t nvars;                    /**< number of variables to store in the file */
    size_t ngridcells;               /**< number of grid cells in aggdata */
    dmy_struct time_bounds[2];       /**< timestep bounds of stream */
    char prefix[MAXSTRING];          /**< prefix of the file name, e.g. "fluxes" */
    char filename[MAXSTRING];        /**< complete file name */
    FILE *fh;                        /**< filehandle */
    unsigned short int file_format;  /**< output file format */
    short int compress;              /**< Compress output files in stream*/
    unsigned short int *type;        /**< type, when written to a binary file;
                                          OUT_TYPE_USINT  = unsigned short int
                                          OUT_TYPE_SINT   = short int
                                          OUT_TYPE_FLOAT  = single precision floating point
                                          OUT_TYPE_DOUBLE = double precision floating point */
    double *mult;                    /**< multiplier, when written to a binary file [shape=(nvars, )] */
    char **format;                    /**< format, when written to disk [shape=(nvars, )] */
    unsigned int *varid;             /**< id numbers of the variables to store in the file
                                          (a variable's id number is its index in the out_data array).
                                          The order of the id numbers in the varid array
                                          is the order in which the variables will be written. */
    unsigned short int *aggtype;     /**< type of aggregation to use [shape=(nvars, )] */
    double ****aggdata;              /**< array of aggregated data values [shape=(ngridcells, nvars, nelem, nbins)] */
    alarm_struct agg_alarm;          /**< alaram for stream aggregation */
    alarm_struct write_alarm;        /**< alaram for controlling stream write */
} stream_struct;

/******************************************************************************
 * @brief   This structure stores moisture state information for differencing
 *          with next time step.
 *****************************************************************************/
typedef struct {
    double total_moist_storage;   /**< total moisture storage [mm] */
    double total_soil_moist;      /**< total column soil moisture [mm] */
    double surfstor;              /**< surface water storage [mm] */
    double swe;                   /**< snow water equivalent [mm] */
    double wdew;                  /**< canopy interception [mm] */
} save_data_struct;

/******************************************************************************
 * @brief   This structure stores metadata for individual variables
 *****************************************************************************/
typedef struct {
    char varname[MAXSTRING];  /**< name of variable */
    char long_name[MAXSTRING];  /**< name of variable */
    char standard_name[MAXSTRING];  /**< cf long_name of variable */
    char units[MAXSTRING];  /**< units of variable */
    char description[MAXSTRING];  /**< descripition of variable */
    size_t nelem;          /**< number of data values */
} metadata_struct;

/******************************************************************************
 * @brief   This structure holds all variables needed for the error handling
 *          routines.
 *****************************************************************************/
typedef struct {
    force_data_struct *force;
    double dt;
    energy_bal_struct *energy;
    size_t rec;
    double **out_data;
    stream_struct *output_streams;
    snow_data_struct *snow;
    soil_con_struct soil_con;
    veg_con_struct *veg_con;
    veg_var_struct *veg_var;
} Error_struct;

/******************************************************************************
 * @brief   This structure holds timer information for profiling
 *****************************************************************************/
typedef struct {
    double start_wall;
    double start_cpu;
    double stop_wall;
    double stop_cpu;
    double delta_wall;
    double delta_cpu;
} timer_struct;

/******************************************************************************
 * @brief    Structure to store location information for individual grid cells.
 * @details  The global and local indices show the position of the grid cell
 *           within the global and local (processor) domains. When the model is
 *           run on a single processor, the glonal and local domains are
 *           identical. The model is run over a list of cells.
 *****************************************************************************/
typedef struct {
    bool run; /**< TRUE: run grid cell. FALSE: do not run grid cell */
    double latitude; /**< latitude of grid cell center */
    double longitude; /**< longitude of grid cell center */
    double area; /**< area of grid cell */
    double frac; /**< fraction of grid cell that is active */
    size_t nveg; /**< number of vegetation type according to parameter file */
    size_t global_idx; /**< index of grid cell in global list of grid cells */
    size_t io_idx; /**< index of cell in 1-D I/O arrays */
    size_t local_idx; /**< index of grid cell in local list of grid cells */
} location_struct;

/******************************************************************************
 * @brief    Structure to store information about the domain file.
 *****************************************************************************/
typedef struct {
    char lat_var[MAXSTRING]; /**< latitude variable name in the domain file */
    char lon_var[MAXSTRING];  /**< longitude variable name in the domain file */
    char mask_var[MAXSTRING]; /**< mask variable name in the domain file */
    char area_var[MAXSTRING]; /**< area variable name in the domain file */
    char frac_var[MAXSTRING]; /**< fraction variable name in the domain file */
    char y_dim[MAXSTRING]; /**< y dimension name in the domain file */
    char x_dim[MAXSTRING]; /**< x dimension name in the domain file */
    size_t n_coord_dims; /**< number of x/y coordinates */
} domain_info_struct;

/******************************************************************************
 * @brief    Structure to store local and global domain information. If the
 *           model is run on a single processor, then the two are identical.
 *****************************************************************************/
typedef struct {
    size_t ncells_total; /**< total number of grid cells on domain */
    size_t ncells_active; /**< number of active grid cells on domain */
    size_t n_nx; /**< size of x-index; */
    size_t n_ny; /**< size of y-index */
    location_struct *locations; /**< locations structs for local domain */
    domain_info_struct info; /**< structure storing domain file info */
} domain_struct;

/******************************************************************************
 * @brief    Structure for netcdf variable information
 *****************************************************************************/
typedef struct {
    int nc_varid;                   /**< variable netcdf id */
    int nc_type;                    /**< variable netcdf type */
    int nc_dimids[MAXDIMS];         /**< ids of dimensions */
    size_t nc_counts[MAXDIMS];      /**< size of dimid */
    size_t nc_dims;                 /**< number of dimensions */
} nc_var_struct;

/******************************************************************************
 * @brief    Structure for netcdf file information. Initially to store
 *           information for the output files (state and history)
 *****************************************************************************/
typedef struct {
    char c_fillvalue;
    int i_fillvalue;
    double d_fillvalue;
    float f_fillvalue;
    short int s_fillvalue;
    int nc_id;
    int band_dimid;
    int front_dimid;
    int frost_dimid;
    int lake_node_dimid;
    int layer_dimid;
    int ni_dimid;
    int nj_dimid;
    int node_dimid;
    int outlet_dimid;
    int routing_timestep_dimid;
    int root_zone_dimid;
    int time_dimid;
    int time_bounds_dimid;
    int veg_dimid;
    int time_varid;
    int time_bounds_varid;
    size_t band_size;
    size_t front_size;
    size_t frost_size;
    size_t lake_node_size;
    size_t layer_size;
    size_t ni_size;
    size_t nj_size;
    size_t node_size;
    size_t outlet_size;
    size_t routing_timestep_size;
    size_t root_zone_size;
    size_t time_size;
    size_t veg_size;
    
    int sector_dimid;
    int dam_dimid;
    size_t sector_size;
    size_t dam_size;
    
    bool open;
    nc_var_struct *nc_vars;
} nc_file_struct;

/******************************************************************************
 * @brief    Structure for mapping the vegetation types for each grid cell as
 *           stored in VIC's veg_con_struct to a regular array.
 *****************************************************************************/
typedef struct {
    size_t nv_types; /**< total number of vegetation types */
                     /**< size of vidx and Cv arrays */
    size_t nv_active; /**< number of active vegetation types. Because of the */
                      /**< way that VIC defines nveg, this is nveg+1 */
                      /**< (for bare soil) or nveg+2 (if the treeline option */
                      /**< is active as well) */
    int *vidx;     /**< array of indices for active vegetation types */
    double *Cv;    /**< array of fractional coverage for nc_types */
} veg_con_map_struct;

/******************************************************************************
 * @brief   file structures
 *****************************************************************************/
typedef struct {
    FILE *globalparam;  /**< global parameters file */
    FILE *constants;    /**< model constants parameter file */
    FILE *logfile;      /**< log file */
} filep_struct;

/******************************************************************************
 * @brief   This structure stores input and output filenames.
 *****************************************************************************/
typedef struct {
    nameid_struct forcing[N_FORCING_TYPES];  /**< atmospheric forcing files */
    char f_path_pfx[N_FORCING_TYPES][MAXSTRING]; /**< path and prefix for
                                                    atmospheric forcing files */
    char global[MAXSTRING];     /**< global control file name */
    nameid_struct domain;       /**< domain file name and nc_id*/
    char constants[MAXSTRING];  /**< model constants file name */
    nameid_struct params;       /**< model parameters file name and nc_id */
    nameid_struct init_state;   /**< initial model state file name and nc_id */
    char result_dir[MAXSTRING]; /**< result directory */
    char statefile[MAXSTRING];  /**< name of model state file */
    char log_path[MAXSTRING];   /**< Location to write log file to */
    
    // Plugins
    nameid_struct groundwater;
    nameid_struct routing;
    nameid_struct rout_params;  /**< routing parameters file name and nc_id */
    char water_use_forcing_pfx[MAXSTRING];
    nameid_struct water_use;
    nameid_struct irrigation;
    nameid_struct dams;
    
} filenames_struct;

double air_density(double t, double p);
void agg_stream_data(stream_struct *stream, dmy_struct *dmy_current,
                     double ***out_data);
double all_30_day_from_dmy(dmy_struct *dmy);
double all_leap_from_dmy(dmy_struct *dmy);
void alloc_aggdata(stream_struct *stream);
void alloc_out_data(size_t ngridcells, double ***out_data);
double average(double *ar, size_t n);
double calc_energy_balance_error(double, double, double, double, double);
void calc_root_fractions(veg_con_struct *veg_con, soil_con_struct *soil_con);
double calc_water_balance_error(double, double, double, double);
bool cell_method_from_agg_type(unsigned short int aggtype, char cell_method[]);
bool check_write_flag(int rec);
void collect_eb_terms(energy_bal_struct, snow_data_struct, cell_data_struct,
                      double, double, double, bool, bool, double, bool, int,
                      double *, double, double **);
void collect_wb_terms(cell_data_struct, veg_var_struct, snow_data_struct,
                      double, double, double, bool, double, bool, double *,
                      double **);
void compute_derived_state_vars(all_vars_struct *, soil_con_struct *,
                                veg_con_struct *);
void compute_lake_params(lake_con_struct *, soil_con_struct);
void compute_treeline(force_data_struct *, dmy_struct *, double, double *,
                      bool *);
size_t count_force_vars(FILE *gp);
void count_nstreams_nvars(FILE *gp, size_t *nstreams, size_t nvars[]);
void cmd_proc(int argc, char **argv, char *globalfilename);
void compress_files(char string[], short int level);
stream_struct create_outstream(stream_struct *output_streams);
double get_cpu_time();
void get_current_datetime(char *cdt);
double get_wall_time();
double date2num(double origin, dmy_struct *date, double tzoffset,
                unsigned short int calendar, unsigned short int time_units);
void dmy_all_30_day(double julian, dmy_struct *dmy);
void dmy_all_leap(double julian, dmy_struct *dmy);
bool dmy_equal(dmy_struct *a, dmy_struct *b);
void dmy_julian_day(double julian, unsigned short int calendar,
                    dmy_struct *dmy);
void dmy_no_leap_day(double julian, dmy_struct *dmy);
void dt_seconds_to_time_units(unsigned short int time_units, double dt_seconds,
                              double *dt_time_units);
void display_current_settings(int);
double fractional_day_from_dmy(dmy_struct *dmy);
void free_all_vars(all_vars_struct *all_vars, int Nveg);
void free_dmy(dmy_struct **dmy);
void free_out_data(size_t ngridcells, double ***out_data);
void free_streams(stream_struct **streams);
void free_vegcon(veg_con_struct **veg_con);
void generate_default_state(all_vars_struct *, soil_con_struct *,
                            veg_con_struct *, dmy_struct *);
void generate_default_lake_state(all_vars_struct *, soil_con_struct *,
                                 lake_con_struct);
void get_default_nstreams_nvars(size_t *nstreams, size_t nvars[]);
void get_parameters(FILE *paramfile);
void init_output_list(double **out_data, int write, char *format, int type,
                      double mult);
void initialize_energy(energy_bal_struct **energy, size_t nveg);
void initialize_global(void);
void initialize_options(void);
void initialize_parameters(void);
void initialize_param_set(void);
void initialize_save_data(all_vars_struct *all_vars, force_data_struct *force,
                          soil_con_struct *soil_con, veg_con_struct *veg_con,
                          veg_lib_struct *veg_lib, lake_con_struct *lake_con,
                          double **out_data, save_data_struct *save_data,
                          timer_struct *timer);
void initialize_snow(snow_data_struct **snow, size_t veg_num);
void initialize_soil(cell_data_struct **cell, size_t veg_num);
void initialize_time(void);
void initialize_veg(veg_var_struct **veg_var, size_t nveg);
double julian_day_from_dmy(dmy_struct *dmy, unsigned short int calendar);
bool leap_year(unsigned short int year, unsigned short int calendar);
all_vars_struct make_all_vars(size_t nveg);
cell_data_struct **make_cell_data(size_t veg_type_num);
dmy_struct *make_dmy(global_param_struct *global);
energy_bal_struct **make_energy_bal(size_t nveg);
void make_lastday(unsigned short int calendar, unsigned short int year,
                  unsigned short int lastday[]);
snow_data_struct **make_snow_data(size_t nveg);
veg_var_struct **make_veg_var(size_t veg_type_num);
double no_leap_day_from_dmy(dmy_struct *dmy);
void num2date(double origin, double time_value, double tzoffset,
              unsigned short int calendar, unsigned short int time_units,
              dmy_struct *date);
FILE *open_file(char string[], char type[]);
void parse_nc_time_units(char *nc_unit_chars, unsigned short int *units,
                         dmy_struct *dmy);
void put_data(all_vars_struct *, force_data_struct *, soil_con_struct *,
              veg_con_struct *, veg_lib_struct *veg_lib, lake_con_struct *,
              double **out_data, save_data_struct *, timer_struct *timer);
void print_alarm(alarm_struct *alarm);
void print_cell_data(cell_data_struct *cell, size_t nlayers, size_t nfrost);
void print_dmy(dmy_struct *dmy);
void print_energy_bal(energy_bal_struct *eb, size_t nnodes, size_t nfronts);
void print_force_type(force_type_struct *force_type);
void print_global_param(global_param_struct *gp);
void print_lake_con(lake_con_struct *lcon, size_t nlnodes);
void print_lake_var(lake_var_struct *lvar, size_t nlnodes, size_t nfronts,
                    size_t nlayers, size_t nnodes, size_t nfrost);
void print_layer_data_states(layer_data_struct *ldata, size_t nfrost);
void print_layer_data_fluxes(layer_data_struct *ldata);
void print_license(void);
void print_option(option_struct *option);
void print_out_data(double **out_data, metadata_struct *metadata);
void print_out_metadata(metadata_struct *metadata, size_t nvars);
void print_output_streams(stream_struct *outf);
void print_param_set(param_set_struct *param_set);
void print_parameters(parameters_struct *param);
void print_save_data(save_data_struct *save);
void print_snow_data(snow_data_struct *snow);
void print_soil_con(soil_con_struct *scon, size_t nlayers, size_t nnodes,
                    size_t nfrost, size_t nbands, size_t nzwt);
void print_stream(stream_struct *stream, metadata_struct *metadata);
void print_veg_con(veg_con_struct *vcon, size_t nroots, char blowing, char lake,
                   char carbon, size_t ncanopy);
void print_veg_lib(veg_lib_struct *vlib, char carbon);
void print_veg_var(veg_var_struct *vvar, size_t ncanopy);
void print_version(char *);
void print_usage(char *);
double q_to_vp(double q, double p);
bool raise_alarm(alarm_struct *alarm, dmy_struct *dmy_current);
void reset_alarm(alarm_struct *alarm, dmy_struct *dmy_current);
void reset_stream(stream_struct *stream, dmy_struct *dmy_current);
void set_output_var(stream_struct *stream, char *varname, size_t varnum,
                    char *format, unsigned short int type, double mult,
                    unsigned short int aggtype);
unsigned int get_default_outvar_aggtype(unsigned int varid);
void set_alarm(dmy_struct *dmy_current, unsigned int freq, void *value,
               alarm_struct *alarm);
void set_output_defaults(stream_struct **output_streams,
                         dmy_struct     *dmy_current,
                         unsigned short  default_file_format);
void set_output_met_data_info();
void setup_stream(stream_struct *stream, size_t nvars, size_t ngridcells);
void soil_moisture_from_water_table(soil_con_struct *soil_con, size_t nlayers);
void sprint_dmy(char *str, dmy_struct *dmy);
void str_from_calendar(unsigned short int calendar, char *calendar_str);
void str_from_time_units(unsigned short int time_units, char *unit_str);
unsigned short int str_to_agg_type(char aggstr[]);
void str_to_ascii_format(char *format);
bool str_to_bool(char str[]);
unsigned short int str_to_calendar(char *cal_chars);
unsigned short int str_to_freq_flag(char freq[]);
double str_to_out_mult(char multstr[]);
unsigned short int str_to_out_type(char typestr[]);
unsigned short int str_to_timeunits(char units_chars[]);
void strpdmy(const char *s, const char *format, dmy_struct *dmy);
double time_delta(dmy_struct *dmy_current, unsigned short int freq, int n);
bool between_dmy(dmy_struct, dmy_struct, dmy_struct);
double between_jday(double, double, double);
void timer_continue(timer_struct *t);
void timer_init(timer_struct *t);
void timer_start(timer_struct *t);
void timer_stop(timer_struct *t);
int update_step_vars(all_vars_struct *, veg_con_struct *, veg_hist_struct *);
int invalid_date(unsigned short int calendar, dmy_struct *dmy);
void validate_parameters(void);
void validate_streams(stream_struct **stream);
char will_it_snow(double *t, double t_offset, double max_snow_temp,
                  double *prcp, size_t n);
void zero_output_list(double **);

void add_nveg_to_global_domain(nameid_struct *nc_nameid,
                               domain_struct *global_domain);
void alloc_force(force_data_struct *force);
void alloc_veg_hist(veg_hist_struct *veg_hist);
double air_density(double t, double p);
double average(double *ar, size_t n);
void check_init_state_file(void);
void compare_ncdomain_with_global_domain(nameid_struct *nc_nameid);
void free_force(force_data_struct *force);
void free_veg_hist(veg_hist_struct *veg_hist);
void get_domain_type(char *cmdstr);
size_t get_global_domain(nameid_struct *domain_nc_nameid,
                         nameid_struct *param_nc_nameid,
                         domain_struct *global_domain);
void copy_domain_info(domain_struct *domain_from, domain_struct *domain_to);
void get_nc_latlon(nameid_struct *nc_nameid, domain_struct *nc_domain);
size_t get_nc_dimension(nameid_struct *nc_nameid, char *dim_name);
void get_nc_var_attr(nameid_struct *nc_nameid, char *var_name, char *attr_name,
                     char **attr);
int get_nc_var_type(nameid_struct *nc_nameid, char *var_name);
int get_nc_varndimensions(nameid_struct *nc_nameid, char *var_name);
int get_nc_field_double(nameid_struct *nc_nameid, char *var_name, size_t *start,
                        size_t *count, double *var);
int get_nc_field_float(nameid_struct *nc_nameid, char *var_name, size_t *start,
                       size_t *count, float *var);
int get_nc_field_int(nameid_struct *nc_nameid, char *var_name, size_t *start,
                     size_t *count, int *var);
int get_nc_dtype(unsigned short int dtype);
int get_nc_mode(unsigned short int format);
void initialize_domain(domain_struct *domain);
void initialize_domain_info(domain_info_struct *info);
void initialize_filenames(void);
void initialize_fileps(void);
void initialize_global_structures(void);
void initialize_history_file(nc_file_struct *nc, stream_struct *stream);
void initialize_state_file(char *filename, nc_file_struct *nc_state_file,
                           dmy_struct *dmy_state);
void initialize_location(location_struct *location);
int initialize_model_state(all_vars_struct *all_vars, size_t Nveg,
                           size_t Nnodes, double surf_temp,
                           soil_con_struct *soil_con, veg_con_struct *veg_con);
void initialize_nc_file(nc_file_struct *nc_file, size_t nvars,
                        unsigned int *varids, unsigned short int *dtypes);
void initialize_soil_con(soil_con_struct *soil_con);
void initialize_veg_con(veg_con_struct *veg_con);
void parse_output_info(FILE *gp, stream_struct **output_streams,
                       dmy_struct *dmy_current);
void print_force_data(force_data_struct *force);
void print_domain(domain_struct *domain, bool print_loc);
void print_location(location_struct *location);
void print_nc_file(nc_file_struct *nc);
void print_nc_var(nc_var_struct *nc_var);
void print_veg_con_map(veg_con_map_struct *veg_con_map);
void put_nc_attr(int nc_id, int var_id, const char *name, const char *value);
void set_force_type(char *cmdstr);
void set_global_nc_attributes(int ncid, unsigned short int file_type);
void set_state_meta_data_info();
void set_nc_var_dimids(unsigned int varid, nc_file_struct *nc_hist_file,
                       nc_var_struct *nc_var);
void set_nc_var_info(unsigned int varid, unsigned short int dtype,
                     nc_file_struct *nc_hist_file, nc_var_struct *nc_var);
void set_nc_state_file_info(nc_file_struct *nc_state_file);
void set_nc_state_var_info(nc_file_struct *nc_state_file);
void sprint_location(char *str, location_struct *loc);

// allocation
void vic_alloc(void);
void alloc_general(void);

// initialisation
void vic_init(void);
void init_general(void);

// finalize
void vic_finalize(void);
void finalize_general(void);

void vic_image_run(dmy_struct *dmy_current);
void vic_init_output(dmy_struct *dmy_current);
void vic_restore(void);
void vic_start(void);
void vic_store(dmy_struct *dmy_state, char *state_filename);
void vic_write(stream_struct *stream, nc_file_struct *nc_hist_file,
               dmy_struct *dmy_current);
void vic_write_output(dmy_struct *dmy);
void write_vic_timing_table(timer_struct *timers, char *driver);
void get_global_param(FILE *);

bool check_save_state_flag(size_t, dmy_struct *dmy_offset);
void display_current_settings(int);
void get_forcing_file_info(param_set_struct *param_set, size_t file_num);
void vic_force(void);
void vic_populate_model_state(dmy_struct *dmy_current);

void cshift(double *, int, int, int, int);

//TODO MOVE!! (WF)
void dam_set_nc_output_file_info(nc_file_struct *nc_output_file);
void dam_write_def_dim(nc_file_struct *nc_file, stream_struct *stream);
bool dam_set_nc_var_dimids(int varid, nc_file_struct *nc_file, nc_var_struct *nc_var);
bool dam_set_nc_var_info(int varid, unsigned short int dtype, nc_file_struct *nc_file, nc_var_struct *nc_var);
void wu_set_nc_output_file_info(nc_file_struct *nc_output_file);
void wu_write_def_dim(nc_file_struct *nc_file, stream_struct *stream);
bool wu_set_nc_var_dimids(int varid, nc_file_struct *nc_file, nc_var_struct *nc_var);
bool wu_set_nc_var_info(int varid, unsigned short int dtype, nc_file_struct *nc_file, nc_var_struct *nc_var);

#endif
