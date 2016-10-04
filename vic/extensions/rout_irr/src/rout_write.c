/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vic_def.h>
#include <rout.h>

void rout_write(void){
    extern rout_options_struct rout_options;
    
    if(!rout_options.routing){
        return;
    }
}