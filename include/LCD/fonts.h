/* fonts.h */




typedef struct /* Struct for font descriptor */
{
	uint8_t width;
	uint8_t height;
	uint16_t offset;
} FONT_CHAR_INFO;





typedef struct /* Struct for font handling */
{
	uint8_t height; /* Max height of the font chars */
	uint8_t startChar; /* Array will start from this char. So, should be used as offset */
	uint16_t endChar; /* Seems useless. May be removed */
	uint8_t spaceWidth; /* Space between two chars, when printing a string */
	const FONT_CHAR_INFO *descriptor; /* Pointer to descriptor address of this font */
	const uint8_t *bitmap; /* Pointer to bitmap address of this font */
} FONT_INFO;





extern const uint8_t Font14Bitmap[];
extern const FONT_CHAR_INFO Font14Descriptors[];
extern const FONT_INFO Font14;







/* fonts.h */
