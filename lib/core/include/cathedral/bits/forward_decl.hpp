#pragma once

#define FORWARD_CLASS(_namespace, type)                                                                                     \
    namespace _namespace                                                                                                    \
    {                                                                                                                       \
        class type;                                                                                                         \
    }
#define FORWARD_CLASS_INLINE(type) class type;
#define FORWARD_STRUCT(_namespace, type)                                                                                    \
    namespace _namespace                                                                                                    \
    {                                                                                                                       \
        struct type;                                                                                                        \
    }
#define FORWARD_STRUCT_INLINE(type) struct type;