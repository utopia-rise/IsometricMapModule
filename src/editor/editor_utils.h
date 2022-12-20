#ifndef ISOMETRIC_MAPS_EDITOR_UTILS_H
#define ISOMETRIC_MAPS_EDITOR_UTILS_H

#ifdef TOOLS_ENABLED

#include "resource/positionable_set.h"

#include <core/ustring.h>
#include <scene/gui/item_list.h>

namespace editor {
    struct PositionableItemListMetadata : public Reference {
        GDCLASS(PositionableItemListMetadata, Reference)

    public:
        int positionable_id;
        bool is_valid;
    };

    class EditorUtils {
    public:
        static Error find_all_positionables_in_path(const String& path, List<String>* r_value);
        static void refresh_item_list_containing_tiles_for_category(
          const String& category,
          ItemList* item_list,
          const Ref<resource::PositionableSet>& positionable_set,
          Control* calling_control
        );
        static bool item_from_list_has_metadata(ItemList* item_list, int index);
        static bool item_from_list_has_valid_tile_metadata(ItemList* item_list, int index);
        static void refresh_positionable_icons_for_item_list(ItemList* item_list, Control* registered_in_manager);
    };
}// namespace editor

#endif
#endif// ISOMETRIC_MAPS_EDITOR_UTILS_H
