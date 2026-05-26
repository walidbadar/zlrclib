/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/sd/sd_spec.h>

#include <zlrclib_filesystem.h>

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

FS_LITTLEFS_DECLARE_CUSTOM_CONFIG(lfs_data, CONFIG_SDHC_BUFFER_ALIGNMENT, SDMMC_DEFAULT_BLOCK_SIZE,
				  SDMMC_DEFAULT_BLOCK_SIZE, SDMMC_DEFAULT_BLOCK_SIZE,
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
	.storage_dev = (void *)PARTITION_ID(storage_partition),
	.mnt_point = "/root",
};
#endif

/* Maintenance guarantees this begins with '/' and is NUL-terminated. */
static uint8_t cwd[CONFIG_ZLRCLIB_FS_PATH_LEN] = "/";

static int create_abs_path(const uint8_t *file_name, uint8_t *path)
{
	int ret;

	if (!file_name || !path) {
		return -EINVAL;
	}

	ret = snprintf(path, CONFIG_ZLRCLIB_FS_PATH_LEN, "%s/%s", cwd, file_name);

	if (ret < 0) {
		return ret;
	}

	else if (ret >= CONFIG_ZLRCLIB_FS_PATH_LEN) {
		fprintf(stderr, "Error: Path exceeds maximum length (%d bytes)\n",
			CONFIG_ZLRCLIB_FS_PATH_LEN);
		return -ENAMETOOLONG;
	}

	return ret;
}

int zlrclib_cd(uint8_t *path)
{
	int ret;

	if (!path || *path == '\0') {
		LOG_ERR("Invalid path");
		return -EINVAL;
	}

	struct fs_dirent entry;

	if (strcmp(path, "..") == 0) {
		uint8_t *prev = strrchr(cwd, '/');

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

int zlrclib_ls(uint8_t *path)
{
	int ret;

	if (!path || *path == '\0') {
		LOG_ERR("Invalid path");
		return -EINVAL;
	}

	struct fs_dir_t dir;
	fs_dir_t_init(&dir);

	ret = fs_opendir(&dir, path);
	if (ret != 0) {
		LOG_ERR("Unable to open file: %s (%d)", path, ret);
		return -EIO;
	}

	while (1) {
		struct fs_dirent entry;
		const uint8_t *name_end;

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
		LOG_DBG("%s%s", entry.name, name_end);
	}

	fs_closedir(&dir);

	return 0;
}

int zlrclib_pwd(uint8_t *path)
{
	if (!path) {
		LOG_ERR("Invalid path");
		return -EINVAL;
	}

	LOG_DBG("%s", cwd);
	memset(path, 0, strlen(path));
	memcpy(path, cwd, strlen(cwd));

	return 0;
}

int zlrclib_rm(const uint8_t *file_name)
{
	int ret;
	uint8_t path[CONFIG_ZLRCLIB_FS_PATH_LEN];

	if (!file_name) {
		LOG_ERR("rm: Invalid argument");
		return -EINVAL;
	}

	create_abs_path(file_name, path);

	ret = fs_unlink(path);
	if (ret != 0) {
		LOG_ERR("Failed to remove file: %s (%d)", path, ret);
		ret = -EIO;
	}

	return ret;
}

int zlrclib_fwrite(const uint8_t *file_name, uint8_t *buf, size_t buf_len, uint16_t pos)
{
	int ret;
	struct fs_file_t file;
	uint8_t path[CONFIG_ZLRCLIB_FS_PATH_LEN];

	if (!file_name || !buf || buf_len < 1) {
		LOG_ERR("fwrite: Invalid argument");
		return -EINVAL;
	}

	create_abs_path(file_name, path);

	fs_file_t_init(&file);

	ret = fs_open(&file, path, FS_O_CREATE | FS_O_WRITE);
	if (ret < 0) {
		LOG_ERR("Failed to open file: %s (%d)", path, ret);
		fs_close(&file);
		return ret;
	}

	ret = fs_seek(&file, pos, FS_SEEK_SET);
	if (ret < 0) {
		LOG_ERR("Failed to seek file: %s (%d)", path, ret);
		return ret;
	}

	ret = fs_write(&file, buf, buf_len);
	if (ret < 0) {
		LOG_ERR("Failed to write file: %s (%d)", path, ret);
	} else {
		LOG_DBG("Wrote %d bytes to %s", ret, path);
	}

	fs_close(&file);

	return ret;
}

int zlrclib_fread(const uint8_t *file_name, uint8_t *buf, size_t buf_len, uint16_t pos)
{
	int ret;
	struct fs_file_t file;
	uint8_t path[CONFIG_ZLRCLIB_FS_PATH_LEN];

	if (!file_name || !buf || buf_len < 1) {
		LOG_ERR("fread: Invalid argument");
		return -EINVAL;
	}

	create_abs_path(file_name, path);

	fs_file_t_init(&file);

	ret = fs_open(&file, path, FS_O_READ);
	if (ret < 0) {
		LOG_ERR("Failed to open file: %s (%d)", path, ret);
		return ret;
	}

	ret = fs_seek(&file, pos, FS_SEEK_SET);
	if (ret < 0) {
		LOG_ERR("Failed to seek %s: (%d)", path, ret);
		return ret;
	}

	ret = fs_read(&file, buf, buf_len);
	if (ret < 0) {
		LOG_ERR("Failed to read file: %s (%d)", path, ret);
		fs_close(&file);
		return ret;
	}

	buf[ret] = '\0';

	LOG_DBG("Read %d bytes from %s", ret, path);

	fs_close(&file);

	return ret;
}

static int zlrclib_mount(void)
{
	int ret;

	ret = fs_mount(&lfs_mnt);
	if (ret < 0) {
		LOG_ERR("Mount failed: %d", ret);
		return 0;
	}

	zlrclib_cd("/root");

	return 0;
}

SYS_INIT(zlrclib_mount, APPLICATION, 95);
