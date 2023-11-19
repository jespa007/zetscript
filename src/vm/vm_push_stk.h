/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define ZS_VM_PUSH_STK_UNDEFINED \
		ZS_STK_SET_UNDEFINED(data->vm_stk_current);\
		data->vm_stk_current++; \

#define ZS_VM_PUSH_STK_NULL \
		ZS_STK_SET_NULL(data->vm_stk_current);\
		data->vm_stk_current++; \

#define ZS_VM_PUSH_STK_BOOLEAN(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=ZS_STK_PROPERTY_BOOL; \
data->vm_stk_current++;


#define ZS_VM_PUSH_STK_INT(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=ZS_STK_PROPERTY_INT; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_SCRIPT_OBJECT(obj_value) \
data->vm_stk_current->value=(zs_int)obj_value; \
data->vm_stk_current->properties=ZS_STK_PROPERTY_SCRIPT_OBJECT; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_FLOAT(init_value) \
	ZS_WRITE_INTPTR_FLOAT(&data->vm_stk_current->value,init_value);\
	data->vm_stk_current->properties=ZS_STK_PROPERTY_FLOAT; \
	data->vm_stk_current++; \


#define ZS_VM_PUSH_STK_SCRIPT_FUNCTION(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=ZS_STK_PROPERTY_FUNCTION; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_TYPE(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=ZS_STK_PROPERTY_INDEX_CLASS_TYPE; \
data->vm_stk_current++;


#define ZS_VM_PUSH_STK_PTR(stk_ptr) \
	data->vm_stk_current->value=(intptr_t)(stk_ptr);\
	data->vm_stk_current->properties=ZS_STK_PROPERTY_PTR_STK;\
	data->vm_stk_current++;

#define ZS_VM_PUSH_NEW_CONTAINER_SLOT(so_aux,str_symbol_aux1,stk_slot) \
{\
	data->vm_stk_current->value=(intptr_t)(ContainerSlot::newContainerSlot(\
		(ContainerScriptObject *)so_aux\
		,(zs_int)str_symbol_aux1\
		,stk_slot\
		)\
	);\
	data->vm_stk_current->properties=ZS_STK_PROPERTY_CONTAINER_SLOT;\
	data->vm_stk_current++;\
}
