/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IS_STK_FUNCTION(stk)							((stk->properties & STK_PROPERTY_FUNCTION) || (stk->properties & STK_PROPERTY_MEMBER_FUNCTION))

#define STK_PROPERTY_TYPE_PRIMITIVES					(STK_PROPERTY_FUNCTION-1)
#define GET_STK_PROPERTY_PRIMITIVE_TYPES(prop)			((prop)&STK_PROPERTY_TYPE_PRIMITIVES)

#define STK_PROPERTY_TYPES								(STK_PROPERTY_MAX-1)
#define GET_STK_PROPERTY_TYPES(prop)					((prop)&STK_PROPERTY_TYPES)

#define STK_IS_SCRIPT_OBJECT_OBJECT(stk) 				(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_OBJECT))
#define STK_IS_SCRIPT_OBJECT_ITERATOR_OBJECT(stk)		(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_OBJECT))
#define STK_IS_SCRIPT_OBJECT_STRING(stk) 				(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_STRING))
#define STK_IS_SCRIPT_OBJECT_ITERATOR_STRING(stk)		(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_STRING))
#define STK_IS_SCRIPT_OBJECT_VECTOR(stk) 				(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_VECTOR))
#define STK_IS_SCRIPT_OBJECT_ITERATOR_VECTOR(stk)		(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_ITERATOR_VECTOR))
#define STK_IS_SCRIPT_OBJECT_CLASS(stk) 				(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type>zetscript::IDX_TYPE_SCRIPT_OBJECT_CLASS))
#define STK_IS_SCRIPT_OBJECT_VAR_REF(stk) 				(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_VAR_REF))
#define STK_IS_SCRIPT_OBJECT_MEMBER_FUNCTION(stk)		(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_FUNCTION_MEMBER))
#define STK_IS_SCRIPT_OBJECT_ZETSCRIPT_OBJECT(stk) 		(((stk)->properties & zetscript::STK_PROPERTY_SCRIPT_OBJECT) && (((zetscript::ScriptObject *)(stk)->value)->idx_script_type==zetscript::IDX_TYPE_SCRIPT_OBJECT_CLASS_ZETSCRIPT))

#define STK_GET_STK_VAR_REF(stk)  						(((ScriptObjectVarRef *)((stk))->value)->getStackElementPtr())

#define MSK_STK_OP1_BOOL_OP2_BOOL						((STK_PROPERTY_BOOL<<16)	|	STK_PROPERTY_BOOL)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_INT					((STK_PROPERTY_ZS_INT<<16)	|	STK_PROPERTY_ZS_INT)
#define MSK_STK_OP1_ZS_INT_OP2_ZS_FLOAT					((STK_PROPERTY_ZS_INT<<16)	|	STK_PROPERTY_ZS_FLOAT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_INT					((STK_PROPERTY_ZS_FLOAT<<16)|	STK_PROPERTY_ZS_INT)
#define MSK_STK_OP1_ZS_FLOAT_OP2_ZS_FLOAT				((STK_PROPERTY_ZS_FLOAT<<16)|	STK_PROPERTY_ZS_FLOAT)
#define MSK_STK_OP1_TYPE_OP2_TYPE						((STK_PROPERTY_TYPE<<16)	|	STK_PROPERTY_TYPE)

#define STK_PROPERTY_ZS_INT_PTR 						(STK_PROPERTY_IS_C_VAR_PTR | STK_PROPERTY_ZS_INT)
#define STK_PROPERTY_ZS_FLOAT_PTR 						(STK_PROPERTY_IS_C_VAR_PTR | STK_PROPERTY_ZS_FLOAT)
#define STK_PROPERTY_BOOL_PTR 							(STK_PROPERTY_IS_C_VAR_PTR | STK_PROPERTY_BOOL)

#define STK_PROPERTY_RUNTIME							(((0x1<<(MAX_BIT_RUNTIME-BIT_STK_PROPERTY_IS_C_VAR_PTR))-1)<<(BIT_STK_PROPERTY_IS_C_VAR_PTR))
#define GET_STK_PROPERTY_RUNTIME(prop)					((prop)&STK_PROPERTY_RUNTIME)

#define STK_VALUE_IS_ZS_NULL(stk) 						(stk->properties == 0)


// Check types
#define STK_VALUE_IS_ZS_FLOAT(stk) 						((stk)->properties & STK_PROPERTY_ZS_FLOAT)
#define STK_VALUE_IS_ZS_INT(stk) 						((stk)->properties & STK_PROPERTY_ZS_INT)
#define STK_VALUE_IS_BOOLEAN(stk) 						((stk)->properties & STK_PROPERTY_BOOL)
#define STK_VALUE_IS_NULL(stk) 							((stk)->properties == 0)
#define STK_VALUE_IS_FUNCTION(stk) 						((stk)->properties & STK_PROPERTY_FUNCTION)
#define STK_VALUE_IS_TYPE(stk) 							((stk)->properties & STK_PROPERTY_TYPE)
#define STK_VALUE_IS_MEMBER_PROPERTY(stk) 				((stk)->properties & STK_PROPERTY_MEMBER_PROPERTY)
#define STK_VALUE_IS_MEMBER_FUNCTION(stk) 				((stk)->properties & STK_PROPERTY_MEMBER_FUNCTION)
#define STK_VALUE_IS_SCRIPT_OBJECT(stk) 				((stk)->properties & STK_PROPERTY_SCRIPT_OBJECT)

#define STK_VALUE_TO_BOOL(ptr_result_instruction) 		(((bool)(ptr_result_instruction->value)))
#define STK_SET_NULL(stk)\
	(stk)->value=0;\
	(stk)->properties=0;


namespace zetscript{
	class ZetScript;

	// properties shared by compiler + VM
	typedef enum:unsigned short {
		//---------- BEGIN PRIMITIVE TYPES HERE--------------------
		STK_PROPERTY_NULL				=0x0000,
		STK_PROPERTY_ZS_CHAR			=0x0001,
		STK_PROPERTY_ZS_INT				=0x0002,
		STK_PROPERTY_ZS_FLOAT			=0x0004,
		STK_PROPERTY_BOOL				=0x0008,
		STK_PROPERTY_TYPE				=0x0010,
		//---------- END PRIMITIVE TYPES HERE--------------------
		STK_PROPERTY_FUNCTION			=0x0020,
		STK_PROPERTY_MEMBER_FUNCTION	=0x0040,
		STK_PROPERTY_MEMBER_PROPERTY	=0x0080,
		STK_PROPERTY_SCRIPT_OBJECT		=0x0100,
		STK_PROPERTY_MAX				=0x0200,
		//-- VM RUNTIME
		STK_PROPERTY_IS_C_VAR_PTR 			= STK_PROPERTY_MAX, 		// 0x0200 ptr to C
		STK_PROPERTY_PTR_STK			=0x0400,	 				// 0x0400
		STK_PROPERTY_READ_ONLY			=0x0800

	}StkProperty;

	#pragma pack(push, 1)

	struct StackElement {
		zs_int 			value; // operable value
		uint16_t 		properties; // it tells its properties

		// it gives stackelement as string (the result should be deleted)


		void 			setUndefined();
		zs_int			toInt();
		zs_float		toFloat();

		StackElement	typeOf();

	};

	zs_string 		stk_to_str(ZetScript *_zs, StackElement *_stk,const zs_string & _format="");
	zs_string		stk_to_typeof_str(ZetScript *_zs, StackElement *_stk);


	#pragma pack(pop)


	extern const StackElement k_stk_null;



}
