/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#define ZS_VM_PUSH_STK_UNDEFINED \
		STACK_ELEMENT_SET_UNDEFINED(data->vm_stk_current);\
		data->vm_stk_current++; \

#define ZS_VM_PUSH_STK_NULL \
		STACK_ELEMENT_SET_NULL(data->vm_stk_current);\
		data->vm_stk_current++; \

#define ZS_VM_PUSH_STK_BOOLEAN(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_BOOL; \
data->vm_stk_current++;


#define ZS_VM_PUSH_STK_INT(init_value) \
data->vm_stk_current->value=init_value; \
data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_INT; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_OBJECT(obj_value) \
data->vm_stk_current->value=(zs_int)obj_value; \
data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_OBJECT; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_FLOAT(init_value) \
	ZS_WRITE_INTPTR_FLOAT(&data->vm_stk_current->value,init_value);\
	data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_FLOAT; \
	data->vm_stk_current++; \


#define ZS_VM_PUSH_STK_FUNCTION(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_FUNCTION; \
data->vm_stk_current++;

#define ZS_VM_PUSH_STK_SCRIPT_TYPE(ref) \
data->vm_stk_current->value=ref; \
data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_SCRIPT_TYPE_ID; \
data->vm_stk_current++;


#define ZS_VM_PUSH_STK_PTR(stk_ptr) \
	data->vm_stk_current->value=(intptr_t)(stk_ptr);\
	data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_PTR_STK;\
	data->vm_stk_current++;

#define ZS_VM_PUSH_NEW_CONTAINER_SLOT(so_aux,str_symbol_aux1,stk_slot) \
{\
	data->vm_stk_current->value=(intptr_t)(ContainerSlot::newContainerSlot(\
		(ContainerScriptObject *)so_aux\
		,(zs_int)str_symbol_aux1\
		,stk_slot\
		)\
	);\
	data->vm_stk_current->properties=STACK_ELEMENT_PROPERTY_CONTAINER_SLOT;\
	data->vm_stk_current++;\
}
