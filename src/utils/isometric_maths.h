//
// Created by CedNaru on 14/02/2021.
//

#ifndef ISOMETRIC_MAPS_ISOMETRIC_MATHS_H
#define ISOMETRIC_MAPS_ISOMETRIC_MATHS_H

#include "../data/isometric_parameters.h"

namespace utils {
    struct Hexagone {
        real_t minX{};
        real_t maxX{};
        real_t minY{};
        real_t maxY{};
        real_t minH{};
        real_t maxH{};
    };


    static inline Vector2 from_3D_to_screen(const data::IsometricParameters &params, const Vector3 &pos) {
        return {
                (pos.x - pos.y) * static_cast<real_t>(params.diamond_width) * 0.5f,
                (pos.x + pos.y) * static_cast<real_t>(params.diamond_height) * 0.5f - params.z_length * pos.z
        };
    }

    static Vector3 from_screen_to_3D(const data::IsometricParameters &params, const Vector2 &pos, real_t orth_z) {
        real_t iso_x{pos.x};
        real_t iso_y{pos.y};
        auto tile_width_float = static_cast<real_t>(params.diamond_width);
        auto tile_height_float = static_cast<real_t>(params.diamond_height);
        real_t orth_x{iso_x / tile_width_float + (iso_y + orth_z * params.z_length) / tile_height_float};
        real_t orth_y{(iso_y + orth_z * params.z_length) / tile_height_float - iso_x / tile_width_float};
        return {
                ::roundf(orth_x),
                ::roundf(orth_y),
                ::roundf(orth_z)
        };
    }

    static Hexagone get_hexagone_points(const data::IsometricParameters &params, const data::IsometricElement* data) {
        const Vector3 &ortho_position{data->aabb.position};
        const Vector3 &size{data->aabb.size};
        const Vector3 &upper_point{
                Vector3(ortho_position.x, ortho_position.y, ortho_position.z + params.z_ratio * size.z)};
        const Vector3 &lower_point{Vector3(ortho_position.x + size.x, ortho_position.y + size.y, ortho_position.z)};
        const Vector3 &left_point{Vector3(ortho_position.x, ortho_position.y + size.y, ortho_position.z)};
        const Vector3 &right_point{Vector3(ortho_position.x + size.x, ortho_position.y, ortho_position.z)};

        const real_t minX = upper_point.x - upper_point.z;
        const real_t maxX = lower_point.x - lower_point.z;
        const real_t minY = upper_point.y - upper_point.z;
        const real_t maxY = lower_point.y - lower_point.z;
        const real_t hMin = left_point.x - left_point.y;
        const real_t hMax = right_point.x - right_point.y;

        return {minX, maxX, minY, maxY, hMin, hMax};
    }


    static bool are_elements_overlapping(const data::IsometricParameters &params,
                                  const data::IsometricElement* data1,
                                  const data::IsometricElement* data2) {
        Hexagone hex1 = get_hexagone_points(params, data1);
        Hexagone hex2 = get_hexagone_points(params, data2);

        return !(hex1.minX >= hex2.maxX || hex2.minX >= hex1.maxX) &&
               !(hex1.minY >= hex2.maxY || hex2.minY >= hex1.maxY) &&
               !(hex1.minH >= hex2.maxH || hex2.minH >= hex1.maxH);
    }

    // return 1 ==> box in front of other
    // return 0 ==> colliding
    // return -1 ==> box behind other
    static int is_box_in_front(const data::IsometricParameters &params, const AABB &box, const AABB &other) {
        const Vector3 &boxEnd{box.position + box.size};
        const Vector3 &otherEnd{other.position + other.size};

        if (boxEnd.x - other.position.x <= params.topological_margin) {
            return -1;
        } else if (otherEnd.x - box.position.x <= params.topological_margin) {
            return 1;
        }

        if (boxEnd.y - other.position.y <= params.topological_margin) {
            return -1;
        } else if (otherEnd.y - box.position.y <= params.topological_margin) {
            return 1;
        }

        if (boxEnd.z - other.position.z <= params.topological_margin) {
            return -1;
        } else if (otherEnd.z - box.position.z <= params.topological_margin) {
            return 1;
        }
        return 0;
    }

    static PoolVector2Array get_bounding_box(const data::IsometricParameters &params, Vector3 size) {
        real_t w{size.x};
        real_t d{size.y};
        real_t h{size.z};

        auto tile_width_float = static_cast<real_t>(params.diamond_width);
        auto tile_height_float = static_cast<real_t>(params.diamond_height);

        Vector2 offset(0, -tile_height_float * 0.5f);

        PoolVector2Array points;

        //Lower points
        points.push_back(Vector2(0, 0) + offset);
        points.push_back(Vector2(tile_width_float * 0.5f * w, tile_height_float * 0.5f * w) + offset);
        points.push_back(Vector2(tile_width_float * 0.5f * (w - d), tile_height_float * 0.5f * (d + w)) + offset);
        points.push_back(Vector2(-tile_width_float * 0.5f * d, tile_height_float * 0.5f * d) + offset);

        Vector2 heightOffset(0, -params.z_length * h);

        //Upper points
        points.push_back(points[0] + heightOffset);
        points.push_back(points[1] + heightOffset);
        points.push_back(points[2] + heightOffset);
        points.push_back(points[3] + heightOffset);

        return points;
    }
}


#endif //ISOMETRIC_MAPS_ISOMETRIC_MATHS_H
