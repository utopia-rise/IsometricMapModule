#include "isometric_positionable.h"

#include "editor/outline_drawer.h"
#include "isometric_server.h"
#include "utils/isometric_maths.h"

using namespace node;

StringName IsometricPositionable::debug_group_name { StaticCString::create("isometric_debug_view")};
StringName IsometricPositionable::size_changed_signal { StaticCString::create("size_changed")};

IsometricPositionable::IsometricPositionable() :
  Node2D(),
  size({1, 1, 1}),
  depth(1),
  self(RID()),
  is_dynamic(false),
  collision_object_node_path(),
  collision_object(nullptr),
  world(RID()),
  world_owner(false),
  is_container {false}
#ifdef TOOLS_ENABLED
  ,
  outline_data()
#endif
{
    set_process(true);
}

void IsometricPositionable::_enter_tree() {
    add_to_group(debug_group_name);
    if (!collision_object_node_path.is_empty()) {
        if (auto* collider {Object::cast_to<CollisionObject3D>(get_node(collision_object_node_path))}) {
            collision_object = collider;
            _rebind_collision_object_position();
        } else {
            LOG_ERROR(vformat(
              "Positionable %s collision_object_node_path does not point to "
              "valid CollisionObject",
              this->get_instance_id()
            ));
        }
    }

    if (const Node * parent {get_parent()}) {
        if (auto positionable {cast_to<IsometricPositionable>(parent)}) {
            if (positionable->world != RID()) {
                world = positionable->world;
                world_owner = false;
            }
        }
    }

    if (world == RID()) {
        world = ISOMETRIC_SERVER->space_create();
        world_owner = true;
    }

    if (!is_container) {
        self = ISOMETRIC_SERVER->isometric_element_create(is_dynamic, {get_global_position_3d(), size});
        ISOMETRIC_SERVER->isometric_element_attach_canvas_item(self, get_canvas_item(), depth);
        ISOMETRIC_SERVER->space_attach_isometric_element(world, self);
    }
    update_position();
}

void IsometricPositionable::_ready() {
    if (is_dynamic && collision_object && !Engine::get_singleton()->is_editor_hint()) { set_physics_process(true); }
}

void IsometricPositionable::_physics_process() {
    Vector3 collision_origin {collision_object->get_global_transform().origin};
    collision_origin = {collision_origin.x, collision_origin.z, collision_origin.y};
    if (!collision_origin.is_equal_approx(get_global_position_3d())) { set_global_position_3d(collision_origin); }
}

void IsometricPositionable::_exit_tree() {
    if (self != RID()) {
        ISOMETRIC_SERVER->isometric_element_detach_canvas_item(self);
        ISOMETRIC_SERVER->free_rid(self);
        if (world_owner) {
            world_owner = false;
            ISOMETRIC_SERVER->free_rid(world);
        }
        world = RID();
        self = RID();
    }
    remove_from_group(debug_group_name);
}

void IsometricPositionable::update_position() {
    if (world.is_valid()) {
        const data::IsometricParameters* params = ISOMETRIC_SERVER->space_get_configuration(world);
        Vector2 position2D = utils::from_3D_to_screen(*params, local_position);

        Transform2D transform = get_transform();
        transform.set_origin(position2D);
        set_transform(transform);
    }
}

Vector3 IsometricPositionable::get_local_position_3d() const {
    return local_position;
}

void IsometricPositionable::set_local_position_3d(Vector3 p_local) {
    local_position = p_local;
    if (self.is_valid() && is_dynamic) {
        IsometricServer::get_instance()->isometric_element_set_position(self, get_global_position_3d());
    }
    update_position();
    _rebind_collision_object_position();
}

Vector3 IsometricPositionable::get_global_position_3d() const {
    Vector3 global_position = local_position;

    auto* parent = Object::cast_to<IsometricPositionable>(get_parent());
    if (parent) { global_position += parent->get_global_position_3d(); }
    return global_position;
}

void IsometricPositionable::set_global_position_3d(const Vector3& p_position) {
    const Vector3& offset {p_position - get_global_position_3d()};
    set_local_position_3d(local_position + offset);
}

Vector3 IsometricPositionable::get_size() const {
    return size;
}

void IsometricPositionable::set_size(Vector3 s) {
    size = s;
    if (self.is_valid() && is_dynamic) {
        IsometricServer::get_instance()->isometric_element_set_size(self, size);
#ifdef TOOLS_ENABLED
        editor::OutlineDrawer::draw_outline(this);
#endif
    }
    _rebind_collision_object_position();

#ifdef TOOLS_ENABLED
    emit_signal(size_changed_signal);
#endif
}

int IsometricPositionable::get_depth() const {
    return depth;
}

