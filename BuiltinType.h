/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


namespace zetscript{

	typedef enum:short{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// built-in types...
		IDX_BUILTIN_TYPE_MAIN = 0, 	// Main class ...

		// built in C types...
		IDX_BUILTIN_TYPE_VOID_C,

		IDX_BUILTIN_TYPE_ZS_INT_C,
		IDX_BUILTIN_TYPE_ZS_INT_PTR_C,
		IDX_BUILTIN_TYPE_CHAR_PTR_C,
		IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C,
		IDX_BUILTIN_TYPE_STRING_PTR_C,
		IDX_BUILTIN_TYPE_BOOL_C,
		IDX_BUILTIN_TYPE_BOOL_PTR_C,
		IDX_BUILTIN_TYPE_ZS_FLOAT_C,
		IDX_BUILTIN_TYPE_ZS_FLOAT_PTR_C,
		IDX_BUILTIN_TYPE_CONST_ZS_FLOAT_PTR_C,

		IDX_BUILTIN_TYPE_STACK_ELEMENT,
		IDX_BUILTIN_TYPE_FUNCTION, // function a(){}
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF,
		//----------------------------------------------------------------->
		// OBJECTS STARTS FROM IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING DONT'T MOVE!!!!!
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING, // "String"
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR, // Vector []
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_DATETIME, // DateTime

		// Iterator
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR, // "StringIterator"
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR, // VectorIterator []
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR, // ObjectIterator

		// Object & class
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT, //  Object {}
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS, // class A{}
		IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS_ZETSCRIPT, // zetscript
		// !!!
		// !!! DO NOT DECLARE NOTHING HERE!! RESERVED FOR USER CLASSES !!!!!
		// !!!
		IDX_BUILTIN_TYPE_MAX
	}BuiltinType;

	BuiltinType	is_built_in_type(const char *s);

}
