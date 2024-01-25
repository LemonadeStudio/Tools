#define STB_IMAGE_IMPLEMENTATION
#include "../_Third/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../_Third/stb/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {

	if(argc < 4) {
		printf(
			"Usage:\n"
			"./RemoveAlpha <in.png> <colorOut.png> <alphaOut.png>\n"
		);
		system("pause");
		return 0;
	}

	int width, height, channel;
	unsigned char * data = stbi_load(argv[1], & width, & height, & channel, 0);
	printf("size: %dx%d, channels: %d\n", width, height, channel);

	if(channel != 4) {
		printf("Error! There are %d channels in it!\n", channel);
		stbi_image_free(data);
		return -1;
	}

	size_t siz = (size_t)width * height * channel;
	unsigned char * colorData = (unsigned char *)malloc(siz * sizeof(unsigned char));

	for(size_t i = 0; i < siz; i++) {
		colorData[i] = (i % 4 == 3) ? 255 : data[i];
	}
	printf("Process color over! Now saving...\n");

	stbi_write_png(argv[2], width, height, 4, colorData, 0);
	printf("Save color over!\n");

	for(size_t i = 0; i < siz; i += 4) {
		data[i] = data[i + 1] = data[i + 2] = data[i + 3];
		data[i + 3] = 255;
	}
	printf("Process alpha over! Now saving...\n");

	stbi_write_png(argv[3], width, height, 4, data, 0);
	printf("Save alpha over!\n");

	stbi_image_free(data);

	return 0;
}
