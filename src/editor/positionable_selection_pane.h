#ifdef TOOLS_ENABLED

#ifndef ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H
#define ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#include <scene/gui/split_container.h>
#include <modules/isometric_maps/src/resource/positionable_set.h>

namespace editor {
    class PositionableSelectionPane : public HSplitContainer {
    public:
        Ref<resource::PositionableSet> get_positionable_set() const;
        void set_positionable_set(const Ref<resource::PositionableSet>& set);

        PositionableSelectionPane();
        ~PositionableSelectionPane() override = default;

    private:
        Ref<resource::PositionableSet> positionable_set;
    };
}

#endif //ISOMETRIC_MAPS_POSITIONABLE_SELECTION_PANE_H

#endif
