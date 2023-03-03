/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_TYPE_NAME_UNDEFINED					"Undefined"
#define ZS_TYPE_NAME_NULL 						"Null"
#define ZS_TYPE_NAME_INT 						"Integer"
#define ZS_TYPE_NAME_BOOL 						"Boolean"
#define ZS_TYPE_NAME_STACK_ELEMENT 				"StackElement"
#define ZS_TYPE_NAME_FLOAT 						"Float"
#define ZS_TYPE_NAME_FUNCTION					"Function"
#define ZS_TYPE_NAME_CONTAINER_SLOT 			"ContainerSlot"
#define ZS_TYPE_NAME_OBJECT_STRING 				"String"
#define ZS_TYPE_NAME_OBJECT_ARRAY 				"Array"
#define ZS_TYPE_NAME_OBJECT_OBJECT 				"Object"
#define ZS_TYPE_NAME_OBJECT_CLASS 				"ObjectClass"
#define ZS_TYPE_NAME_OBJECT_VAR_REF 			"VarRef"
#define ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER 	"MemberFunction"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_STRING 	"IteratorString"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_ARRAY 	"IteratorArray"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT 	"IteratorObject"

namespace zetscript{

	typedef enum:short{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		IDX_TYPE_CLASS_MAIN =0, 	// Main type ...
		IDX_TYPE_UNDEFINED, 		// UNDEFINED ...
		IDX_TYPE_NULL, 		// NULL ...


		// built in C types...
		IDX_TYPE_VOID_C,
		IDX_TYPE_ZS_INT_C,
		IDX_TYPE_ZS_INT_PTR_C,
		IDX_TYPE_CHAR_PTR_C,
		IDX_TYPE_CONST_CHAR_PTR_C,
		IDX_TYPE_ZS_STRING_C,
		IDX_TYPE_ZS_STRING_PTR_C,
		IDX_TYPE_BOOL_C,
		IDX_TYPE_BOOL_PTR_C,
		IDX_TYPE_ZS_FLOAT_C,
		IDX_TYPE_ZS_FLOAT_PTR_C,
		IDX_TYPE_CONST_ZS_FLOAT_PTR_C,

		IDX_TYPE_STACK_ELEMENT,
		IDX_TYPE_CONTAINER_SLOT,
		IDX_TYPE_FUNCTION, // function a(){}
		IDX_TYPE_SCRIPT_OBJECT_VAR_REF,
		IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER,
		IDX_TYPE_SCRIPT_OBJECT_ZETSCRIPT, // zetscript
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// BUILT-IN CLASSES
		//
		IDX_TYPE_SCRIPT_OBJECT_STRING, // "String"

		// Iterator
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING, // "IteratorString"
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_ARRAY, // IteratorArray []
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT, // IteratorObject
		//
		// BUILT-IN CLASSES
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// CONTAINER CLASSES
		//
		IDX_TYPE_SCRIPT_OBJECT_ARRAY, // Array []

		// Object & type
		IDX_TYPE_SCRIPT_OBJECT_OBJECT, //  Object {}
		IDX_TYPE_SCRIPT_OBJECT_CLASS, // class A{}

		//
		// CONTAINER CLASSES
		//
		//--------------------------------------------------------------------------------------------------------------------->
		IDX_TYPE_MAX
	}Type;

	Type	is_built_in_type(const char *s);
}
