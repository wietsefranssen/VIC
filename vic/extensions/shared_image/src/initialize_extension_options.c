#include <ext_driver_shared_image.h>

/******************************************************************************
 * @brief    Initialize all options before they are called by the
 *           model.
 *****************************************************************************/
void
initialize_extension_options()
{
    extern ext_option_struct options;

    /** Initialize model option flags **/

    // simulation modes
    options.ROUTING=false;
    options.DAMS=false;
}
