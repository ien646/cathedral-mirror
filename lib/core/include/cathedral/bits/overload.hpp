#pragma once

#define CATHEDRAL_OVERLOAD(class_, member_, rettype_, args_) static_cast<rettype_ (class_::*)(args_)>(&class_::member_)

#define CATHEDRAL_OVERLOAD_CONST(class_, member_, rettype_, args_)                                                          \
    static_cast<rettype_ (class_::*)args_ const>(&class_::member_)
