#define _POSIX_C_SOURCE 200809L
#include "platform_utils.h"
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// --- Platform-specific includes ---
#ifdef _WIN32
#include <shlobj.h> // For SHGetFolderPathA
#include <windows.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

// --- Implementation for POSIX (Linux, macOS) ---
#ifndef _WIN32

// Helper function for POSIX systems to get XDG base directories
static char *get_xdg_path(const char *env_var, const char *fallback_path) {
  const char *env_val = getenv(env_var);
  char *base_path = NULL;

  if (env_val && env_val[0]) {
    base_path = strdup(env_val);
  } else {
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
      return NULL; // Cannot determine home directory
    }
    base_path = path_join(home_dir, fallback_path);
  }

  if (!base_path) {
    return NULL;
  }

  char *ordo_path = path_join(base_path, "ordo");
  free(base_path);

  return ordo_path;
}

char *platform_get_config_dir(void) {
  return get_xdg_path("XDG_CONFIG_HOME", ".config");
}

char *platform_get_data_dir(void) {
  return get_xdg_path("XDG_DATA_HOME", ".local/share");
}

bool platform_create_dir_recursive(const char *path) {
  char *p = strdup(path);
  if (!p)
    return false;

  char *sep = p;
  while ((sep = strchr(sep + 1, '/')) != NULL) {
    *sep = '\0';
    if (mkdir(p, 0700) != 0 && errno != EEXIST) {
      free(p);
      return false;
    }
    *sep = '/';
  }

  if (mkdir(p, 0755) != 0 && errno != EEXIST) {
    free(p);
    return false;
  }

  free(p);
  return true;
}

FileList platform_list_files_in_dir(const char *dir_path,
                                    const char *extension) {
  FileList file_list = {.files = NULL, .count = 0};
  DIR *dir = opendir(dir_path);
  if (!dir) {
    return file_list;
  }

  int capacity = 10; // Initial capacity
  file_list.files = malloc(capacity * sizeof(char *));
  if (!file_list.files) {
    closedir(dir);
    return file_list;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strstr(entry->d_name, extension)) {
      if (file_list.count >= capacity) {
        capacity *= 2; // Double the capacity
        char **new_files = realloc(file_list.files, capacity * sizeof(char *));
        if (!new_files) {
          platform_free_file_list(&file_list);
          closedir(dir);
          return (FileList){.files = NULL, .count = 0};
        }
        file_list.files = new_files;
      }
      file_list.files[file_list.count++] = strdup(entry->d_name);
    }
  }
  closedir(dir);
  return file_list;
}

#else // --- Implementation for Windows ---

char *platform_get_config_dir(void) {
  char path[MAX_PATH];
  // CSIDL_APPDATA corresponds to %APPDATA%
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
    char *ordo_path = path_join(path, "ordo");
    return ordo_path;
  }
  return NULL;
}

char *platform_get_data_dir(void) {
  char path[MAX_PATH];
  // CSIDL_LOCAL_APPDATA corresponds to %LOCALAPPDATA%
  if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
    char *ordo_path = path_join(path, "ordo");
    return ordo_path;
  }
  return NULL;
}

bool platform_create_dir_recursive(const char *path) {
  char *p = strdup(path);
  if (!p)
    return false;

  char *current = p;
  while (*current) {
    if (*current == '\\' || *current == '/') {
      *current = '\0';
      if (!CreateDirectoryA(p, NULL) &&
          GetLastError() != ERROR_ALREADY_EXISTS) {
        free(p);
        return false;
      }
      *current = '\\';
    }
    current++;
  }

  if (!CreateDirectoryA(p, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
    free(p);
    return false;
  }

  free(p);
  return true;
}

FileList platform_list_files_in_dir(const char *dir_path,
                                    const char *extension) {
  FileList file_list = {.files = NULL, .count = 0};
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s\\*%s", dir_path, extension);

  int capacity = 10;
  file_list.files = malloc(capacity * sizeof(char *));
  if (!file_list.files) {
    return file_list;
  }

  WIN32_FIND_DATAA find_data;
  HANDLE h_find = FindFirstFileA(search_path, &find_data);

  if (h_find == INVALID_HANDLE_VALUE) {
    free(file_list.files);
    file_list.files = NULL;
    return file_list;
  }

  do {
    if (file_list.count >= capacity) {
      capacity *= 2;
      char **new_files = realloc(file_list.files, capacity * sizeof(char *));
      if (!new_files) {
        platform_free_file_list(&file_list);
        FindClose(h_find);
        return (FileList){.files = NULL, .count = 0};
      }
      file_list.files = new_files;
    }
    file_list.files[file_list.count++] = strdup(find_data.cFileName);
  } while (FindNextFileA(h_find, &find_data) != 0);

  FindClose(h_find);
  return file_list;
}

#endif

// --- Common Function ---

void platform_free_file_list(FileList *file_list) {
  if (!file_list)
    return;
  for (int i = 0; i < file_list->count; i++) {
    free(file_list->files[i]);
  }
  free(file_list->files);
  file_list->files = NULL;
  file_list->count = 0;
}
