#pragma once

namespace zetscript{

	class IoBuiltIn{
	public:
		// clock
		static intptr_t clock(){
			return std::clock()*(1000.0f/CLOCKS_PER_SEC);
		}
	};

}
