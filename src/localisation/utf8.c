#include "localisation.h"

uint32 utf8_get_next(const utf8 *char_ptr, const utf8 **nextchar_ptr)
{
	int result;
	int numBytes;

	if (!(char_ptr[0] & 0x80)) {
		result = char_ptr[0];
		numBytes = 1;
	} else if ((char_ptr[0] & 0xE0) == 0xC0) {
		result = ((char_ptr[0] & 0x1F) << 6) | (char_ptr[1] & 0x3F);
		numBytes = 2;
	} else if ((char_ptr[0] & 0xF0) == 0xE0) {
		result = ((char_ptr[0] & 0x0F) << 12) | ((char_ptr[1] & 0x3F) << 6) | (char_ptr[2] & 0x3F);
		numBytes = 3;
	} else if ((char_ptr[0] & 0xF8) == 0xF0) {
		result = ((char_ptr[0] & 0x07) << 18) | ((char_ptr[1] & 0x3F) << 12) | ((char_ptr[1] & 0x3F) << 6) | (char_ptr[2] & 0x3F);
		numBytes = 4;
	} else {
		// TODO 4 bytes
		result = ' ';
		numBytes = 1;
	}

	if (nextchar_ptr != NULL)
		*nextchar_ptr = char_ptr + numBytes;
	return result;
}

utf8 *utf8_write_codepoint(utf8 *dst, uint32 codepoint)
{
	if (codepoint <= 0x7F) {
		dst[0] = (utf8)codepoint;
		return dst + 1;
	} else if (codepoint <= 0x7FF) {
		dst[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
		dst[1] = 0x80 | (codepoint & 0x3F);
		return dst + 2;
	} else if (codepoint <= 0xFFFF) {
		dst[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
		dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
		dst[2] = 0x80 | (codepoint & 0x3F);
		return dst + 3;
	} else {
		dst[0] = 0xF0 | ((codepoint >> 18) & 0x07);
		dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
		dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
		dst[3] = 0x80 | (codepoint & 0x3F);
		return dst + 4;
	}
}

/**
 * Inserts the given codepoint at the given address, shifting all characters after along.
 * @returns the size of the inserted codepoint.
 */
int utf8_insert_codepoint(utf8 *dst, uint32 codepoint)
{
	int shift = utf8_get_codepoint_length(codepoint);
	utf8 *endPoint = get_string_end(dst);
	memmove(dst + shift, dst, endPoint - dst + 1);
	utf8_write_codepoint(dst, codepoint);
	return shift;
}

bool utf8_is_codepoint_start(utf8 *text)
{
	if ((text[0] & 0x80) == 0) return true;
	if ((text[0] & 0xC0) == 0xC0) return true;
	return false;
}

int utf8_get_codepoint_length(int codepoint)
{
	if (codepoint <= 0x7F) {
		return 1;
	} else if (codepoint <= 0x7FF) {
		return 2;
	} else if (codepoint <= 0xFFFF) {
		return 3;
	} else {
		return 4;
	}
}

/**
 * Gets the number of characters / codepoints in a UTF-8 string (not necessarily 1:1 with bytes and not including null
 * terminator).
 */
int utf8_length(const utf8 *text)
{
	int codepoint;
	const utf8 *ch = text;

	int count = 0;
	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		count++;
	}
	return count;
}

wchar_t *utf8_to_widechar(const utf8 *src)
{
	wchar_t *result = malloc((utf8_length(src) + 1) * sizeof(wchar_t));
	wchar_t *dst = result;

	const utf8 *ch = src;
	int codepoint;
	while ((codepoint = utf8_get_next(ch, &ch)) != 0) {
		if ((uint32)codepoint > 0xFFFF) {
			*dst++ = '?';
		} else {
			*dst++ = codepoint;
		}
	}
	*dst = 0;

	return result;
}

utf8 *widechar_to_utf8(const wchar_t *src)
{
	utf8 *result = malloc((wcslen(src) * 4) + 1);
	utf8 *dst = result;

	for (; *src != 0; src++) {
		dst = utf8_write_codepoint(dst, *src);
	}
	*dst++ = 0;

	int size = dst - result;
	return realloc(result, size);
}
