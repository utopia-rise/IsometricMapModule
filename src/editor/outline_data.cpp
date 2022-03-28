#ifdef TOOLS_ENABLED

#include <servers/visual_server.h>
#include "outline_data.h"

using namespace editor;

OutlineData::OutlineData() : rid(VisualServer::get_singleton()->canvas_item_create()), is_visible(false) {

}

#endif