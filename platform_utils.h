#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <stdbool.h>

// Returns the path to the platform-specific configuration directory.
// The caller is responsible for freeing the returned memory with free().
char *platform_get_config_dir(void);
char *platform_get_data_dir(void);

// Creates a directory and all its parents if they do not exist.
// Returns true on success, false on failure.
bool platform_create_dir_recursive(const char *path);

// Structure to store a list of file names.
typedef struct {
  char **files;
  int count;
} FileList;

// Lists files in a directory that match an extension.
// The caller is responsible for freeing the FileList memory with
// platform_free_file_list.
FileList platform_list_files_in_dir(const char *dir_path,
                                    const char *extension);

// Frees the memory allocated by platform_list_files_in_dir.
void platform_free_file_list(FileList *file_list);

#endif // PLATFORM_UTILS_H
