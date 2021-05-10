#ifdef TOOLS_ENABLED

#include "positionable_selection_pane.h"

using namespace editor;

Ref<resource::PositionableSet> PositionableSelectionPane::get_positionable_set() const {
    return positionable_set;
}

void PositionableSelectionPane::set_positionable_set(const Ref<resource::PositionableSet>& set) {
    positionable_set = set;
}

PositionableSelectionPane::PositionableSelectionPane() : HSplitContainer(), positionable_set() {

}

#endif
