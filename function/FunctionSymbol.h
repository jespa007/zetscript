#pragma once

namespace zetscript{

	#pragma pack(push, 1)

	struct FunctionSymbol {

		StackElement object; // created object. undefined by default.
		intptr_t  proxy_ptr; // for proxy functions...
		//FunctionSymbol *super_function; // only for functions ...
		std::string key_value;
		//short idxAstNode; // in case there's ast node...

		FunctionSymbol() {
			proxy_ptr = 0;
			object= {

				0,// 0 value
				0,// no var ref related
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C // undefined and c ?.
			};
			key_value="";

			//idxAstNode = ZS_UNDEFINED_IDX;
			//super_function = NULL;
		}

	};

	#pragma pack(pop)
}
