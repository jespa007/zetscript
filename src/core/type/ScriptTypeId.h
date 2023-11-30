/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_SCRIPT_TYPE_NAME_UNDEFINED					"Undefined"
#define ZS_SCRIPT_TYPE_NAME_NULL 						"Null"
#define ZS_SCRIPT_TYPE_NAME_INT 						"Integer"
#define ZS_SCRIPT_TYPE_NAME_BOOL 						"Boolean"
#define ZS_SCRIPT_TYPE_NAME_STACK_ELEMENT 				"StackElement"
#define ZS_SCRIPT_TYPE_NAME_FLOAT 						"Float"
#define ZS_SCRIPT_TYPE_NAME_FUNCTION					"ScriptFunction"
#define ZS_SCRIPT_TYPE_NAME_CONTAINER_SLOT 			"ContainerSlot"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_STRING 				"String"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_ARRAY 				"Array"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_SCRIPT_OBJECT 			"ScriptObject"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_CLASS 				"ObjectClass"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_VAR_REF 			"VarRef"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_FUNCTION_MEMBER 	"MemberFunction"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_ITERATOR_STRING 	"IteratorString"
#define ZS_SCRIPT_TYPE_NAME_ARRAY_ITERATOR_SCRIPT_OBJECT 		"IteratorArray"
#define ZS_SCRIPT_TYPE_NAME_OBJECT_ITERATOR_SCRIPT_OBJECT 	"IteratorObject"
#define ZS_SCRIPT_TYPE_NAME_OBJECT 					"__Object__"

namespace zetscript{

	typedef int ScriptTypeId;

	enum: int16_t{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		SCRIPT_TYPE_ID_INVALID=-1,      // Invalid
		SCRIPT_TYPE_ID_CLASS_MAIN =0, 	// Main type ...
		SCRIPT_TYPE_ID_UNDEFINED, 		// UNDEFINED ...
		SCRIPT_TYPE_ID_NULL, 			// NULL ...


		// built in C types...
		SCRIPT_TYPE_ID_VOID_C,
		SCRIPT_TYPE_ID_INT_C,
		SCRIPT_TYPE_ID_INT_PTR_C,
		SCRIPT_TYPE_ID_CHAR_PTR_C,
		SCRIPT_TYPE_ID_CONST_CHAR_PTR_C,
		SCRIPT_TYPE_ID_STRING_C,
		SCRIPT_TYPE_ID_STRING_PTR_C,
		SCRIPT_TYPE_ID_BOOL_C,
		SCRIPT_TYPE_ID_BOOL_PTR_C,
		SCRIPT_TYPE_ID_FLOAT_C,
		SCRIPT_TYPE_ID_FLOAT_PTR_C,
		SCRIPT_TYPE_ID_CONST_FLOAT_PTR_C,

		SCRIPT_TYPE_ID_STACK_ELEMENT,
		SCRIPT_TYPE_ID_FUNCTION, // function a(){}
		SCRIPT_TYPE_ID_VAR_REF_SCRIPT_OBJECT,
		SCRIPT_TYPE_ID_FUNCTION_MEMBER_SCRIPT_OBJECT,
		SCRIPT_TYPE_ID_ZETSCRIPT_SCRIPT_OBJECT, // zetscript
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// BUILT-IN CLASSES
		//
		SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT, // "String"

		// Iterator
		SCRIPT_TYPE_ID_OBJECT_ITERATOR_STRING, // "IteratorString"
		SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT, // IteratorArray []
		SCRIPT_TYPE_ID_OBJECT_ITERATOR_SCRIPT_OBJECT, // IteratorObject
		//
		// BUILT-IN CLASSES
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// CONTAINER CLASSES
		//
		SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT, // Array []

		// ScriptObject & type
		SCRIPT_TYPE_ID_OBJECT_SCRIPT_OBJECT, //  ScriptObject {}
		SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT, // class A{}

		//
		// CONTAINER CLASSES
		//
		//--------------------------------------------------------------------------------------------------------------------->
		SCRIPT_TYPE_ID_MAX

	};
}
