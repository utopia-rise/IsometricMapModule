#ifndef ISOMETRIC_MAPS_ISOMETRIC_API_H
#define ISOMETRIC_MAPS_ISOMETRIC_API_H

#include "core/io/resource.h"
#include "core/templates/rid.h"

namespace resource {

    class IsometricConfiguration : public Resource {
        GDCLASS(IsometricConfiguration, Resource)

    private:
        int tile_width;
        int angle;
        float topological_margin;
        RID self;

    public:
        IsometricConfiguration();

        ~IsometricConfiguration() override;

    public:
        int get_angle() const;

        void set_angle(int agl);

        int get_tile_width() const;

        void set_tile_width(int t_w);

        float get_topological_margin() const;

        void set_topological_margin(float margin);

        RID get_rid() const override;

        static IsometricConfiguration* get_instance(const RID rid);

    protected:
        static void _bind_methods();
    };

}// namespace resource

#endif// ISOMETRIC_MAPS_ISOMETRIC_API_H
