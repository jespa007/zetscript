/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{


	class  ScriptObjectVectorIterator:public ScriptObject{

	public:

		ScriptObjectVectorIterator();

		StackElement _get();
		void _next();
		bool _end();

	};

}
