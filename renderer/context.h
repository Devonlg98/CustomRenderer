#pragma once
class context
{
	// Forward declaration -- I am declaring a type and using it here
	struct GLFWwindow* window;


public:
	bool init(int width, int height, const char* title);
	void tick();
	void term();
	void clear();

	// const makes functions that shouldn't modifiy anything? "Hey don't modifiy anything here" -Terry 2020
	bool shouldClose() const;

	float time() const;
	void resetTime(float resetValue);
};