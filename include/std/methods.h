#ifndef lox_methods_h
#define lox_methods_h

#include "object.h"

ObjClass *get_bool_class();
ObjClass *get_float_class();
ObjClass *get_int_class();
ObjClass *get_char_class();
ObjClass *get_string_class();
ObjClass *get_list_class();
ObjClass *get_map_class();
ObjClass *get_bytes_class();

#endif // lox_methods_h
