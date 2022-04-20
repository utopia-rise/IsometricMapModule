#include "isometric_kinematic_body.h"

using namespace node::physics;

void IsometricKinematicBody::_notification(int notif) {
    notification_impl(notif);
}

void IsometricKinematicBody::_bind_methods() {
    BIND_VMETHOD(MethodInfo("_do_physics", PropertyInfo(Variant::REAL, "delta")))
}
