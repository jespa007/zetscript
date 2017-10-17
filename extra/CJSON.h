/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "CZetScript.h"

namespace zetscript{

	class CJSON{
	public:

		static void registerScript();
		static CStructScriptVariable * parse(const char *str);
	};


}
