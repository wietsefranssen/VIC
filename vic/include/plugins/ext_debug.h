#ifndef DEBUG_H
#define DEBUG_H

void debug_map_nc_double(char *path, char *var_name, double *data, double fill_value);
void debug_map_nc_sizet(char *path, char *var_name, size_t *data, size_t fill_value);
void debug_map_nc_int(char *path, char *var_name, int *data, int fill_value);
void debug_map_3d_nc_double(char *path, char *var_name, char *dim_name, size_t dim_size, double **data, double fill_value);
void debug_map_3d_nc_sizet(char *path, char *var_name, char *dim_name, size_t dim_size, size_t **data, size_t fill_value);
void debug_map_3d_nc_int(char *path, char *var_name, char *dim_name, size_t dim_size, int **data, int fill_value);

void debug_map_file_double(char *path, double *data);
void debug_map_file_sizet(char *path, size_t *data);
void debug_map_file_int(char *path, int *data);

#endif /* DEBUG_H */

