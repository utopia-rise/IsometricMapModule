#include <modules/isometric_maps/src/isometric_api.h>
#include "isometric_placeholder.h"

using namespace positionable;

static PlaceholderType* default_placeholder_type{nullptr};

IsometricPlaceholder::IsometricPlaceholder()
        : StaticIsometricElement(),
          left_color(),
          right_color(),
          up_color(),
          down_color(),
          side_slope_color(),
          forward_slope_color(),
          backward_slope_color(),
          stair_color() {
    set_placeholder_type(placeholder_type);
    connect("draw", this, "_draw");
}

void IsometricPlaceholder::_draw() {
    int debug_z { get_debug_z() };
    if (debug_z == static_cast<int>(get_size_3d().z)) {
        stair_color.push_back(Color(1, 0.65, 0, 0.2).linear_interpolate(down_color[0], 0.2));
    } else if (debug_z == 0) {
        stair_color.push_back(Color(0.63, 0.13, 0.94, 0.2).linear_interpolate(down_color[0], 0.2));
    } else {
        stair_color.push_back(Color(1, 0, 0, 0.2).linear_interpolate(down_color[0], 0.2));
    }

    switch (static_cast<SlopeType>(get_slope_type())) {
        case SlopeType::NONE:
            break;
        case SlopeType::LEFT:
            up_color = side_slope_color;
            break;
        case SlopeType::RIGHT:
            up_color = side_slope_color;
            break;
        case SlopeType::FORWARD:
            up_color = forward_slope_color;
            break;
        case SlopeType::BACKWARD:
            up_color = backward_slope_color;
            break;
    }

    prepare_points();
    draw_points();
}

void IsometricPlaceholder::on_grid_updated(int stair) {
    set_debug_z(stair - static_cast<int>(get_local_position_3d().z));
    update();
}

void IsometricPlaceholder::on_resize() {
    IsometricElement::on_resize();
    update();
}

Ref<PlaceholderType> IsometricPlaceholder::get_placeholder_type() const {
    return placeholder_type;
}

void IsometricPlaceholder::set_placeholder_type(Ref<PlaceholderType> p_type) {
    if (p_type.is_valid()) {
        placeholder_type = p_type;
    } else {
        if (unlikely(!default_placeholder_type)) {
            default_placeholder_type = memnew(PlaceholderType());
            default_placeholder_type->set_color(Color(1, 1, 1, 1));
            default_placeholder_type->set_type_name("default");
        }
        placeholder_type = Ref<PlaceholderType>(default_placeholder_type);
    }
    update_colors();
    update();
}

