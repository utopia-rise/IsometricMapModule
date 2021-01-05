#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include "isometric_positionable.h"
#include <scene/3d/physics_body.h>

namespace positionable {
    template <class T>
    class IsometricElement : public IsometricPositionable {
    protected:
        bool has_moved;
        bool has_default_body;
        T* default_body;

        /**
         * Sets or remove defaultBody in function of hasDefaultBody
         */
        void update_default_body();

    public:
        IsometricElement();
        ~IsometricElement() override = default;

        void _enter_tree() override;

        /**
         * @return true if should have default body.
         */
        virtual bool get_has_default_body() const;

        /**
         * Sets if StaticIsometricElement should have default body.
         * @param b
         */
        virtual void set_has_default_body(bool b);

        virtual void update_position_from_body(PhysicsBody* physics_body);

        virtual SlopeType get_slope_type() const;

        void set_aabb(AABB ab) override;
        void set_global_position_3d(Vector3 pos) override;
        void on_resize() override;

        bool get_has_moved() const override;
        void set_has_moved(bool hm) override;
    };

    template<class T>
    IsometricElement<T>::IsometricElement()
            : IsometricPositionable(), has_moved(false), has_default_body(true), default_body(nullptr) {

    }

    template<class T>
    void IsometricElement<T>::_enter_tree() {
        IsometricPositionable::_enter_tree();
        if (has_default_body) {
            for (int i = 0; i < get_child_count(); i++) {
                auto *body = cast_to<T>(get_child(i));
                if (body) {
                    default_body = body;
                    default_body->set_parent(this);
                    default_body->set_owner(this);
                    break;
                }
            }
        }
        update_default_body();
    }

    template<class T>
    bool IsometricElement<T>::get_has_default_body() const {
        return has_default_body;
    }

    template<class T>
    void IsometricElement<T>::set_has_default_body(bool b) {
        has_default_body = b;
        if (is_inside_tree()) {
            update_default_body();
        }
    }

    template<class T>
    void IsometricElement<T>::update_position_from_body(PhysicsBody* physics_body) {
        const Vector3 &origin = physics_body->get_global_transform().origin;
        IsometricPositionable::set_global_position_3d({origin.x, origin.z, origin.y});
    }

    template<class T>
    IsometricPositionable::SlopeType IsometricElement<T>::get_slope_type() const {
        return SlopeType::NONE;
    }

    template<class T>
    void IsometricElement<T>::set_aabb(AABB ab) {
        IsometricPositionable::set_aabb(ab);
        has_moved = true;
    }

    template<class T>
    void IsometricElement<T>::set_global_position_3d(Vector3 pos) {
        IsometricPositionable::set_global_position_3d(pos);
        has_moved = true;
    }

    template<class T>
    void IsometricElement<T>::update_default_body() {
        if (has_default_body) {
            if (!default_body) {
                default_body = memnew(T());
                default_body->set_parent(this);
                add_child(default_body);
                default_body->set_owner(this);
                has_moved = true;
            }
        } else {
            if (default_body) {
                remove_child(default_body);
                default_body->queue_delete();
                default_body = nullptr;
            }
        }
    }

    template<class T>
    void IsometricElement<T>::on_resize() {
        IsometricPositionable::on_resize();
        has_moved = true;
    }

    template<class T>
    bool IsometricElement<T>::get_has_moved() const {
        return has_moved;
    }

    template<class T>
    void IsometricElement<T>::set_has_moved(bool hm) {
        has_moved = hm;
    }
}

#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
