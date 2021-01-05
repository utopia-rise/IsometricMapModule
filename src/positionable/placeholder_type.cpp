#include "placeholder_type.h"

using namespace positionable;

String PlaceholderType::get_type_name() const {
    return type_name;
}

void PlaceholderType::set_type_name(const String& name) {
    type_name = name;
}

Color PlaceholderType::get_color() const {
    return color;
}

void PlaceholderType::set_color(Color col) {
    color = col;
}

void PlaceholderType::_bind_methods() {

}
