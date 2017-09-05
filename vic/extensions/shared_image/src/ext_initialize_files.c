/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <ext_shared_image.h>

void
initialize_extension_filenames()
{
    extern ext_filenames_struct ext_filenames;

    strcpy(ext_filenames.routing, "MISSING");
}