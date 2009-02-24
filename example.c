/* this program maps its source into memory and outputs it then to stdout */

#include "mappedfile.h"
#include <stdio.h>

int main(void)
{
	size_t size;
	/* map the file into memory, save filesize in size */
	char *data = map_file(__FILE__, &size);

	/* check if mapping was successful */
	if (data != NULL) {
		fwrite(data, size, 1, stdout); /* print mapped memory */
		unmap_file(data, size); /* release the mapping */
	}
	return 0;
}
