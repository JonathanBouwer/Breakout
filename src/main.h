#include <stdint.h>
#include <assert.h>  
//#define NDEBUG 

struct WindowParameters {
	int16_t WindowWidth;
	int16_t WindowHeight;
	int16_t GameWidth;
	int16_t GameHeight;
	int16_t leftOffset;
	int16_t topOffset;
} windowSizes = { 1280 , 720, 1152, 648 };

struct InputState {
	uint8_t numberOfKeys = 8;
	uint8_t keystate;
	uint32_t setKeys[8];
} inputState;