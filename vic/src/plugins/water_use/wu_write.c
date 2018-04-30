#include <vic.h>

void
wu_set_nc_output_file_info(nc_file_struct *nc_output_file)
{
    nc_output_file->sector_dimid = MISSING;
    nc_output_file->sector_size = WU_NSECTORS;
}

void
wu_write_def_dim(nc_file_struct *nc_file,
                 stream_struct  *stream)
{
    int status;

    status = nc_def_dim(nc_file->nc_id, "sector", nc_file->sector_size,
                        &(nc_file->sector_dimid));
    check_nc_status(status, "Error defining sector dimension in %s",
                    stream->filename);
}

void
wu_write_def_dimvar(nc_file_struct *nc_file,
                    stream_struct  *stream)
{
}

void
wu_write_put_dimvar(nc_file_struct *nc_file,
                    stream_struct  *stream)
{
}
