/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

		class ScriptType;
		class ScriptType;
		class ZetScript;
		struct VirtualMachine;
		class ScriptTypeFactory;
		class  ScriptObjectClass: public ScriptObjectObject{
		public:

			//---> TODO JEB: move all vars to ScriptObjectObject Class
			ScriptFunction 		*	info_function_new;
			Instruction 		*	instruction_new;
			bool 					was_created_by_constructor;

			static ScriptObjectClass * newScriptObjectClass(ZetScript *_zs,short _idx_type,void *  _c_object=NULL);

			ScriptObjectClass();
			void init(ZetScript *_zs, short _idx_type,void *_c_object);

			/**
			 * info_registered_class: scriptclass info
			 * _c_object: pass C object reference (is not delete_c_object_on_destroy by default)
			 */


			bool isCreatedByContructor();
			bool itHasGetMetamethod();
			bool itHasSetMetamethod();
			void deleteNativeObjectOnDestroy(bool _delete_on_destroy);


			virtual void * getNativeObject();
			ScriptType * getNativeScriptClass();

			const char *getNativePointerClassName();

			ScriptFunction *getConstructorFunction();

			virtual zs_string toString();

			virtual ~ScriptObjectClass();

		protected:
			void callConstructorMemberVariables(ScriptType *sc);
			//virtual StackElement * newSlotBuiltin();
			//virtual StackElement * addPropertyBuiltin(const zs_string & symbol_value, StackElement stk=k_stk_null);


		private:
			/**
			 * This variable tells whether is pointer function or not.
			 */
			ScriptType * script_class_native;
			bool delete_c_object_on_destroy;

			void * created_object;
			void * c_object;
		};
}