void IsometricPositionable::set_depth(int p_depth) {
    depth = p_depth;
    if (self.is_valid() && is_dynamic) { ISOMETRIC_SERVER->isometric_element_set_depth(self, p_depth); }
}

void IsometricPositionable::_notification(int notif) {
    switch (notif) {
        case NOTIFICATION_ENTER_TREE:
            _enter_tree();
            break;
        case NOTIFICATION_EXIT_TREE:
            _exit_tree();
            break;
        case NOTIFICATION_READY:
            _ready();
            break;
        case NOTIFICATION_PHYSICS_PROCESS:
            _physics_process();
            break;
        default:
            break;
    }
}

RID IsometricPositionable::get_space_RID() const {
    return world;
}

IsometricPositionable::SlopeType IsometricPositionable::get_slope_type() const {
    return slope_type;
}

bool IsometricPositionable::get_is_dynamic() const {
    return is_dynamic;
}

void IsometricPositionable::set_is_dynamic(bool p_is_dynamic) {
    if (self.is_valid() && p_is_dynamic != is_dynamic) {
        ISOMETRIC_SERVER->free_rid(self);
        self = ISOMETRIC_SERVER->isometric_element_create(is_dynamic, {get_global_position_3d(), size});
        ISOMETRIC_SERVER->isometric_element_attach_canvas_item(self, get_canvas_item(), depth);
        update_position();
        ISOMETRIC_SERVER->space_attach_isometric_element(world, self);
    }
    is_dynamic = p_is_dynamic;
}

const NodePath& IsometricPositionable::get_collision_object_node_path() const {
    return collision_object_node_path;
}

void IsometricPositionable::set_collision_object_node_path(const NodePath& p_node_path) {
    collision_object_node_path = p_node_path;
}

void IsometricPositionable::_rebind_collision_object_position() const {
    if (!collision_object) { return; }
    const Vector3& global_position {get_global_position_3d()};
    collision_object->set_global_transform(
      {{1, 0, 0, 0, 1, 0, 0, 0, 1}, {global_position.x, global_position.z, global_position.y}}
    );
}

RID IsometricPositionable::get_rid() const {
    return self;
}

#ifdef TOOLS_ENABLED

editor::OutlineData& IsometricPositionable::get_outline_data() {
    return outline_data;
}

void IsometricPositionable::set_debug_view(bool p_debug) {
    debug_view = p_debug;
    if (is_container) { return; }
    for (int i = 0; i < get_child_count(); i++) {
        CanvasItem* item {cast_to<CanvasItem>(get_child(i))};
        if (item && !cast_to<IsometricPositionable>(item)) { item->set_visible(!p_debug); }
    }
    outline_data.should_draw_polygons = p_debug;
    editor::OutlineDrawer::set_outline_visible(this, p_debug);
    if (p_debug) { editor::OutlineDrawer::draw_outline(this); }
    queue_redraw();
}

#endif

void IsometricPositionable::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_local_position_3d", "p_local"), &IsometricPositionable::set_local_position_3d);
    ClassDB::bind_method(D_METHOD("get_local_position_3d"), &IsometricPositionable::get_local_position_3d);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "local_position_3d"), "set_local_position_3d", "get_local_position_3d");

    ClassDB::bind_method(D_METHOD("set_size", "s"), &IsometricPositionable::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &IsometricPositionable::get_size);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "size"), "set_size", "get_size");
    ADD_PROPERTY_DEFAULT("size", Vector3(1, 1, 1));

    ClassDB::bind_method(D_METHOD("set_depth", "d"), &IsometricPositionable::set_depth);
    ClassDB::bind_method(D_METHOD("get_depth"), &IsometricPositionable::get_depth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "depth"), "set_depth", "get_depth");
    ADD_PROPERTY_DEFAULT("depth", 1);

    ClassDB::bind_method(D_METHOD("set_is_dynamic", "p_is_dynamic"), &IsometricPositionable::set_is_dynamic);
    ClassDB::bind_method(D_METHOD("get_is_dynamic"), &IsometricPositionable::get_is_dynamic);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_dynamic"), "set_is_dynamic", "get_is_dynamic");

    ClassDB::bind_method(D_METHOD("set_collision_object_node_path", "p_node_path"), &IsometricPositionable::set_collision_object_node_path);
    ClassDB::bind_method(D_METHOD("get_collision_object_node_path"), &IsometricPositionable::get_collision_object_node_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "collision_object_node_path"), "set_collision_object_node_path", "get_collision_object_node_path");

#ifdef TOOLS_ENABLED
    ClassDB::bind_method(D_METHOD("set_debug_view"), &IsometricPositionable::set_debug_view);
    ADD_SIGNAL(MethodInfo("size_changed"));
#endif
}