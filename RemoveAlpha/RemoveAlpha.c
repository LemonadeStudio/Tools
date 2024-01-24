#define STB_IMAGE_IMPLEMENTATION
#include "../_Third/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../_Third/stb/stb_image_write.h"

#include <stdio.h>

int main(int argc, char ** argv) {

	if(argc < 2) {
		printf(
			"Usage:\n"
			"./RemoveAlpha <in.png> <out.png>\n"
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
	for(size_t i = channel - 1; i < siz; i += channel) {
		data[i] = 255;
	}
	printf("Process over! Now saving...\n");

	stbi_write_png(argv[2], width, height, channel, data, 0);
	printf("Save over!\n");

	stbi_image_free(data);

	return 0;
}
