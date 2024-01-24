#ifndef ISOMETRIC_MAPS_GRID_3D_H
#define ISOMETRIC_MAPS_GRID_3D_H

#include <core/templates/vector.h>
#include <core/variant/array.h>

#include <core/math/aabb.h>
#include <core/math/vector3.h>
#include <core/math/vector3i.h>

namespace containers {

    template<class T, T default_value>
    class Grid3D {
    private:
        int width = 1;
        int depth = 1;
        int height = 1;

        Vector<T> internal_array;

        bool is_in_range(int x, int y, int z) const;

        int get_index_from_position(const Vector3& position) const;

        Vector3 plane_square_and_jumps_from(const Vector3& size) const;

        inline int compute_array_size() const { return width * depth * height; }

        static int index_increment_from(const Vector3& planeSquareAndJumps, const Vector3& size, int iteration);

    public:
        Grid3D();

        ~Grid3D() = default;

        void update_array_size(const Vector3& size, bool reset = false);

        Vector3i get_dimensions();

        void reset();

        T get_data(const Vector3& position) const;

        void set_data(const Vector3& position, T data);

        Vector3 get_position_3d_from_index(int id) const;

        bool insert_box(const AABB& aabb, T data, bool remove = false);

        bool is_overlapping(const AABB& aabb) const;

        bool has(T object) const;

        void for_each(void (*fptr)(T));

        int get_width() const;

        void set_width(int w);

        int get_depth() const;

        void set_depth(int d);

        int get_height() const;

        void set_height(int h);

        const Vector<T>& get_internal_array() const;

        void set_internal_array(const Vector<T>& array);

        static inline T get_default_value();

        Array to_array() const;
        void from_array(const Array& array);
    };

    template<class T, T default_value>
    void Grid3D<T, default_value>::update_array_size(const Vector3& size, bool reset) {
        // Save old values
        int old_size {internal_array.size()};
        Vector<T> old_array {internal_array};
        int old_width {width};
        int old_depth {depth};

        // Set new values
        width = static_cast<int>(size.x);
        depth = static_cast<int>(size.y);
        height = static_cast<int>(size.z);
        internal_array = Vector<T>();

        for (int i = 0; i < compute_array_size(); i++) {
            internal_array.push_back(default_value);
        }

        if (reset) { return; }

        for (int i = 0; i < old_size; i++) {
            int x = i % old_width;
            int y = (i / old_width) % old_depth;
            int z = i / (old_width * old_depth);

            if (!is_in_range(x, y, z)) { continue; }

            T value {old_array[i]};
            if (value) { set_data({static_cast<real_t>(x), static_cast<real_t>(y), static_cast<real_t>(z)}, value); }
        }
    }

