/* mappedfile.c -- cross-platform wrapper for memory mapped files
 *
 * Copyright (C) 2008-2009 Benjamin Kramer
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include "mappedfile.h"

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__MACH__)
#	include <unistd.h>
#	ifndef HAVE_MMAP
#		if _POSIX_VERSION >= 199506L
#			define HAVE_MMAP 1
#		endif /* _POSIX_VERSION */
#	endif /* HAVE_MMAP */
#endif

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#elif HAVE_MMAP
#	include <fcntl.h>
#	include <sys/types.h>
#	include <sys/mman.h>
#else /* !_WIN32 && !HAVE_MMAP */
#	include <stdlib.h>
#	include <stdio.h>
#endif /* !_WIN32 && !HAVE_MMAP */

/* returns NULL on failure */
char *map_file(const char *path, size_t *length)
{
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
	if (!hMap)
		goto fail;

	data = (char *)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);

	/* We can call CloseHandle here, but it will not be closed until
	 * we unmap the view */
	CloseHandle(hMap);
fail:
	CloseHandle(hFile);

#elif HAVE_MMAP
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		return NULL;

	/* lseek returns the offset from the beginning of the file */
	size = lseek(fd, 0, SEEK_END);
	if (size <= 0)
		goto fail;

	/* we don't need to lseek again as mmap ignores the offset */
	data = (char *)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED)
		data = NULL;
fail:
	close(fd);

#else /* !_WIN32 && !HAVE_MMAP */
	FILE *fd = fopen(path, "rb");
	if (!fd)
		return NULL;

	fseek(fd, 0, SEEK_END);
	size = ftell(fd); /* returns the size of the file */
	if (size <= 0)
		goto fail;

	rewind(fd);
	data = (char *)malloc(size);
	if (!data)
		goto fail;

	/* only return the data if the read was successful */
	if (fread(data, size, 1, fd) != 1) {
		free(data);
		data = NULL;
	}

fail:
	fclose(fd);
#endif /* !_WIN32 && !HAVE_MMAP */

	if (length)
		*length = size;
	return data;
}

void unmap_file(char *data, size_t length)
{
	(void)length; /* supresses warning about unused parameters */
#ifdef _WIN32
	UnmapViewOfFile(data);
#elif HAVE_MMAP
	munmap(data, length);
#else /* !_WIN32 && !HAVE_MMAP */
	free(data);
#endif /* !_WIN32 && !HAVE_MMAP */
}
