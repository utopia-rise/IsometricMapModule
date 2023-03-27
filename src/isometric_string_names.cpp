#include "isometric_string_names.h"

IsometricStringNames* IsometricStringNames::singleton = nullptr;

IsometricStringNames::IsometricStringNames() :
  debug_group_name(StaticCString::create("isometric_debug_view")),
  size_changed_signal(StaticCString::create("size_changed")) {}