    template<class T, T default_value>
    Vector3i Grid3D<T, default_value>::get_dimensions() {
        return {width, depth, height};
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::reset() {
        update_array_size({static_cast<real_t>(width), static_cast<real_t>(depth), static_cast<real_t>(height)}, true);
    }

    template<class T, T default_value>
    T Grid3D<T, default_value>::get_data(const Vector3& position) const {
        int pos_x = static_cast<int>(position.x);
        int pos_y = static_cast<int>(position.y);
        int pos_z = static_cast<int>(position.z);
        return pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < depth && pos_z >= 0 && pos_z < height
               ? internal_array[pos_x + width * pos_y + width * depth * pos_z]
               : default_value;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::set_data(const Vector3& position, T data) {
        internal_array.set(
          static_cast<int>(position.x) + width * static_cast<int>(position.y)
            + width * depth * static_cast<int>(position.z),
          data
        );
    }

    template<class T, T default_value>
    bool Grid3D<T, default_value>::insert_box(const AABB& aabb, T data, bool remove) {
        const Vector3& position {aabb.position};
        const Vector3& size {aabb.size};
        int index {get_index_from_position(position)};
        int sizeX = static_cast<int>(size.x);
        int sizeY = static_cast<int>(size.y);
        int sizeZ = static_cast<int>(size.z);
        int endX = static_cast<int>(position.x) + sizeX;
        int endY = static_cast<int>(position.y) + sizeY;
        int endZ = static_cast<int>(position.z) + sizeZ;

        if (endX > width || endY > depth || endZ > height) { return false; }

        internal_array.set(index, remove ? nullptr : data);
        for (int i = 1; i < sizeX * sizeY * sizeZ; i++) {
            index += Grid3D::index_increment_from(plane_square_and_jumps_from(size), size, i);
            internal_array.set(index, remove ? nullptr : data);
        }
        return true;
    }

    template<class T, T default_value>
    bool Grid3D<T, default_value>::is_overlapping(const AABB& aabb) const {
        int index {get_index_from_position(aabb.position)};
        const Vector3& size {aabb.size};

        if (index >= 0 && index < internal_array.size()) {
            T element = internal_array[index];
            if (element) { return true; }
            for (int i = 1; i < static_cast<int>(size.x) * static_cast<int>(size.y) * static_cast<int>(size.z); ++i) {
                index += Grid3D::index_increment_from(plane_square_and_jumps_from(size), size, i);
                if (index >= 0 && index < internal_array.size()) {
                    element = internal_array[index];
                    if (element) { return true; }
                }
            }
        }
        return false;
    }

    template<class T, T default_value>
    bool Grid3D<T, default_value>::has(T object) const {
        for (int i = 0; i < internal_array.size(); ++i) {
            if (internal_array[i] == object) { return true; }
        }
        return false;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::for_each(void (*fptr)(T)) {
        for (int i = 0; i < internal_array.size(); ++i) {
            T element = internal_array[i];
            if (element) { fptr(element); }
        }
    }

    template<class T, T default_value>
    int Grid3D<T, default_value>::get_width() const {
        return width;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::set_width(int w) {
        width = w;
    }

    template<class T, T default_value>
    int Grid3D<T, default_value>::get_depth() const {
        return depth;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::set_depth(int d) {
        depth = d;
    }

    template<class T, T default_value>
    int Grid3D<T, default_value>::get_height() const {
        return height;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::set_height(int h) {
        height = h;
    }

    template<class T, T default_value>
    const Vector<T>& Grid3D<T, default_value>::get_internal_array() const {
        return internal_array;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::set_internal_array(const Vector<T>& array) {
        internal_array = array;
    }

    template<class T, T default_value>
    bool Grid3D<T, default_value>::is_in_range(int x, int y, int z) const {
        return x < width && y < depth && z < height;
    }

    template<class T, T default_value>
    int Grid3D<T, default_value>::get_index_from_position(const Vector3& position) const {
        return static_cast<int>(position.x) + width * static_cast<int>(position.y)
             + width * depth * static_cast<int>(position.z);
    }

    template<class T, T default_value>
    Vector3 Grid3D<T, default_value>::get_position_3d_from_index(int id) const {
        return {
          static_cast<real_t>(id % width),
          static_cast<real_t>((id / width) % depth),
          static_cast<real_t>(static_cast<int>(id / (width * depth)))};
    }

    template<class T, T default_value>
    Vector3 Grid3D<T, default_value>::plane_square_and_jumps_from(const Vector3& size) const {
        return {size.x * size.y, static_cast<real_t>(width) - size.x, width * (depth - size.y)};
    }

    template<class T, T default_value>
    int Grid3D<T, default_value>::index_increment_from(const Vector3& planeSquareAndJumps, const Vector3& size, int iteration) {
        int increment {0};
        increment += (iteration % static_cast<int>(size.x)) == 0 ? static_cast<int>(planeSquareAndJumps.y) : 0;
        increment += (iteration % static_cast<int>(planeSquareAndJumps.x)) == 0 ? static_cast<int>(planeSquareAndJumps.z) : 0;
        increment += 1;
        return increment;
    }

    template<class T, T default_value>
    T Grid3D<T, default_value>::get_default_value() {
        return default_value;
    }

    template<class T, T default_value>
    Array Grid3D<T, default_value>::to_array() const {
        Array ret;
        for (int i = 0; i < internal_array.size(); ++i) {
            ret.append(internal_array[i]);
        }
        return ret;
    }

    template<class T, T default_value>
    void Grid3D<T, default_value>::from_array(const Array& array) {
        Vector<T> new_internal_array;
        for (int i = 0; i < array.size(); ++i) {
            new_internal_array.push_back(array[i]);
        }
        set_internal_array(new_internal_array);
    }

    template<class T, T default_value>
    Grid3D<T, default_value>::Grid3D() {
        update_array_size(Vector3(1, 1, 1));
    }

}// namespace containers

#endif// ISOMETRIC_MAPS_GRID_3D_H
