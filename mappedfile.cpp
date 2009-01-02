/* mappedfile.cpp -- provides a class for memory mapped files
 *
 * Copyright (C) 2008 Benjamin Kramer
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
#include <string>
#ifdef NO_EXCEPTIONS
#include <iostream>
#include <stdlib.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifndef HAVE_MMAP
#if defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define HAVE_MMAP 1
#endif
#endif

#ifdef WINDOWS
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

static inline void error(const std::string& mess) {
#ifndef NO_EXCEPTIONS
	throw MappedFile::IOException(mess);
#else
	std::clog << mess << std::endl;
	exit(EXIT_FAILURE);
#endif
}

MappedFile::MappedFile(const char *path) {
#ifdef WINDOWS
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		error(std::string("Couldn't open File \"") + path + "\"");
	}
	size = GetFileSize(hFile, NULL);
	HANDLE hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, size, NULL);
	data = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);
	CloseHandle(hMap);
	CloseHandle(hFile);
#elif HAVE_MMAP
	int fd = open(path, O_RDONLY);
	if (fd < 0) {
		error(std::string("Couldn't open File \"") + path + "\"");
	}
	size = lseek(fd, 0, SEEK_END);
	data = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
#else
	FILE *fd = fopen(path, "rb");
	if (fd == NULL) {
		error(std::string("Couldn't open File \"") + path + "\"");
	}
	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	rewind(fd);
	data = (char*)malloc(size);
	fread(data, size, 1, fd);
	fclose(fd);
#endif
}

MappedFile::~MappedFile() {
#ifdef WINDOWS
	UnmapViewOfFile(data);
#elif HAVE_MMAP
	munmap(data, size);
#else
	free(data);
#endif

}
