/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

// properties shared by compiler + VM
typedef enum:unsigned char {
	//-- COMPILER/VM TYPE VAR
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED = 0,								// 0x0001
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL,										// 0x0002
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT,									// 0x0004
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT,										// 0x0008
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL,									// 0x0010
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING,										// 0x0020
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION,									// 0x0040
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT,									// 0x0080
	BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_MAX,
	//-- VM RUNTIME
	BIT_STACK_ELEMENT_PROPERTY_IS_VAR_C = BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_MAX, 	// 0x0100
	BIT_STACK_ELEMENT_PROPERTY_PTR_STK,	 											// 0x0200
	BIT_STACK_ELEMENT_PROPERTY_READ_ONLY,				 							// 0x0400
	//BIT_STACK_ELEMENT_PROPERTY_RETURN,											// 0x0800
	MAX_BIT_RUNTIME

}StackElementProperty;

enum
	:unsigned short {
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED), // let's say a variable not defined...
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL), // let's say a variable is null assigned ..
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT), // primitive int
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT), // primitive number
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL), // primitive bool
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING), // constant / script var
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION), // primitive function
	MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT = (0x1 << BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT) // always is an script class...

};

#define MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPE_PRIMITIVES					((0x1<<BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)-1)
#define GET_STACK_ELEMENT_PROPERTY_PRIMITIVE_TYPES(prop)					((prop)&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPE_PRIMITIVES)

#define MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES							((0x1<<BIT_STACK_ELEMENT_PROPERTY_VAR_TYPE_MAX)-1)
#define GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(prop)					((prop)&MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES)

enum:unsigned short {
	MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C = (0x1 << BIT_STACK_ELEMENT_PROPERTY_IS_VAR_C),
	MSK_STACK_ELEMENT_PROPERTY_PTR_STK = (0x1 << BIT_STACK_ELEMENT_PROPERTY_PTR_STK),
	MSK_STACK_ELEMENT_PROPERTY_READ_ONLY = (0x1 << BIT_STACK_ELEMENT_PROPERTY_READ_ONLY),
	//MSK_STACK_ELEMENT_PROPERTY_RETURN = (0x1 << BIT_STACK_ELEMENT_PROPERTY_RETURN)
};

#define MSK_STACK_ELEMENT_PROPERTY_RUNTIME					(((0x1<<(MAX_BIT_RUNTIME-BIT_STACK_ELEMENT_PROPERTY_IS_VAR_C))-1)<<(BIT_STACK_ELEMENT_PROPERTY_IS_VAR_C))
#define GET_MSK_STACK_ELEMENT_PROPERTY_RUNTIME(prop)		((prop)&MSK_STACK_ELEMENT_PROPERTY_RUNTIME)


#define STK_VALUE_IS_FLOAT_OR_INT(stk)\
	(stk->properties & (MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT|MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT))

// Check types
#define STK_VALUE_IS_FLOAT(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT)


#define STK_VALUE_IS_ZS_INT(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT)


#define STK_VALUE_IS_STRING(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING)

#define STK_VALUE_IS_CHAR_PTR(stk) \
(stk->properties & STK_PROPERTY_TYPE_CHAR_PTR)

#define STK_VALUE_IS_BOOLEAN(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL)

#define STK_VALUE_IS_UNDEFINED(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED)

#define STK_VALUE_IS_NULL(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_NULL)

#define STK_VALUE_IS_FUNCTION(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION)

#define STK_VALUE_IS_SCRIPT_VAR(stk) \
(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT)

#define STK_VALUE_IS_VECTOR(stk) \
(( stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPT_OBJECT) &&\
 (((ScriptObject *)(stk->stk_value))->idx_class==IDX_BUILTIN_TYPE_CLASS_SCRIPT_OBJECT_VECTOR))

/*#define STK_VALUE_IS_INT_OR_FLOAT(properties) \
((properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT) ||\
(properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT))*/

#define STK_VALUE_TO_ZS_INT(ptr_result_instruction) \
		((zs_int)((zs_int)(ptr_result_instruction->stk_value)))

#define STK_VALUE_TO_BOOL(ptr_result_instruction) \
		(((bool)(ptr_result_instruction->stk_value)))


#define STK_VALUE_TO_STRING(ptr_result_instruction) \
		(((const char *)(ptr_result_instruction->stk_value)))

namespace zetscript{

	#pragma pack(push, 1)

	struct StackElement {
		//VALUE_INSTRUCTION_TYPE 		type; // tells what kind of variable is. By default is object.
		void * stk_value; // operable value
		void * var_ref; // stack ref in case to assign new value.
		unsigned short properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)
		const char * toString();

	};

	#pragma pack(pop)


	typedef StackElement ConstantValue;


}
