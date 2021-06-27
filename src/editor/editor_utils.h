#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_EDITOR_UTILS_H
#define ISOMETRIC_MAPS_EDITOR_UTILS_H

#include <core/ustring.h>

namespace editor {
    class EditorUtils {
    public:
        static Error find_all_positionables_in_path(const String& path, List<String>* r_value);
    };
}


#endif //ISOMETRIC_MAPS_EDITOR_UTILS_H

#endif
