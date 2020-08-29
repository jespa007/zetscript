#pragma once

#define CLOCK_MILLISECONDS (1.0f/1000.0f)

class IoBuiltIn{
public:
	// clock
	intptr_t clock(){
#ifdef _WIN32
		SYSTEMTIME time;
		GetSystemTime(&time);
		return (time.wSecond * 1000) + time.wMilliseconds;
#elif __GNUC__ // suposes GNU
		struct timeval te;
	    gettimeofday(&te, NULL); // get current time
	    return = te.tv_sec*1000LL + te.tv_usec*CLOCK_MILLISECONDS; // calculate milliseconds
#else
#error "Platform not supported"
#endif
/*		 clock_t ms=clock
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();*/
	}
};
