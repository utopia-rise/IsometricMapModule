#ifndef ISOMETRIC_MAPS_PLACEHOLDER_TYPE_H
#define ISOMETRIC_MAPS_PLACEHOLDER_TYPE_H

#include "core/io/resource.h"

namespace resource {

    class PlaceholderType : public Resource {
        GDCLASS(PlaceholderType, Resource)

    private:
        String type_name;
        Color color;

    public:
        String get_type_name() const;

        void set_type_name(const String& name);

        Color get_color() const;

        void set_color(Color col);

    protected:
        static void _bind_methods();
    };

}// namespace resource

#endif// ISOMETRIC_MAPS_PLACEHOLDER_TYPE_H
