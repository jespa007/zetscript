/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define TYPE_SCRIPT_VARIABLE 						"__ScriptObject__"

namespace zetscript{

	class ScriptObject{
	public:
		InfoSharedPointerNode 	* 	shared_pointer; // 8
		short						idx_script_class; // 2

		ScriptObject();
		void setZetScript(ZetScript	*_zs);


		virtual				size_t length();

		//bool 				initSharedPtr();
		//bool 				unrefSharedPtr(int _idx_current_call);
		ZetScript      * 	getZetScript();

		ScriptClass * 	    getNativeScriptClass();
		bool 				isNativeObject();
		const std::string & getClassName();
		ScriptClass * 	    getScriptClass();
		virtual StackElement * 			getProperty(const std::string & property_name, int * idx=NULL);

		virtual std::string toString();

		virtual ~ScriptObject();
	protected:
		ZetScript 				*	zs; // 8
		StackElement 		stk_length;
	};

}
