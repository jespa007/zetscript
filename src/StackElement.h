/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

// properties shared by compiler + VM
typedef enum:unsigned short {
	//-- COMPILER/VM TYPE VAR
	STK_PROPERTY_NULL				=0x0000,
	STK_PROPERTY_ZS_CHAR			=0x0001,
	STK_PROPERTY_ZS_INT				=0x0002,
	STK_PROPERTY_ZS_FLOAT			=0x0004,
	STK_PROPERTY_BOOL				=0x0008,
	STK_PROPERTY_FUNCTION			=0x0010,
	STK_PROPERTY_MEMBER_FUNCTION	=0x0020,
	STK_PROPERTY_MEMBER_ATTRIBUTE	=0x0040,
	STK_PROPERTY_CLASS				=0x0080,
	STK_PROPERTY_SCRIPT_OBJECT		=0x0100,
	STK_PROPERTY_MAX				=0x0200,
	//-- VM RUNTIME
	STK_PROPERTY_IS_VAR_C 			= STK_PROPERTY_MAX, 		// 0x0200 ptr to C
	STK_PROPERTY_PTR_STK			=0x0400,	 				// 0x0400
	STK_PROPERTY_READ_ONLY			=0x0800

}StkProperty;


#define IS_STK_FUNCTION(stk)	((stk->properties & STK_PROPERTY_FUNCTION) || (stk->properties & STK_PROPERTY_MEMBER_FUNCTION))

#define STK_PROPERTY_TYPE_PRIMITIVES					(STK_PROPERTY_FUNCTION-1)
#define GET_STK_PROPERTY_PRIMITIVE_TYPES(prop)				((prop)&STK_PROPERTY_TYPE_PRIMITIVES)

#define STK_PROPERTY_TYPES								(STK_PROPERTY_MAX-1)
#define GET_STK_PROPERTY_TYPES(prop)					((prop)&STK_PROPERTY_TYPES)

#define STK_IS_SCRIPT_OBJECT_OBJECT(stk) 				(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT))
#define STK_IS_SCRIPT_OBJECT_OBJECT_ITERATOR(stk)		(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_OBJECT_ITERATOR))
#define STK_IS_SCRIPT_OBJECT_STRING(stk) 				(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING))
#define STK_IS_SCRIPT_OBJECT_STRING_ITERATOR(stk)		(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING_ITERATOR))
#define STK_IS_SCRIPT_OBJECT_VECTOR(stk) 				(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR))
#define STK_IS_SCRIPT_OBJECT_VECTOR_ITERATOR(stk)		(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR_ITERATOR))
#define STK_IS_SCRIPT_OBJECT_CLASS(stk) 				(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class>IDX_BUILTIN_TYPE_SCRIPT_OBJECT_CLASS))
#define STK_IS_SCRIPT_OBJECT_VAR_REF(stk) 				(((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT) && (((ScriptObject *)(stk)->value)->idx_script_class==IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VAR_REF))
#define STK_GET_STK_VAR_REF(stk)  						(((ScriptObjectVarRef *)((stk))->value)->getStackElementPtr())

#define MSK_STK_OP1_BOOL_OP2_BOOL						((STK_PROPERTY_BOOL<<16)|STK_PROPERTY_BOOL)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_INT					((STK_PROPERTY_ZS_INT<<16)|STK_PROPERTY_ZS_INT)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT					((STK_PROPERTY_ZS_INT<<16)|STK_PROPERTY_ZS_FLOAT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT					((STK_PROPERTY_ZS_FLOAT<<16)|STK_PROPERTY_ZS_INT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT				((STK_PROPERTY_ZS_FLOAT<<16)|STK_PROPERTY_ZS_FLOAT)

#define STK_PROPERTY_ZS_INT_PTR (STK_PROPERTY_IS_VAR_C | STK_PROPERTY_ZS_INT)
#define STK_PROPERTY_FLOAT_PTR (STK_PROPERTY_IS_VAR_C | STK_PROPERTY_ZS_FLOAT)
#define STK_PROPERTY_BOOL_PTR (STK_PROPERTY_IS_VAR_C | STK_PROPERTY_BOOL)

#define STK_PROPERTY_RUNTIME					(((0x1<<(MAX_BIT_RUNTIME-BIT_STK_PROPERTY_IS_VAR_C))-1)<<(BIT_STK_PROPERTY_IS_VAR_C))
#define GET_STK_PROPERTY_RUNTIME(prop)			((prop)&STK_PROPERTY_RUNTIME)

#define STK_VALUE_IS_ZS_NULL(stk) \
(stk->properties == 0)


// Check types
#define STK_VALUE_IS_ZS_FLOAT(stk) \
(stk->properties & STK_PROPERTY_ZS_FLOAT)


#define STK_VALUE_IS_ZS_INT(stk) \
(stk->properties & STK_PROPERTY_ZS_INT)

#define STK_VALUE_IS_BOOLEAN(stk) \
(stk->properties & STK_PROPERTY_BOOL)

#define STK_VALUE_IS_NULL(stk) \
(stk->properties == 0)

#define STK_VALUE_IS_FUNCTION(stk) \
(stk->properties & STK_PROPERTY_FUNCTION)

#define STK_VALUE_IS_CLASS(stk) \
(stk->properties & STK_PROPERTY_CLASS)

#define STK_VALUE_IS_MEMBER_ATTRIBUTE(stk) \
(stk->properties & STK_PROPERTY_MEMBER_ATTRIBUTE)

#define STK_VALUE_IS_MEMBER_FUNCTION(stk) \
(stk->properties & STK_PROPERTY_MEMBER_FUNCTION)

#define STK_VALUE_IS_SCRIPT_VAR(stk) \
(stk->properties & STK_PROPERTY_SCRIPT_OBJECT)

#define STK_VALUE_TO_ZS_INT(ptr_result_instruction) \
		((zs_int)((zs_int)(ptr_result_instruction->value)))

#define STK_VALUE_TO_BOOL(ptr_result_instruction) \
		(((bool)(ptr_result_instruction->value)))

#define STK_SET_NULL(stk) memset(stk,0,sizeof(StackElement))
//#define STK_ASSIGN(dst,src) memcpy(dst,src,sizeof(StackElement))

namespace zetscript{

	#pragma pack(push, 1)

	struct StackElement {
		void * value; // operable value
		uint16_t properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)
		const char * 	typeOf();
		std::string 	toString(const std::string & _format="");
		void 			setUndefined();
		zs_int			toInt();
		zs_float		toFloat();

	};

	#pragma pack(pop)


	extern const StackElement k_stk_undefined;



}
