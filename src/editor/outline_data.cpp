#ifdef TOOLS_ENABLED

#include "outline_data.h"

#include "servers/rendering_server.h"

using namespace editor;

OutlineData::OutlineData() :
  rid(RenderingServer::get_singleton()->canvas_item_create()),
  is_visible(false),
  color(),
  line_size(),
  should_draw_polygons(false) {}

#endif