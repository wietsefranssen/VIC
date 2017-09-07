#include <ext_driver_shared_image.h>

void
initialize_extension_filenames()
{
    extern ext_filenames_struct ext_filenames;

    strcpy(ext_filenames.routing, "MISSING");
}