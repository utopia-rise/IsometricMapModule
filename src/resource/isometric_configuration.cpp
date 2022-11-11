#include "isometric_configuration.h"
#include <modules/isometric_maps/src/data/isometric_parameters.h>
#include <modules/isometric_maps/src/logging.h>

using namespace resource;

static RID_Owner<IsometricConfiguration> configurations_owner;

// Todo: Change it so we can set the default values in the editor
IsometricConfiguration::IsometricConfiguration() : tile_width(DEFAULT_WIDTH), angle(DEFAULT_ANGLE), topological_margin(DEFAULT_MARGIN) {
    configurations_owner.make_rid(this);
}

int IsometricConfiguration::get_tile_width() const {
    return tile_width;
}

void IsometricConfiguration::set_tile_width(int t_w) {
    tile_width = t_w;
}

int IsometricConfiguration::get_angle() const {
    return angle;
}

void IsometricConfiguration::set_angle(int agl) {
    angle = agl;
}

float IsometricConfiguration::get_topological_margin() const {
    return topological_margin;
}

void IsometricConfiguration::set_topological_margin(float margin) {
    topological_margin = margin;
}

RID IsometricConfiguration::get_rid() const {
    return self;
}

IsometricConfiguration* IsometricConfiguration::get_instance(const RID rid) {
    IsometricConfiguration* conf {configurations_owner.getornull(rid)};
    if (!conf) {
        LOG_WARNING("This is not a valid isometric configuration RID.");
        return nullptr;
    }
    return conf;
}

void IsometricConfiguration::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_angle"), &IsometricConfiguration::get_angle);
    ClassDB::bind_method(D_METHOD("set_angle"), &IsometricConfiguration::set_angle);
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "angle"), "set_angle", "get_angle");
    ADD_PROPERTY_DEFAULT("angle", DEFAULT_ANGLE);

    ClassDB::bind_method(D_METHOD("get_tile_width"), &IsometricConfiguration::get_tile_width);
    ClassDB::bind_method(D_METHOD("set_tile_width"), &IsometricConfiguration::set_tile_width);
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "diamond_width"), "set_tile_width", "get_tile_width");
    ADD_PROPERTY_DEFAULT("diamond_width", DEFAULT_WIDTH);

    ClassDB::bind_method(D_METHOD("get_margin"), &IsometricConfiguration::get_topological_margin);
    ClassDB::bind_method(D_METHOD("set_margin"), &IsometricConfiguration::set_topological_margin);
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "margin"), "set_margin", "get_margin");
    ADD_PROPERTY_DEFAULT("margin", DEFAULT_MARGIN);
}

IsometricConfiguration::~IsometricConfiguration() {
    configurations_owner.free(self);
}
