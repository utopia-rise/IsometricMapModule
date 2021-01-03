#ifndef ISOMETRIC_MAPS_GRID_3D_H
#define ISOMETRIC_MAPS_GRID_3D_H


#include <core/array.h>
#include <core/math/vector3.h>
#include <core/math/aabb.h>

namespace containers {
    class Grid3D {
    private:

        int width = 1;
        int depth = 1;
        int height = 1;

        Array internal_array;

        bool is_in_range(int x, int y, int z) const;

        int get_id(const Vector3& position) const;
        Vector3 get_position_3d(int id) const;
        Vector3 plane_square_and_jumps_from(const Vector3& size) const;

        inline int compute_array_size() const {
            return width * depth * height;
        }

        static int index_increment_from(const Vector3& planeSquareAndJumps, const Vector3& size, int iteration);
    public:
        Grid3D() = default;
        ~Grid3D() = default;

        void update_array_size(const Vector3& size, bool reset = false);
        void reset();
        Object* get_data(const Vector3& position);
        void set_data(const Vector3& position, Object* data);
        bool insert_box(const AABB& aabb, Object* data, bool remove = false);
        bool is_overlapping(const AABB& aabb) const;
        bool has(Object* object) const;

        void for_each(void (*fptr)(Object* ));

        int get_width() const;
        void set_width(int w);
        int get_depth() const;
        void set_depth(int d);
        int get_height() const;
        void set_height(int h);

        const Array& get_internal_array() const;
        void set_internal_array(const Array& array);
    };
    };
}


#endif //ISOMETRIC_MAPS_GRID_3D_H
