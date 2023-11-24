/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define SCRIPT_TYPE_NAME_UNDEFINED					"Undefined"
#define SCRIPT_TYPE_NAME_NULL 						"Null"
#define SCRIPT_TYPE_NAME_INT 						"Integer"
#define SCRIPT_TYPE_NAME_BOOL 						"Boolean"
#define SCRIPT_TYPE_NAME_STACK_ELEMENT 				"StackElement"
#define SCRIPT_TYPE_NAME_FLOAT 						"Float"
#define SCRIPT_TYPE_NAME_FUNCTION					"Function"
#define SCRIPT_TYPE_NAME_CONTAINER_SLOT 			"ContainerSlot"
#define SCRIPT_TYPE_NAME_OBJECT_STRING 				"String"
#define SCRIPT_TYPE_NAME_OBJECT_ARRAY 				"Array"
#define SCRIPT_TYPE_NAME_OBJECT_OBJECT 				"Object"
#define SCRIPT_TYPE_NAME_OBJECT_CLASS 				"ObjectClass"
#define SCRIPT_TYPE_NAME_OBJECT_VAR_REF 			"VarRef"
#define SCRIPT_TYPE_NAME_OBJECT_FUNCTION_MEMBER 	"MemberFunction"
#define SCRIPT_TYPE_NAME_OBJECT_ITERATOR_ASSIGNRING "IteratorString"
#define SCRIPT_TYPE_NAME_OBJECT_ITERATOR_ARRAY 		"IteratorArray"
#define SCRIPT_TYPE_NAME_OBJECT_ITERATOR_OBJECT 	"IteratorObject"
#define SCRIPT_TYPE_NAME_SCRIPT_OBJECT 				"__ScriptObject__"

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
		SCRIPT_TYPE_ID_ZS_STRING_C,
		SCRIPT_TYPE_ID_ZS_STRING_PTR_C,
		SCRIPT_TYPE_ID_BOOL_C,
		SCRIPT_TYPE_ID_BOOL_PTR_C,
		SCRIPT_TYPE_ID_FLOAT_C,
		SCRIPT_TYPE_ID_FLOAT_PTR_C,
		SCRIPT_TYPE_ID_CONST_FLOAT_PTR_C,

		SCRIPT_TYPE_ID_STACK_ELEMENT,
		SCRIPT_TYPE_ID_FUNCTION, // function a(){}
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_VAR_REF,
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_FUNCTION_MEMBER,
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_ZETSCRIPT, // zetscript
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// BUILT-IN CLASSES
		//
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_STRING, // "String"

		// Iterator
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_STRING, // "IteratorString"
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_ARRAY, // IteratorArray []
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_ITERATOR_OBJECT, // IteratorObject
		//
		// BUILT-IN CLASSES
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// CONTAINER CLASSES
		//
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_ARRAY, // Array []

		// Object & type
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_OBJECT, //  Object {}
		SCRIPT_TYPE_ID_SCRIPT_OBJECT_CLASS, // class A{}

		//
		// CONTAINER CLASSES
		//
		//--------------------------------------------------------------------------------------------------------------------->
		SCRIPT_TYPE_ID_MAX

	};
}
