/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define TYPE_NAME_UNDEFINED					"Undefined"
#define TYPE_NAME_NULL 						"Null"
#define TYPE_NAME_INT 						"Integer"
#define TYPE_NAME_BOOL 						"Boolean"
#define TYPE_NAME_STACK_ELEMENT 				"StackElement"
#define TYPE_NAME_FLOAT 						"Float"
#define TYPE_NAME_FUNCTION					"Function"
#define TYPE_NAME_CONTAINER_SLOT 			"ContainerSlot"
#define TYPE_NAME_OBJECT_STRING 				"String"
#define TYPE_NAME_OBJECT_ARRAY 				"Array"
#define TYPE_NAME_OBJECT_OBJECT 				"Object"
#define TYPE_NAME_OBJECT_CLASS 				"ObjectClass"
#define TYPE_NAME_OBJECT_VAR_REF 			"VarRef"
#define TYPE_NAME_OBJECT_FUNCTION_MEMBER 	"MemberFunction"
#define TYPE_NAME_OBJECT_ITERATOR_ASSIGNRING "IteratorString"
#define TYPE_NAME_OBJECT_ITERATOR_ARRAY 		"IteratorArray"
#define TYPE_NAME_OBJECT_ITERATOR_OBJECT 	"IteratorObject"
#define TYPE_NAME_OBJECT 				"__Object__"

namespace zetscript{

	typedef int TypeId;

	enum: int16_t{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		TYPE_ID_INVALID=-1,      // Invalid
		TYPE_ID_CLASS_MAIN =0, 	// Main type ...
		TYPE_ID_UNDEFINED, 		// UNDEFINED ...
		TYPE_ID_NULL, 			// NULL ...


		// built in C types...
		TYPE_ID_VOID_C,
		TYPE_ID_INT_C,
		TYPE_ID_INT_PTR_C,
		TYPE_ID_CHAR_PTR_C,
		TYPE_ID_CONST_CHAR_PTR_C,
		TYPE_ID_ZS_STRING_C,
		TYPE_ID_ZS_STRING_PTR_C,
		TYPE_ID_BOOL_C,
		TYPE_ID_BOOL_PTR_C,
		TYPE_ID_FLOAT_C,
		TYPE_ID_FLOAT_PTR_C,
		TYPE_ID_CONST_FLOAT_PTR_C,

		TYPE_ID_STACK_ELEMENT,
		TYPE_ID_FUNCTION, // function a(){}
		TYPE_ID_OBJECT_VAR_REF,
		TYPE_ID_OBJECT_FUNCTION_MEMBER,
		TYPE_ID_OBJECT_ZETSCRIPT, // zetscript
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// BUILT-IN CLASSES
		//
		TYPE_ID_OBJECT_STRING, // "String"

		// Iterator
		TYPE_ID_OBJECT_ITERATOR_STRING, // "IteratorString"
		TYPE_ID_OBJECT_ITERATOR_ARRAY, // IteratorArray []
		TYPE_ID_OBJECT_ITERATOR_OBJECT, // IteratorObject
		//
		// BUILT-IN CLASSES
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// CONTAINER CLASSES
		//
		TYPE_ID_OBJECT_ARRAY, // Array []

		// Object & type
		TYPE_ID_OBJECT_OBJECT, //  Object {}
		TYPE_ID_OBJECT_CLASS, // class A{}

		//
		// CONTAINER CLASSES
		//
		//--------------------------------------------------------------------------------------------------------------------->
		TYPE_ID_MAX

	};
}