void IsometricPlaceholder::draw_points() {
    const Vector3 &position3D {get_local_position_3d() };
    const Vector3 &size { get_size_3d() };

    int debugZ { get_debug_z() };

    if (get_is_temporary()) {
        Color upC = up_color[0];
        Color leC = left_color[0];
        Color riC = right_color[0];
        Color stairC = stair_color[0];
        upC.a = tempAlpha;
        leC.a = tempAlpha;
        riC.a = tempAlpha;
        stairC.a = tempAlpha;
        up_color.set(0, upC);
        left_color.set(0, leC);
        right_color.set(0, riC);
        stair_color.set(0, stairC);
    } else {
        Color upC = up_color[0];
        Color leC = left_color[0];
        Color riC = right_color[0];
        Color stairC = stair_color[0];
        upC.a = 1;
        leC.a = 1;
        riC.a = 1;
        stairC.a = 0.5;
        up_color.set(0, upC);
        left_color.set(0, leC);
        right_color.set(0, riC);
        stair_color.set(0, stairC);
    }
    draw_polygon(up_points, up_color);
    draw_polygon(left_points, left_color);
    draw_polygon(right_points, right_color);

    if (debugZ >= 0) {
        auto sizeZInt = static_cast<int>(size.z);
        if (debugZ <= sizeZInt) {
            draw_polygon(debug_points, stair_color);
            draw_line(debug_points[0], debug_points[1], stair_color[0], 2.0);
            draw_line(debug_points[1], debug_points[2], stair_color[0], 4.0);
            draw_line(debug_points[2], debug_points[3], stair_color[0], 4.0);
            draw_line(debug_points[3], debug_points[0], stair_color[0], 2.0);
        }
        real_t zRatio { IsometricApi::get_instance()->get_z_ratio() };
        int tileWidth { IsometricApi::get_instance()->get_tile_width() };
        int tileHeight { IsometricApi::get_instance()->get_tile_height() };
        float eZ { IsometricApi::get_instance()->get_e_z() };

        int correctedZ { CLAMP(debugZ, 0, sizeZInt) };
        int addedLines { static_cast<int>(ceil(static_cast<double>(zRatio * static_cast<real_t>(correctedZ)))) };

        int zDelta { debugZ - correctedZ };
        auto zDeltaFloat = static_cast<real_t>(zDelta);
        real_t offset { (zRatio - 1) * zDeltaFloat };
        auto tileHeightFloat = static_cast<real_t>(tileHeight);
        auto tileWidthFloat = static_cast<real_t>(tileWidth);
        const Vector2 &base { down_points[0] + Vector2(0, (zDeltaFloat + offset) * tileHeightFloat - eZ * static_cast<real_t>(debugZ)) };

        real_t maxX { map_size.x - position3D.x - zDeltaFloat - offset };
        real_t maxY { map_size.y - position3D.y - zDeltaFloat - offset };

        maxX = MIN(maxX, size.x + static_cast<float>(addedLines) - 1);
        maxY = MIN(maxY, size.y + static_cast<float>(addedLines) - 1);

        if (maxX > 0 && maxY > 0) {
            for (int i = 0; i < static_cast<int>(maxY + 1); i++) {
                auto iFloat = static_cast<real_t>(i);
                const Vector2 &from { base + Vector2(tileWidthFloat * 0.5f - iFloat, tileHeightFloat * 0.5f * iFloat) };
                const Vector2 &to { base + Vector2(tileWidthFloat * 0.5f * (maxX - iFloat), tileHeightFloat * 0.5f * (maxX + iFloat)) };
                draw_line(from, to, Color(0, 0, 0, 1), 2.0);
            }
            for (int i = 0; i < static_cast<int>(maxX + 1); i++) {
                auto iFloat = static_cast<real_t>(i);
                const Vector2 &from { base + Vector2(tileWidthFloat * 0.5f * iFloat, tileHeightFloat * 0.5f * iFloat) };
                const Vector2 &to { base + Vector2(tileWidthFloat * 0.5f * (iFloat - maxY), tileHeightFloat * 0.5f * (maxY + iFloat)) };
                draw_line(from, to, Color(0, 0, 0, 1), 2.0);
            }
        }
    }
}

void IsometricPlaceholder::update_colors() {
    left_color = Vector<Color>();
    left_color.push_back(placeholder_type->get_color().darkened(0.25));
    right_color = Vector<Color>();
    right_color.push_back(placeholder_type->get_color().darkened(0.5));
    up_color = Vector<Color>();
    up_color.push_back(placeholder_type->get_color());
    down_color = Vector<Color>();
    down_color.push_back(placeholder_type->get_color().darkened(0.9));
    side_slope_color = Vector<Color>();
    side_slope_color.push_back(placeholder_type->get_color().darkened(0.10));
    forward_slope_color = Vector<Color>();
    forward_slope_color.push_back(placeholder_type->get_color().darkened(0.10));
    backward_slope_color = Vector<Color>();
    backward_slope_color.push_back(placeholder_type->get_color().lightened(0.10));
}

void IsometricPlaceholder::set_map_size(const Vector3& size) {
    map_size = size;
}

void IsometricPlaceholder::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_placeholder_type"), &IsometricPlaceholder::set_placeholder_type);
    ClassDB::bind_method(D_METHOD("get_placeholder_type"), &IsometricPlaceholder::get_placeholder_type);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "placeholder_type"), "set_placeholder_type", "get_placeholder_type");
    ADD_PROPERTY_DEFAULT("placeholder_type", Ref<positionable::PlaceholderType>(nullptr));
}
