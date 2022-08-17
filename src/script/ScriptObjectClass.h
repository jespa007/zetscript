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

			ScriptFunction 		*	info_function_new;
			Instruction 		*	instruction_new;
			bool 					was_created_by_constructor;

			static ScriptObjectClass * newScriptObjectClass(ZetScript *_zs,short _idx_script_type,void *  _c_object=NULL);

			ScriptObjectClass();
			void init(ZetScript *_zs, short _idx_script_type,void *_c_object);

			template<typename _C>
			_C to();

			/**
			 * info_registered_class: scriptclass info		 * _c_object: pass C object reference (is not delete_c_object_on_destroy by default)
			 */
			bool isCreatedByContructor();
			bool itHasGetMetamethod();
			bool itHasSetMetamethod();
			void deleteNativeObjectOnDestroy(bool _delete_on_destroy);


			virtual void * getNativeObject();
			ScriptType * getNativeScriptClass();

			const char *getTypeNamePtr();

			ScriptFunction *getConstructorFunction();

			virtual std::string toString();

			virtual ~ScriptObjectClass();

		protected:
			void callConstructorMemberVariables(ScriptType *sc);


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
