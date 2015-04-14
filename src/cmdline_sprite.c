#include <lodepng/lodepng.h>
#include "cmdline.h"
#include "drawing/drawing.h"

typedef struct {
	uint32 num_entries;
	uint32 total_size;
} rct_sprite_file_header;

typedef struct {
	FILE *file;
	rct_sprite_file_header header;
} sprite_file_handle;

bool sprite_file_open(const char *path, sprite_file_handle *handle)
{
	FILE *file;

	file = fopen(path, "rb");
	if (file == NULL)
		return false;

	handle->file = file;
	if (fread(&handle->header, sizeof(rct_sprite_file_header), 1, file) != 1) {
		fclose(file);
		return false;
	}

	return true;
}

bool sprite_file_export(sprite_file_handle *handle, int spriteIndex, const char *outPath)
{
	rct_g1_element spriteHeader;
	rct_drawpixelinfo dpi;
	uint8 *palette, *pixels;
	int pixelBufferSize;

	fseek(handle->file, sizeof(rct_sprite_file_header) + spriteIndex * sizeof(rct_g1_element), SEEK_SET);
	if (fread(&spriteHeader, sizeof(rct_g1_element), 1, handle->file) != 1)
		return 0;

	pixelBufferSize = spriteHeader.width * spriteHeader.height;
	pixels = malloc(pixelBufferSize);
	memset(pixels, 0, pixelBufferSize);

	dpi.bits = pixels;
	dpi.x = 0;
	dpi.y = 0;
	dpi.width = spriteHeader.width;
	dpi.height = spriteHeader.height;
	dpi.pitch = spriteHeader.width;
	dpi.zoom_level = 0;

	palette = (uint8*)0x01424680;
	gfx_rle_sprite_to_buffer(spriteHeader.offset, pixels, palette, &dpi, IMAGE_TYPE_NO_BACKGROUND, 0, spriteHeader.height, 0, spriteHeader.width);


	LodePNGState pngState;
	unsigned int pngError;
	unsigned char* pngData;
	size_t pngSize;

	lodepng_state_init(&pngState);
	pngState.info_raw.colortype = LCT_PALETTE;
	int stride = (spriteHeader.width + 3) & ~3;
	for (int i = 0; i < 256; i++) {
		lodepng_palette_add(
			&pngState.info_raw,
			palette[i * 4 + 2],
			palette[i * 4 + 1],
			palette[i * 4 + 0],
			255
		);
	}

	pngError = lodepng_encode(&pngData, &pngSize, pixels, stride, spriteHeader.height, &pngState);
	if (pngError != 0) {
		fprintf(stderr, "Error creating PNG data, %u: %s", pngError, lodepng_error_text(pngError));
		return false;
	} else {
		lodepng_save_file(pngData, pngSize, outPath);
		free(pngData);
		return true;
	}
}

int cmdline_for_sprite(const char **argv, int argc)
{
	sprite_file_handle spriteFile;

	if (argc == 0)
		return -1;

	if (_strcmpi(argv[0], "export") == 0) {
		if (argc < 3) {
			fprintf(stderr, "usage: sprite export <input> <idx> <output>\n");
			return -1;
		}

		const char *inputPath = argv[1];
		int spriteIndex = atoi(argv[2]);
		const char *outputPath = argv[3];

		if (!sprite_file_open(inputPath, &spriteFile)) {
			fprintf(stderr, "Unable to open input sprite file.\n");
			return -1;
		}

		if (spriteIndex < 0 || spriteIndex >= (int)spriteFile.header.num_entries) {
			fprintf(stderr, "Sprite #%d does not exist in sprite file.\n", spriteIndex);
			return -1;
		}

		if (!sprite_file_export(&spriteFile, spriteIndex, outputPath))
			return -1;

		return 1;
	} else {
		return -1;
	}
}