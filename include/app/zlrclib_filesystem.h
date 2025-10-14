#ifndef ZLRCLIB_FILESYSTEM_H_
#define ZLRCLIB_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

int zlrclib_cd(uint8_t *path);
int zlrclib_ls(uint8_t *path);
int zlrclib_pwd(uint8_t *path);
int zlrclib_rm(const uint8_t *file_name);
int zlrclib_fwrite(const uint8_t *file_name, uint8_t *buf, size_t buf_len, uint16_t pos);
int zlrclib_fread(const uint8_t *file_name, uint8_t *buf, size_t buf_len, uint16_t pos);

#endif /* ZLRCLIB_FILESYSTEM_H_ */
