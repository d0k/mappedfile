#include "mappedfile.h"

#ifndef HAVE_MMAP
/* We should check for POSIX/SuS here, but I don't know the revision it */
/* was introduced. So we just guess that every unix supports this. */
#if defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define HAVE_MMAP 1
#endif
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif HAVE_MMAP
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#else
#include <stdlib.h>
#include <stdio.h>
#endif

/* returns NULL on failure */
char *map_file(const char* path, size_t* length) {
	char *data = NULL;
	size_t size = 0;

#ifdef _WIN32
	HANDLE hMap;
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	size = GetFileSize(hFile, NULL);
	if (size == INVALID_FILE_SIZE || size == 0)
		goto fail;

	hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, size, NULL);
	if (hMap == NULL)
		goto fail;

	data = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);
	CloseHandle(hMap);
fail:
	CloseHandle(hFile);

#elif HAVE_MMAP
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return NULL;

	size = lseek(fd, 0, SEEK_END);
	if (size <= 0)
		goto fail;

	data = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (data == (void*)-1) /* does it really return -1? */
		data = NULL;
fail:
	close(fd);
#else
	FILE *fd = fopen(path, "rb");
	if (fd == NULL)
		return NULL;

	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	if (size <= 0)
		goto fail;

	rewind(fd);
	data = (char*)malloc(size);
	if (data == NULL)
		goto fail;

	if (fread(data, size, 1, fd) != 1) {
		free(data);
		data = NULL;
	}

fail:
	fclose(fd);
#endif
	if (length != NULL)
		*length = size;
	return data;
}

/* TODO: fix warning about unused parameters*/
void unmap_file(char* data, size_t size)
{
#ifdef _WIN32
	UnmapViewOfFile(data);
#elif HAVE_MMAP
	munmap(data, size);
#else
	free(data);
#endif
}