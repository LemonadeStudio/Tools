#define STB_IMAGE_IMPLEMENTATION
#include "../_Third/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../_Third/stb/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SImage {
	unsigned char * data;
	int width, height;
	int x, y;
	char * filenameWithoutDirectory;
} SImage;

SImage * ImageLoad(const char * filename) {
	SImage * pImage = (SImage *)malloc(sizeof(SImage));

	int channel = 0;
	pImage->data = stbi_load(filename, & pImage->width, & pImage->height, & channel, 0);
	if(!pImage->data) {
		printf("Error! %s cannot be loaded!\n", filename);
		free(pImage);
		return NULL;
	}
	if(channel != 4) {
		printf("Error! There are %d channels in %s!\n", channel, filename);
		stbi_image_free(pImage->data);
		free(pImage);
		return NULL;
	}

	pImage->x = pImage->y = -1;

	size_t i, len = strlen(filename);
	for(i = len - 1; i != 0; i--) {
		if(filename[i] == '/' || filename[i] == '\\') {
			i++;
			break;
		}
	}
	len = len - i + 1;

	pImage->filenameWithoutDirectory = (char *)malloc(sizeof(char) * len);
	strcpy(pImage->filenameWithoutDirectory, filename + i);

	return pImage;
}

void ImageFree(SImage * pImage) {
	free(pImage->filenameWithoutDirectory);
	stbi_image_free(pImage->data);
	free(pImage);
}

// 排序优先考虑宽度，到后头拼接的时候优先考虑高度
int ImageCompare(const void * a, const void * b) {
	const SImage * p1 = *(SImage **)a;
	const SImage * p2 = *(SImage **)b;
	if(p2->width == p1->width) {
		return p2->height - p1->height;
	}
	return p2->width - p1->width;
}

void SaveUVData(const char * filename, int textureW, int textureH, SImage ** images, int imageNum);

int main(int argc, char ** argv) {
	if(argc < 2) {
		printf("Usage:\nDrag images onto this exe to run\n");
		system("pause");
		return 0;
	}

	printf("Number of input images: %d\n", argc - 1);

	char name[1024];
	int width, height, alignment, spacing;
	printf("Input output image width & height(example: 2048 2048): ");
	scanf_s("%d%d", & width, & height);
	printf("Input alignment(example: 1): ");
	scanf_s("%d", & alignment);
	printf("Input spacing between images(example: 16): ");
	scanf_s("%d", & spacing);
	printf("Input output filename(without suffix): ");
	scanf_s("%s", name);

	SImage * images[argc - 1];
	int imageNum = 0;
	for(int i = 1; i < argc; i++) {
		SImage * pImage = ImageLoad(argv[i]);
		if(pImage) {
			images[imageNum++] = pImage;
		}
	}
	qsort(images, imageNum, sizeof(* images), ImageCompare);

	unsigned char * outImage = (unsigned char *)malloc(sizeof(unsigned char) * height * width * 4);
	for(int i = height * width * 4 - 1; i >= 0; i--) {
		outImage[i] = 0;
	}

	int ** marks = (int **)malloc(sizeof(int *) * height); // 存储该位置上的图片的底部边缘的y坐标
	for(int i = 0; i < height; i++) {
		marks[i] = (int *)malloc(sizeof(int) * width);
		for(int j = 0; j < width; j++) {
			marks[i][j] = 0;
		}
	}

	for(int i = 0; i < imageNum; i++) {
		SImage * pImage = images[i];
		for(int x = 0; x < width; x += alignment) {
			if(x + pImage->width > width) {
				printf("Error! Output image size is too small!\n");
				system("pause");
				return -1;
			}
			for(int y = 0; y < height; y += alignment) {
				if(y + pImage->height > height) {
					break;
				}
				int canDrawHere = 1;
				for(int xx = -spacing; xx < pImage->width + spacing; xx++) {
					for(int yy = -spacing; yy < pImage->height + spacing; yy++) {
						if(y + yy < 0 || x + xx < 0 || y + yy >= height || x + xx >= width) {
							canDrawHere = 0;
							break;
						}
						if(marks[y + yy][x + xx]) {
							y = marks[y + yy][x + xx];
							xx = yy = -spacing;
						}
					}
					if(!canDrawHere) {
						break;
					}
				}
				if(!canDrawHere) {
					continue;
				}
				for(int xx = 0; xx < pImage->width; xx++) {
					for(int yy = 0; yy < pImage->height; yy++) {
						marks[y + yy][x + xx] = y + pImage->height + spacing;
						for(int channelIndex = 0; channelIndex < 4; channelIndex++) {
							outImage[(y + yy) * width * 4 + (x + xx) * 4 + channelIndex] = pImage->data[yy * pImage->width * 4 + xx * 4 + channelIndex];
						}
					}
				}
				pImage->x = x;
				pImage->y = y;

				goto nextImage;
			}
		}
nextImage: {};
	}

	char outPngName[1024], outTxtName[1024];
	strcpy(outPngName, name);
	strcat(outPngName, ".png");
	strcpy(outTxtName, name);
	strcat(outTxtName, ".txt");

	printf("Start to save now\n");
	stbi_write_png(outPngName, width, height, 4, outImage, 0);
	printf("Saved texture atlas as %s\n", outPngName);
	SaveUVData(outTxtName, width, height, images, imageNum);
	printf("Saved uv data as %s\n", outTxtName);

	for(int i = 0; i < imageNum; i++) {
		ImageFree(images[i]);
		images[i] = NULL;
	}

	free(outImage);

	for(int i = 0; i < height; i++) {
		free(marks[i]);
	}
	free(marks);

	system("pause");

	return 0;
}

void SaveUVData(const char * filename, int textureW, int textureH, SImage ** images, int imageNum) {
	FILE * f = fopen(filename, "w");
	if(!f) {
		printf("Error! Failed to write into %s!\n", filename);
		return;
	}
	fprintf(f, "{ // \"image name\" : (left top) (right bottom)\n");
	for(int i = 0; i < imageNum; i++) {
		float texW = (float)textureW, texH = (float)textureH;
		float left = (float)images[i]->x, top = (float)(textureH - images[i]->y);
		float right = (float)(images[i]->x + images[i]->width), bottom = (float)(textureH - (images[i]->y + images[i]->height));
		fprintf(f, "{ \"%s\", { { %ff, %ff }, { %ff, %ff } } }%s",
			images[i]->filenameWithoutDirectory,
			left / texW, top / texH,
			right / texW, bottom / texH,
			(i == imageNum - 1) ? "\n" : ",\n"
		);
	}
	fprintf(f, "}");
	fclose(f);
}
