#include <vic.h>

void
dam_set_nc_output_file_info(nc_file_struct *nc_output_file)
{
    extern option_struct options;

    nc_output_file->dam_dimid = MISSING;
    nc_output_file->dam_size = options.MAXDAMS;
}

void
dam_write_def_dim(nc_file_struct *nc_file,
                  stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc_file->nc_id, "dam", nc_file->dam_size,
                        &(nc_file->dam_dimid));
    check_nc_status(status, "Error defining dam dimension in %s",
                    stream->filename);
}

void
dam_write_def_dimvar(nc_file_struct *nc_file,
                     stream_struct  *stream)
{
}

void
dam_write_put_dimvar(nc_file_struct *nc_file,
                     stream_struct  *stream)
{
}
