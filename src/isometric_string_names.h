#ifndef ISOMETRIC_MAPS_ISOMETRIC_STRING_NAMES_H
#define ISOMETRIC_MAPS_ISOMETRIC_STRING_NAMES_H

#include "core/os/memory.h"
#include "core/string/string_name.h"
#include "core/typedefs.h"

class IsometricStringNames {

public:

    _FORCE_INLINE_ static IsometricStringNames *get_singleton() { return singleton; }
    static void create() { singleton = memnew(IsometricStringNames); }
    static void free() {
        memdelete(singleton);
        singleton = nullptr;
    }

    static IsometricStringNames *singleton;

    StringName debug_group_name { StaticCString::create("isometric_debug_view")};
    StringName size_changed_signal { StaticCString::create("size_changed")};

    IsometricStringNames();
};

#endif// ISOMETRIC_MAPS_ISOMETRIC_STRING_NAMES_H
