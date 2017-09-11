#pragma once

#include "CZetScript.h"

namespace zetscript{

	class CJSON{
	public:

		static void registerScript();
		static CStructScriptVariable * parse(const char *str);
	};


}
