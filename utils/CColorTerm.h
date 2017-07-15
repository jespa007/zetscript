// ccolor.hpp
#pragma once

namespace zetscript{

	class  CColorTerm
	{
	public:

		enum {
			RESET = 0,
			BRIGHT = 1,
			DIM = 2,
			UNDERLINE = 3,
			BLINK = 4,
			REVERSE = 7,
			HIDDEN = 8

		};

		enum {

			BLACK = 0,
			RED = 1,
			GREEN = 2,
			YELLOW = 3,
			BLUE = 4,
			MAGENTA = 5,
			CYAN = 6,
			WHITE = 7
		};

		static void  setColor(FILE *std_type, int attr, int fg, int bg);
		static void  resetColor(FILE *std_type);

	};

}
