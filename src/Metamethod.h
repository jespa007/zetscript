/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Metamethod.h"

namespace zetscript{

	class MetamethodHelper{
	public:

		static const char * getOperatorName(Metamethod _metamethod);
		static const char * getSymbolName(Metamethod _metamethod);
		static int			getNumberArguments(Metamethod _metamethod);
		static bool		 	shouldBeStatic(Metamethod _metamethod);
	};

}
