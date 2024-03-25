#ifndef lox_methods_h
#define lox_methods_h

#include "object.h"

ObjClass *get_bool_class();
ObjClass *get_bound_method_class();
ObjClass *get_float_class();
ObjClass *get_int_class();
ObjClass *get_char_class();
ObjClass *get_class_class();
ObjClass *get_closure_class();
ObjClass *get_function_class();
ObjClass *get_string_class();
ObjClass *get_list_class();
ObjClass *get_map_class();
ObjClass *get_bytes_class();
ObjClass *get_module_class();
ObjClass *get_native_struct_class();
ObjClass *get_nil_class();
ObjClass *get_upvalue_class();
ObjClass *get_native_function_class();

#endif // lox_methods_h
