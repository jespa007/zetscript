/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

#include "Metamethod.h"

namespace zetscript{

	class MetamethodHelper{
	public:

		static const char * getMetamethodOperatorName(Metamethod _metamethod);
		static const char * getMetamethodSymbolName(Metamethod _metamethod);
		static int			getMetamethodNumberArguments(Metamethod _metamethod);
		static bool		 	isMetamethodStatic(Metamethod _metamethod);
	};

}
