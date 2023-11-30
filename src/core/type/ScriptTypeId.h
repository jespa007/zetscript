/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_SCRIPT_TYPE_NAME_UNKNOWN							"UnknownScriptType"
#define ZS_SCRIPT_TYPE_NAME_UNDEFINED						"Undefined"
#define ZS_SCRIPT_TYPE_NAME_NULL 							"Null"
#define ZS_SCRIPT_TYPE_NAME_VOID							"__@void@__"
#define ZS_SCRIPT_TYPE_NAME_INT 							"Integer"
#define ZS_SCRIPT_TYPE_NAME_INT_PTR							"__@zs_int_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_CHAR_PTR						"__@char_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_CONST_CHAR_PTR					"__@const_char_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_STRING							"__@String@__"
#define ZS_SCRIPT_TYPE_NAME_STRING_PTR						"__@String_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_BOOL 							"Boolean"
#define ZS_SCRIPT_TYPE_NAME_BOOL_PTR						"__@bool_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_STACK_ELEMENT 					"StackElement"
#define ZS_SCRIPT_TYPE_NAME_FLOAT 							"Float"
#define	ZS_SCRIPT_TYPE_NAME_FLOAT_PTR						"__@zs_float_ptr@__"
#define	ZS_SCRIPT_TYPE_NAME_CONST_FLOAT_PTR					"__@const_zs_float_ptr@__"
#define ZS_SCRIPT_TYPE_NAME_SCRIPT_FUNCTION					"__@ScriptFunction@__"
#define ZS_SCRIPT_TYPE_NAME_SCRIPT_OBJECT					"__@ScriptObject@__"
#define ZS_SCRIPT_TYPE_NAME_CONTAINER_SLOT_SCRIPT_OBJECT	"__@ContainerSlot@__"
#define ZS_SCRIPT_TYPE_NAME_STRING_SCRIPT_OBJECT 			"String"
#define ZS_SCRIPT_TYPE_NAME_ARRAY_SCRIPT_OBJECT 			"__@Array@__"
#define ZS_SCRIPT_TYPE_NAME_DICTIONARY_SCRIPT_OBJECT 		"__@DictionaryObject@__"
#define ZS_SCRIPT_TYPE_NAME_CLASS_SCRIPT_OBJECT 			"__@ObjectClass@__"
#define ZS_SCRIPT_TYPE_NAME_VAR_REF_SCRIPT_OBJECT 			"__@VarRef@__"
#define ZS_SCRIPT_TYPE_NAME_MEMBER_FUNCTION_SCRIPT_OBJECT 	"__@MemberFunction@__"
#define ZS_SCRIPT_TYPE_NAME_STRING_ITERATOR_SCRIPT_OBJECT 	"__@IteratorString@__"
#define ZS_SCRIPT_TYPE_NAME_ARRAY_ITERATOR_SCRIPT_OBJECT 	"__@IteratorArray@__"
#define ZS_SCRIPT_TYPE_NAME_DICTIONARY_ITERATOR_SCRIPT_OBJECT 	"__@IteratorObject@__"


namespace zetscript{

	typedef int ScriptTypeId;

	enum: int16_t{

		// !!! HERE WE DEFINE THE BUILTINT CLASSES
		// !!! WARNING THE REGISTER HAS TO RESPECT THE SAME ORDER
		// primitive types...
		SCRIPT_TYPE_ID_INVALID=-1,      // Invalid
		SCRIPT_TYPE_ID_CLASS_MAIN =0, 	// Main type ...
		SCRIPT_TYPE_ID_UNDEFINED, 		// UNDEFINED ...
		SCRIPT_TYPE_ID_NULL, 			// NULL ...


		// built in C types...
		SCRIPT_TYPE_ID_VOID,
		SCRIPT_TYPE_ID_INT,
		SCRIPT_TYPE_ID_INT_PTR,
		SCRIPT_TYPE_ID_CHAR_PTR,
		SCRIPT_TYPE_ID_CONST_CHAR_PTR,
		SCRIPT_TYPE_ID_STRING,
		SCRIPT_TYPE_ID_STRING_PTR,
		SCRIPT_TYPE_ID_BOOL,
		SCRIPT_TYPE_ID_BOOL_PTR,
		SCRIPT_TYPE_ID_FLOAT,
		SCRIPT_TYPE_ID_FLOAT_PTR,
		SCRIPT_TYPE_ID_CONST_FLOAT_PTR,

		SCRIPT_TYPE_ID_STACK_ELEMENT,
		SCRIPT_TYPE_ID_SCRIPT_FUNCTION, // function a(){}
		SCRIPT_TYPE_ID_VAR_REF_SCRIPT_OBJECT,
		SCRIPT_TYPE_ID_MEMBER_FUNCTION_SCRIPT_OBJECT,
		SCRIPT_TYPE_ID_ZETSCRIPT_SCRIPT_OBJECT, // zetscript
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// BUILT-IN CLASSES
		//
		SCRIPT_TYPE_ID_STRING_SCRIPT_OBJECT, // "String"

		// Iterator
		SCRIPT_TYPE_ID_STRING_ITERATOR_SCRIPT_OBJECT, // "IteratorString"
		SCRIPT_TYPE_ID_ARRAY_ITERATOR_SCRIPT_OBJECT, // IteratorArray []
		SCRIPT_TYPE_ID_DICTIONARY_ITERATOR_SCRIPT_OBJECT, // IteratorObject
		//
		// BUILT-IN CLASSES
		//--------------------------------------------------------------------------------------------------------------------->
		//
		// CONTAINER CLASSES
		//
		SCRIPT_TYPE_ID_ARRAY_SCRIPT_OBJECT, // Array []

		// ScriptObject & type
		SCRIPT_TYPE_ID_DICTIONARY_SCRIPT_OBJECT, //  ScriptObject {}
		SCRIPT_TYPE_ID_CLASS_SCRIPT_OBJECT, // class A{}

		//
		// CONTAINER CLASSES
		//
		//--------------------------------------------------------------------------------------------------------------------->
		SCRIPT_TYPE_ID_MAX

	};
}
