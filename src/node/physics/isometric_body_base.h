#ifndef ISOMETRIC_MAPS_ISOMETRIC_BODY_BASE_H
#define ISOMETRIC_MAPS_ISOMETRIC_BODY_BASE_H

#include <scene/main/node.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include <modules/isometric_maps/src/logging.h>
#include <core/engine.h>


namespace node {
    namespace physics {
        template<class Derived, bool can_do_physics, bool should_update_parent_position>
        class IsometricBodyBase {
        protected:
            inline void notification_impl(int notif);

        private:
            void _enter_tree();
            void _physics_process(float delta);
        };

        template<class Derived, bool can_do_physics, bool should_update_parent_position>
        void IsometricBodyBase<Derived, can_do_physics, should_update_parent_position>::notification_impl(int notif) {
            switch (notif) {
                case Node::NOTIFICATION_PHYSICS_PROCESS:
                    _physics_process(static_cast<Derived*>(this)->get_physics_process_delta_time());
                    break;
                case Node::NOTIFICATION_ENTER_TREE:
                    _enter_tree();
                    break;
                default:
                    break;
            }
        }

        template<class Derived, bool can_do_physics, bool should_update_parent_position>
        void IsometricBodyBase<Derived, can_do_physics, should_update_parent_position>::_enter_tree() {
            if (auto* parent{Object::cast_to<node::IsometricPositionable>(static_cast<Derived*>(this)->get_parent())}) {
                const Vector3& parent_position{parent->get_global_position_3d()};
                static_cast<Derived*>(this)->set_global_transform(
                        {
                                {1, 0, 0, 0, 1, 0, 0, 0, 1},
                                {parent_position.x, parent_position.z, parent_position.y}
                        }
                );
                static_cast<Derived*>(this)->set_physics_process(true);
            }
        }

        template<class Derived, bool can_do_physics, bool should_update_parent_position>
        void IsometricBodyBase<Derived, can_do_physics, should_update_parent_position>::_physics_process(float delta) {
            if (auto* parent{Object::cast_to<node::IsometricPositionable>(static_cast<Derived*>(this)->get_parent())}) {
                if (parent->get_has_moved()) {
                    const Vector3& parent_position{parent->get_global_position_3d()};
                    static_cast<Derived*>(this)->set_global_transform(
                            {
                                    {1, 0, 0, 0, 1, 0, 0, 0, 1},
                                    {parent_position.x, parent_position.z, parent_position.y}
                            }
                    );
                    parent->set_has_moved(false);
                }

                if (!can_do_physics || Engine::get_singleton()->is_editor_hint()) {
                    return;
                }

                Variant delta_param{delta};
                const Variant* args[1] = {&delta_param};
                static_cast<Derived*>(this)->call_multilevel_reversed("_do_physics", args, 1);

                if (!should_update_parent_position) {
                    return;
                }

                const Vector3& body_origin{static_cast<Derived*>(this)->get_global_transform().origin};
                parent->set_global_position_3d({body_origin.x, body_origin.z, body_origin.y});
            } else {
                LOG_WARNING(
                        vformat(
                                "IsometricBody with instance %s is not child of an IsometricPositionable",
                                static_cast<Derived*>(this)->get_instance_id()
                        )
                );
            }
        }
    }
}

#endif //ISOMETRIC_MAPS_ISOMETRIC_BODY_BASE_H
