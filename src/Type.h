/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_TYPE_NAME_NULL 						"null"
#define ZS_TYPE_NAME_INT 						"int"
#define ZS_TYPE_NAME_BOOL 						"bool"
#define ZS_TYPE_NAME_FLOAT 						"float"
#define ZS_TYPE_NAME_OBJECT_STRING 				"String"
#define ZS_TYPE_NAME_OBJECT_VECTOR 				"Vector"
#define ZS_TYPE_NAME_OBJECT_OBJECT 				"Object"
#define ZS_TYPE_NAME_OBJECT_VAR_REF 			"VarRef"
#define ZS_TYPE_NAME_OBJECT_FUNCTION_MEMBER 	"FunctionMember"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_STRING 	"IteratorString"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_VECTOR 	"IteratorVector"
#define ZS_TYPE_NAME_OBJECT_ITERATOR_OBJECT 	"IteratorObject"

namespace zetscript{

	typedef enum:short{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		IDX_TYPE_CLASS_MAIN =0, 	// Main class ...
		IDX_TYPE_NULL, 		// NULL ...


		// built in C types...
		IDX_TYPE_VOID_C,
		IDX_TYPE_ZS_INT_C,
		IDX_TYPE_ZS_INT_PTR_C,
		IDX_TYPE_CHAR_PTR_C,
		IDX_TYPE_CONST_CHAR_PTR_C,
		IDX_TYPE_STRING_C,
		IDX_TYPE_STRING_PTR_C,
		IDX_TYPE_BOOL_C,
		IDX_TYPE_BOOL_PTR_C,
		IDX_TYPE_ZS_FLOAT_C,
		IDX_TYPE_ZS_FLOAT_PTR_C,
		IDX_TYPE_CONST_ZS_FLOAT_PTR_C,

		IDX_TYPE_STACK_ELEMENT,
		IDX_TYPE_FUNCTION, // function a(){}
		IDX_TYPE_SCRIPT_OBJECT_VAR_REF,
		IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER,
		//--------------------------------------------------------------------------------------------------------------------->
		// RESERVED FOR ZETSCRIPT BUILT-IN CLASSES STARTS STARTS FROM "IDX_TYPE_SCRIPT_OBJECT_STRING". PLEASE DONT'T MOVE!!!!!
		IDX_TYPE_SCRIPT_OBJECT_STRING, // "String"
		IDX_TYPE_SCRIPT_OBJECT_VECTOR, // Vector []

		// Iterator
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING, // "IteratorString"
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR, // IteratorVector []
		IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT, // IteratorObject

		// Object & class
		IDX_TYPE_SCRIPT_OBJECT_OBJECT, //  Object {}
		IDX_TYPE_SCRIPT_OBJECT_CLASS, // class A{}
		IDX_TYPE_SCRIPT_OBJECT_CLASS_ZETSCRIPT, // zetscript

		// !!!
		// !!! DO NOT DECLARE NOTHING HERE!! RESERVED FOR ZETSCRIPT BUILT-IN CLASSES !!!!!
		// !!!
		//--------------------------------------------------------------------------------------------------------------------->
		IDX_TYPE_MAX
	}Type;

	Type	is_built_in_type(const char *s);

}
