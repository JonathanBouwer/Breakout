#include <stdint.h>
#include <assert.h>  
//#define NDEBUG 
struct Vector {
	float x;
	float y;

	Vector(float x, float y) : x(x), y(y) {};
	Vector operator+(const Vector &other) {
		return Vector(x + other.x, y + other.y);
	}
	Vector operator-(const Vector &other) {
		return Vector(x - other.x, y - other.y);
	}
	Vector& operator+=(const Vector &other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	Vector& operator-=(const Vector &other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	Vector operator*(float c) {
		return Vector(x * c, y * c);
	}
};

inline Vector operator*(float f, Vector v) {
	return v*f;
}

struct WindowParameters {
	int16_t WindowWidth;
	int16_t WindowHeight;
	int16_t GameWidth;
	int16_t GameHeight;
	int16_t ClientWidth;
	int16_t ClientHeight;
	int16_t leftOffset;
	int16_t topOffset;
} windowSizes = { 1280 , 720, 1152, 648 };

struct InputState {
	uint8_t numberOfKeys = 8;
	uint8_t keystate;
	uint32_t setKeys[8];
} inputState;

enum StateLabel {
	INV = 0x00,
	MENU = 0x01,
	PLAY = 0x02,
	PAUSE = 0x04
};

struct Player {
	int32_t width;
	int32_t height;
	Vector position;
	Vector velocity;

	Player() : width(0), height(0), position(Vector(0, 0)), velocity(Vector(0, 0)) {};
};

struct Ball {
	int32_t radius;
	Vector position;
	Vector velocity;

	Ball() : radius(0), position(Vector(0, 0)), velocity(Vector(0, 0)) {};
};

struct GameState {
	StateLabel currentState;
	StateLabel goalState;
	Player player;
	Ball ball;

	GameState() {

	}
} gameState;