#include "default_static_body.h"

using namespace positionable::physics;

DefaultStaticBody::DefaultStaticBody() : StaticBody() {
    initialize_shapes();
}
