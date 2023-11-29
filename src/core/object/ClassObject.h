/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

		class Type;
		class Type;
		class ZetScript;
		struct VirtualMachine;
		class TypesFactory;
		class  ClassObject: public DictionaryObject{
		public:

			Function 		*	info_function_new;
			Instruction 		*	instruction_new;
			bool 					was_created_by_constructor;

			static ClassObject * newClassObject(ZetScript *_zs,TypeId _type_id,void *  _c_object=NULL);

			ClassObject(ZetScript *_zs, TypeId _type_id,void *  _c_object=NULL);

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
			Type * getNativeScriptClass();

			const char *getTypeNamePtr();

			Function *getConstructorFunction();

			virtual String toString();

			virtual ~ClassObject();

		protected:
			void callConstructorMemberVariables(Type *sc);


		private:
			/**
			 * This variable tells whether is pointer function or not.
			 */
			Type * script_class_native;
			bool delete_c_object_on_destroy;

			void * created_object;
			void * c_object;
		};
}
