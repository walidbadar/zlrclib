#ifndef ZLRCLIB_FILESYSTEM_H_
#define ZLRCLIB_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/sd/sd_spec.h>

int zlrclib_cd(char *path);
int zlrclib_ls(char *path);
int zlrclib_pwd(char *path);
int zlrclib_fwrite(char *file_name, char *buf);
int zlrclib_fread(const char *file_name, char *buf, size_t buf_size);

#endif /* ZLRCLIB_FILESYSTEM_H_ */
