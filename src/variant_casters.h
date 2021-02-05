#ifndef ISOMETRIC_MAPS_VARIANT_CASTERS_H
#define ISOMETRIC_MAPS_VARIANT_CASTERS_H

#include <scene/3d/physics_body.h>

#define DECLARE_VARIANT_CASTER(T)                                     \
	template <>                                                       \
	struct VariantCaster<T *> {                                       \
                                                                      \
		static _FORCE_INLINE_ T *cast(const Variant &p_variant) {     \
			return Object::cast_to<T>(p_variant.operator Object *()); \
		}                                                             \
	};

DECLARE_VARIANT_CASTER(PhysicsBody)

#endif //ISOMETRIC_MAPS_VARIANT_CASTERS_H
