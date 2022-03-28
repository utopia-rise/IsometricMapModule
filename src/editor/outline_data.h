#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_OUTLINE_DATA_H
#define ISOMETRIC_MAPS_OUTLINE_DATA_H


#include <core/rid.h>

namespace editor {
    struct OutlineData {
        RID rid;
        bool is_visible;

        OutlineData();
        ~OutlineData() = default;
    };
}


#endif //ISOMETRIC_MAPS_OUTLINE_DATA_H

#endif
