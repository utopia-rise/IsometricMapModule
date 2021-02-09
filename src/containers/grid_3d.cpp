#include "grid_3d.h"

using namespace containers;

void Grid3D::update_array_size(const Vector3 &size, bool reset) {
    //Save old values
    int old_size{internal_array.size()};
    Vector<Object*> old_array{internal_array};
    int old_width{width};
    int old_depth{depth};

    //Set new values
    width = static_cast<int>(size.x);
    depth = static_cast<int>(size.y);
    height = static_cast<int>(size.z);
    internal_array = Vector<Object*>();

    for (int i = 0; i < compute_array_size(); i++) {
        internal_array.push_back(nullptr);
    }

    if (reset) {
        return;
    }

    for (int i = 0; i < old_size; i++) {
        int x = i % old_width;
        int y = (i / old_width) % old_depth;
        int z = i / (old_width * old_depth);

        if (!is_in_range(x, y, z)) {
            continue;
        }

        Object* value = old_array[i];
        if (value) {
            set_data({static_cast<real_t>(x), static_cast<real_t>(y), static_cast<real_t>(z)}, value);
        }
    }
}

void Grid3D::reset() {
    update_array_size({static_cast<real_t>(width), static_cast<real_t>(depth), static_cast<real_t>(height)}, true);
}

Object* Grid3D::get_data(const Vector3 &position) {
    int pos_x = static_cast<int>(position.x);
    int pos_y = static_cast<int>(position.y);
    int pos_z = static_cast<int>(position.z);
    return pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < depth && pos_z >= 0 && pos_z < height
           ? (Object*) internal_array[pos_x + width * pos_y + width * depth * pos_z] : nullptr;
}

void Grid3D::set_data(const Vector3 &position, Object* data) {
    internal_array.set(
            static_cast<int>(position.x) + width * static_cast<int>(position.y) +
            width * depth * static_cast<int>(position.z),
            data);
}

bool Grid3D::insert_box(const AABB &aabb, Object* data, bool remove) {
    const Vector3 &position{aabb.position};
    const Vector3 &size{aabb.size};
    int index{get_id(position)};
    int sizeX = static_cast<int>(size.x);
    int sizeY = static_cast<int>(size.y);
    int sizeZ = static_cast<int>(size.z);
    int endX = static_cast<int>(position.x) + sizeX;
    int endY = static_cast<int>(position.y) + sizeY;
    int endZ = static_cast<int>(position.z) + sizeZ;

    if (endX > width || endY > depth || endZ > height) {
        return false;
    }

    internal_array.set(index, remove ? nullptr : data);
    for (int i = 1; i < sizeX * sizeY * sizeZ; i++) {
        index += Grid3D::index_increment_from(plane_square_and_jumps_from(size), size, i);
        internal_array.set(index, remove ? nullptr : data);
    }
    return true;
}

bool Grid3D::is_overlapping(const AABB &aabb) const {
    int index{get_id(aabb.position)};
    const Vector3 &size{aabb.size};

    if (index >= 0 && index < internal_array.size()) {
        Object* element = internal_array[index];
        if (element) {
            return true;
        }
        for (int i = 1; i < static_cast<int>(size.x) * static_cast<int>(size.y) * static_cast<int>(size.z); i++) {
            index += Grid3D::index_increment_from(plane_square_and_jumps_from(size), size, i);
            if (index >= 0 && index < internal_array.size()) {
                element = internal_array[index];
                if (element) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Grid3D::has(Object* object) const {
    for (int i = 0; i < internal_array.size(); i++) {
        if ((Object*) internal_array[i] == object) {
            return true;
        }
    }
    return false;
}

void Grid3D::for_each(void (* fptr)(Object*)) {
    for (int i = 0; i < internal_array.size(); i++) {
        Object* element = internal_array[i];
        if (element) {
            fptr(element);
        }
    }
}

int Grid3D::get_width() const {
    return width;
}

void Grid3D::set_width(int w) {
    width = w;
}

int Grid3D::get_depth() const {
    return depth;
}

void Grid3D::set_depth(int d) {
    depth = d;
}

int Grid3D::get_height() const {
    return height;
}

void Grid3D::set_height(int h) {
    height = h;
}

const Vector<Object*> &Grid3D::get_internal_array() const {
    return internal_array;
}

void Grid3D::set_internal_array(const Vector<Object*> &array) {
    internal_array = array;
}

bool Grid3D::is_in_range(int x, int y, int z) const {
    return x < width && y < depth && z < height;
}

int Grid3D::get_id(const Vector3 &position) const {
    return static_cast<int>(position.x) + width * static_cast<int>(position.y) +
           width * depth * static_cast<int>(position.z);
}

Vector3 Grid3D::get_position_3d(int id) const {
    return {
            static_cast<real_t>(id % width),
            static_cast<real_t>((id / width) % depth),
            static_cast<real_t>(id) / (width * depth)
    };
}

Vector3 Grid3D::plane_square_and_jumps_from(const Vector3 &size) const {
    return {
            size.x * size.y,
            static_cast<real_t>(width) - size.x,
            width * (depth - size.y)
    };
}

int Grid3D::index_increment_from(const Vector3 &planeSquareAndJumps, const Vector3 &size, int iteration) {
    int increment{0};
    increment += (iteration % static_cast<int>(size.x)) == 0 ? static_cast<int>(planeSquareAndJumps.y) : 0;
    increment +=
            (iteration % static_cast<int>(planeSquareAndJumps.x)) == 0 ? static_cast<int>(planeSquareAndJumps.z) : 0;
    increment += 1;
    return increment;
}
