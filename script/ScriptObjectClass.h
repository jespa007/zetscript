/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

		class ScriptClass;
		class ScriptClass;
		class ZetScript;
		class VirtualMachine;
		class ScriptClassFactory;
		class  ScriptObjectClass: public ScriptObjectObject{
		public:

			//---> TODO JEB: move all vars to ScriptObjectObject Class
			ScriptFunction 		*	info_function_new;
			Instruction 		*	instruction_new;
			bool 					was_created_by_constructor;

			static ScriptObjectClass * newScriptObjectClass(ZetScript *zs);

			ScriptObjectClass();
			//ScriptObjectClass(ZetScript *zs, short idx_script_class,void *c_object=NULL);

			/**
			 * info_registered_class: scriptclass info
			 * _c_object: pass C object reference (is not delete_c_object by default)
			 */
			void init(short idx_script_class,void *  _c_object=NULL);

			bool isCreatedByContructor();
			bool itHasSetMetamethod();
			void deleteNativeObjectOnDestroy(bool _delete_on_destroy);

			void * getNativeObject();
			ScriptClass * getNativeScriptClass();

			Symbol *		getSymbolMemberByIdx(int idx_symbol_member);
			const std::string & getNativePointerClassName();

			ScriptFunction *getConstructorFunction();

			virtual std::string toString();

			~ScriptObjectClass();

		protected:
			void callConstructorBuiltin(ScriptClass *sc);

		private:
			/**
			 * This variable tells whether is pointer function or not.
			 */
			//ScriptClass *script_class;
			zs_vector	* builtin_members;
			ScriptClass * script_class_native;
			bool delete_c_object;

			void * created_object;
			void * c_object;


			StackElement * addPropertyBuiltIn(
					const std::string & symbol_value
			);
		};
}
