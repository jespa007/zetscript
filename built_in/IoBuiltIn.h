#pragma once

#define CLOCK_MILLISECONDS (1.0f/1000.0f)

class IoBuiltIn{
public:
	// clock
	static intptr_t clock(){
		return std::clock()*(1000.0f/CLOCKS_PER_SEC);
	}
};
