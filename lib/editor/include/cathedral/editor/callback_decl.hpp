#pragma once

#include <boost/preprocessor.hpp>
#include <functional>

#define CATHEDRAL_CALLBACKS_STRUCT_NAME callbacks
#define CATHEDRAL_CALLBACK(name, ...) std::function<void(__VA_ARGS__)> name = [](__VA_ARGS__) { return; }

#define CATHEDRAL_BPP_DECLARE_CALLBACKS_MACRO_(r, data, elem) CATHEDRAL_CALLBACK elem;
#define CATHEDRAL_DECLARE_CALLBACKS(...)                                                                                    \
    struct                                                                                                                  \
    {                                                                                                                       \
        BOOST_PP_SEQ_FOR_EACH(CATHEDRAL_BPP_DECLARE_CALLBACKS_MACRO_, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))             \
    } CATHEDRAL_CALLBACKS_STRUCT_NAME
