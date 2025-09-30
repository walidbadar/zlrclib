/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app/zlrclib_filesystem.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zlrclib_filesystem);

/* TODO: Implement dynamic storage dev selection */
#ifdef CONFIG_FS_LITTLEFS_BLK_DEV
#if defined(CONFIG_DISK_DRIVER_SDMMC)
#define DISK_NAME "SD"
#elif defined(CONFIG_DISK_DRIVER_MMC)
#define DISK_NAME "SD2"
#else
#retor "No disk device defined, is your board supported?"
#endif

FS_LITTLEFS_DECLARE_CUSTOM_CONFIG(
	lfs_data,
	CONFIG_SDHC_BUFFER_ALIGNMENT,
	SDMMC_DEFAULT_BLOCK_SIZE,
	SDMMC_DEFAULT_BLOCK_SIZE,
	SDMMC_DEFAULT_BLOCK_SIZE,
	2 * SDMMC_DEFAULT_BLOCK_SIZE);

static struct fs_mount_t lfs_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &lfs_data,
	.flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
	.storage_dev = DISK_NAME,
};
#else
#include <zephyr/storage/flash_map.h>

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(lfs_data);
static struct fs_mount_t lfs_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &lfs_data,
	.storage_dev = (void *)FIXED_PARTITION_ID(storage_partition),
	.mnt_point = "/root",
};
#endif

#define MAX_PATH_LEN 128
#define MAX_FILENAME_LEN 128
#define MAX_INPUT_LEN 20

/* Maintenance guarantees this begins with '/' and is NUL-terminated. */
static char cwd[MAX_PATH_LEN] = "/";

static int create_abs_path(const char *file_name, char *path)
{
	int ret;

    if (!file_name || !path) {
        return -EINVAL;
    }

    ret = snprintf(path, MAX_PATH_LEN, "%s/%s", cwd, file_name);

    if (ret < 0) {
        return -errno;
    }

    if (ret >= MAX_PATH_LEN) {
        fprintf(stderr, "Error: Path exceeds maximum length (%d bytes)\n", MAX_PATH_LEN);
        return -ENAMETOOLONG;
    }

    return ret;
}

int zlrclib_cd(char *path)
{
	int ret;

	if (!path || *path == '\0'){
		LOG_ERR("Invalid path");
		return -EINVAL;
	}

	struct fs_dirent entry;

	if (strcmp(path, "..") == 0) {
		char *prev = strrchr(cwd, '/');

		if (!prev || prev == cwd) {
			strcpy(cwd, "/");
		} else {
			*prev = '\0';
		}

		/* No need to test that a parent exists */
		return 0;
	}

	ret = fs_stat(path, &entry);
	if (ret != 0) {
		LOG_ERR("%s doesn't exist", path);
		return -ENOENT;
	}

	if (entry.type != FS_DIR_ENTRY_DIR) {
		LOG_ERR("%s is not a directory", path);
		return -ENOTDIR;
	}

	strncpy(cwd, path, sizeof(cwd));
	cwd[sizeof(cwd) - 1] = '\0';

	return 0;
}

int zlrclib_ls(char *path)
{
	int ret;

	if (!path || *path == '\0'){
		LOG_ERR("Invalid path");
		return -EINVAL;
	}
	
	struct fs_dir_t dir;
	fs_dir_t_init(&dir);

	ret = fs_opendir(&dir, path);
	if (ret != 0) {
		LOG_ERR("Unable to open %s (ret %d)", path, ret);
		return -EIO;
	}

	while (1) {
		struct fs_dirent entry;
		const char *name_end;

		ret = fs_readdir(&dir, &entry);
		if (ret != 0) {
			LOG_ERR("Unable to read directory");
			break;
		}

		/* Check for end of directory listing */
		if (entry.name[0] == '\0') {
			break;
		}

		name_end = (entry.type == FS_DIR_ENTRY_DIR) ? "/" : "";
		LOG_INF("%s%s", entry.name, name_end);
	}

	fs_closedir(&dir);

	return 0;
}

int zlrclib_pwd(char *path)
{
	if (!path){
		LOG_ERR("Invalid path");
		return -EINVAL;
	}

	LOG_DBG("%s", cwd);
	memset(path, 0, strlen(path));
	memcpy(path, cwd, strlen(cwd));

	return 0;
}

int zlrclib_fwrite(char *file_name, char *buf)
{
	int ret;
	struct fs_file_t file;
	char path[MAX_PATH_LEN];

	if (!file_name && !buf){
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	create_abs_path(file_name, path);

	fs_file_t_init(&file);

	ret = fs_open(&file, path, FS_O_CREATE | FS_O_WRITE);
	if (ret >= 0) {
		fs_write(&file, buf, strlen(buf));
		LOG_INF("Created test file: %s", path);
	} else {
		LOG_ERR("Failed to create test file: %s", path);
	}

	fs_close(&file);

	return ret;
}

int zlrclib_fread(const char *file_name, char *buf, size_t buf_size)
{
    int ret;
    struct fs_file_t file;
    char path[MAX_PATH_LEN];

    if (!file_name || !buf || buf_size == 0) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }

    create_abs_path(file_name, path);

    fs_file_t_init(&file);

    ret = fs_open(&file, path, FS_O_READ);
    if (ret < 0) {
        LOG_ERR("Failed to open file: %s (%d)", path, ret);
        return ret;
    }

    ssize_t bytes_read = fs_read(&file, buf, buf_size - 1);
    if (bytes_read < 0) {
        LOG_ERR("Failed to read file: %s (%d)", path, (int)bytes_read);
        fs_close(&file);
        return bytes_read;
    }

    buf[bytes_read] = '\0';

    LOG_INF("Read %d bytes from %s", (int)bytes_read, path);

    fs_close(&file);

    return bytes_read;
}

static int zlrclib_mount(void)
{
	int ret;

	/* Mount the filesystem */
	ret = fs_mount(&lfs_mnt);
	if (ret < 0) {
		LOG_ERR("Mount failed: %d", ret);
		return 0;
	}

	zlrclib_cd("/root");

#if 1
	char *file_name = "track";
	char buf[1024];
	char pwd_path[MAX_PATH_LEN] = "";

	zlrclib_cd("/root");

	zlrclib_ls(cwd);

	zlrclib_fread(file_name, buf, sizeof(buf));
	LOG_INF("%s", buf);

	zlrclib_pwd(pwd_path);
	LOG_INF("pwd: %s", pwd_path);
#endif

	return 0;
}

SYS_INIT(zlrclib_mount, APPLICATION, 95);
