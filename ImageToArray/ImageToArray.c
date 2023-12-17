#define STB_IMAGE_IMPLEMENTATION
#include "../_Third/stb/stb_image.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	if(argc < 3) {
		printf("ImageToArray <inputFilename> <outputFilename>");
		return 0;
	}

	int width, height, channels;
	unsigned char * data = stbi_load(argv[1], & width, & height, & channels, 0);
	if(!data) {
		printf("Cannot load the image file:\n%s", argv[1]);
		return -1;
	}

	FILE * outputFile = fopen(argv[2], "w+");
	if(!outputFile) {
		printf("Cannot open or create the output file:\n%s", argv[2]);
		return -1;
	}

	printf("Input file: %s\nOutput file: %s\n", argv[1], argv[2]);

	/* width: in pixels => in bytes */
	width *= channels;
	
	fputs("{\n", outputFile);
	int i, j;
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			fprintf(outputFile, "%3d", data[i * width + j]);
			if(i != height - 1 || j != width - 1) {
				fputs(", ", outputFile);
			}
		}
		fputc('\n', outputFile);
	}
	fputs("}\n", outputFile);

	fclose(outputFile);

	stbi_image_free(data);

	printf("Over.");
	return 0;
}
