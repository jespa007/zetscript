/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class  ScriptObjectObjectIterator:public ScriptObject{

	public:

		ScriptObjectObjectIterator();

		StackElement get();
		void next();
		bool end();

	};

}
