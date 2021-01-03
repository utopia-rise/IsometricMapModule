#ifndef ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
#define ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H

#include "isometric_positionable.h"
#include <scene/3d/physics_body.h>

namespace positionable {
    template <class T>
    class IsometricElement : public IsometricPositionable {
    protected:
        bool hasMoved;
        bool hasDefaultBody;
        T *defaultBody;

        /**
         * Sets or remove defaultBody in function of hasDefaultBody
         */
        void updateDefaultBody();

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

        virtual void update_position_from_body(PhysicsBody* physicsBody);

        virtual SlopeType get_slope_type() const;

        void set_aabb(AABB ab) override;
        void set_global_position_3d(Vector3 pos) override;
        void on_resize() override;

        bool get_has_moved() const override;
        void set_has_moved(bool hm) override;
    };

    template<class T>
    IsometricElement<T>::IsometricElement()
            : IsometricPositionable(), hasMoved(false), hasDefaultBody(true), defaultBody(nullptr) {

    }

    template<class T>
    void IsometricElement<T>::_enter_tree() {
        IsometricPositionable::_enter_tree();
        const Array &children = get_children();
        if (hasDefaultBody) {
            for (int i = 0; i < children.size(); i++) {
                auto *body = cast_to<T>(children[i]);
                if (body) {
                    defaultBody = body;
                    defaultBody->setParent(this);
                    defaultBody->set_owner(this);
                    break;
                }
            }
        }
        updateDefaultBody();
    }

    template<class T>
    bool IsometricElement<T>::get_has_default_body() const {
        return hasDefaultBody;
    }

    template<class T>
    void IsometricElement<T>::set_has_default_body(bool b) {
        hasDefaultBody = b;
        if (is_inside_tree()) {
            updateDefaultBody();
        }
    }

    template<class T>
    void IsometricElement<T>::update_position_from_body(PhysicsBody* physicsBody) {
        const Vector3 &origin = physicsBody->get_global_transform().origin;
        IsometricPositionable::setGlobalPosition3D({origin.x, origin.z, origin.y});
    }

    template<class T>
    SlopeType IsometricElement<T>::get_slope_type() const {
        return SlopeType::NONE;
    }

    template<class T>
    void IsometricElement<T>::set_aabb(AABB ab) {
        IsometricPositionable::setAABB(ab);
        hasMoved = true;
    }

    template<class T>
    void IsometricElement<T>::set_global_position_3d(Vector3 pos) {
        IsometricPositionable::setGlobalPosition3D(pos);
        hasMoved = true;
    }

    template<class T>
    void IsometricElement<T>::updateDefaultBody() {
        if (hasDefaultBody) {
            if (!defaultBody) {
                defaultBody = T::_new();
                defaultBody->setParent(this);
                add_child(defaultBody);
                defaultBody->set_owner(this);
                hasMoved = true;
            }
        } else {
            if (defaultBody) {
                remove_child(defaultBody);
                defaultBody->queue_free();
                defaultBody = nullptr;
            }
        }
    }

    template<class T>
    void IsometricElement<T>::on_resize() {
        IsometricPositionable::onResize();
        hasMoved = true;
    }

    template<class T>
    bool IsometricElement<T>::get_has_moved() const {
        return hasMoved;
    }

    template<class T>
    void IsometricElement<T>::set_has_moved(bool hm) {
        hasMoved = hm;
    }
}

#endif //ISOMETRIC_MAPS_ISOMETRIC_ELEMENT_H
