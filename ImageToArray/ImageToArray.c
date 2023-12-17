#define STB_IMAGE_IMPLEMENTATION
#include "../_Third/stb/stb_image.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
	if(argc < 4) {
		printf("ImageToArray <input filename> <output filename> <flip vertically 0/1> [array's name]");
		return 0;
	}

	stbi_set_flip_vertically_on_load(argv[3][0] - '0');
	
	int width, height, channel;
	unsigned char * data = stbi_load(argv[1], & width, & height, & channel, 0);
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

	const char * arrayName = argc < 5 ? "__outputImageData" : argv[4];
	fprintf(outputFile, "constexpr int %sWidth = %d, %sHeight = %d, %sChannel = %d;\n", arrayName, width, arrayName, height, arrayName, channel);

	/* width: in pixels => in bytes */
	width *= channel;
	
	fprintf(outputFile, "const unsigned char * %s = new unsigned char [%d] {\n", arrayName, width * height);
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
	fputs("};", outputFile);

	fclose(outputFile);

	stbi_image_free(data);

	printf("Over.");
	return 0;
}
