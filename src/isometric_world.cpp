#include "isometric_world.h"
#include "isometric_api.h"
#include "isometric_server.h"

IsometricWorld::IsometricWorld(): static_elements(), dynamic_elements() {
    
}

void IsometricWorld::register_isometric_element(IsometricPositionableData* p_positionable_data, bool p_is_dynamic) {
    if(!p_is_dynamic){
        static_elements.push_back(p_positionable_data);
    }
    else {
        dynamic_elements.push_back(p_positionable_data);
    }
}

void IsometricWorld::unregister_isometric_element(IsometricPositionableData* p_positionable_data, bool p_is_dynamic) {
    if(!p_is_dynamic){
        static_elements.erase(p_positionable_data);
        for (int i = 0; i < static_elements.size(); i++) {
            static_elements[i]->behind_statics.erase(p_positionable_data);

        }
    }
    else {
        dynamic_elements.erase(p_positionable_data);
    }
    p_positionable_data->behind_statics.clear();
    p_positionable_data->behind_dynamics.clear();
}

void IsometricWorld::generateTopologicalRenderGraph() {
    //Iterate the static and dynamic elements and clear them.
    for (int i = 0; i < static_elements.size(); ++i) {
        auto *positionable = Object::cast_to<positionable::IsometricPositionable>(static_elements[i]);
        if (positionable) {
            positionable->behind_dynamics.clear();
            positionable->set_z_index(0);
            positionable->set_rendered(false);
        }
    }
    for (int i = 0; i < dynamic_elements.size(); ++i) {
        auto *positionable = Object::cast_to<positionable::IsometricPositionable>(dynamic_elements[i]);
        if (positionable) {
            positionable->behind_dynamics.clear();
            positionable->set_z_index(0);
            positionable->set_rendered(false);
        }
    }

    for (int i = 0; i < dynamic_elements.size(); ++i) {
        auto *dynamicPositionable = Object::cast_to<positionable::IsometricPositionable>(dynamic_elements[i]);
        if (dynamicPositionable) {
            for (int j = 0; j < dynamic_elements.size(); ++j){
                if(j != i){
                    auto *positionable = Object::cast_to<positionable::IsometricPositionable>(dynamic_elements[j]);
                    if(positionable){
                        if(IsometricApi::get_instance()->do_hexagone_overlap(dynamicPositionable->get_hexagone_coordinates(),
                                                                            positionable->get_hexagone_coordinates())){
                            if(IsometricApi::get_instance()->is_box_in_front(dynamicPositionable->get_aabb(), positionable->get_aabb())){
                                dynamicPositionable->behind_dynamics.append(positionable);
                            }
                            else{
                                positionable->behind_dynamics.append(dynamicPositionable);
                            }
                        }
                    }
                }
            }
            for (int j = 0; j < static_elements.size(); ++j){
                auto *positionable = Object::cast_to<positionable::IsometricPositionable>(static_elements[j]);
                if(positionable){
                    if(IsometricApi::get_instance()->do_hexagone_overlap(dynamicPositionable->get_hexagone_coordinates(),
                                                                        positionable->get_hexagone_coordinates())){
                        if(IsometricApi::get_instance()->is_box_in_front(dynamicPositionable->get_aabb(), positionable->get_aabb())){
                            dynamicPositionable->behind_dynamics.append(positionable);
                        }
                        else{
                            positionable->behind_dynamics.append(dynamicPositionable);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < static_elements.size(); ++i) {
        auto *positionable = Object::cast_to<positionable::IsometricPositionable>(static_elements[i]);
        if (positionable && !positionable->is_rendered()) {
            register_isometric_element(positionable, false);
        }
    }
    for (int i = 0; i < dynamic_elements.size(); ++i) {
        auto *positionable = Object::cast_to<positionable::IsometricPositionable>(dynamic_elements[i]);
        if (positionable && !positionable->is_rendered()) {
            register_isometric_element(positionable, false);
        }
    }
}

void IsometricWorld::render_isometric_element(positionable::IsometricPositionable* positionable) {

}

//TODO
void IsometricWorld::order_statics() {
    for (int i = 0; i < static_elements.size(); i++) {
        IsometricPositionableData* element{static_elements[i]};
        if(IsometricApi::get_instance()->do_hexagone_overlap(element->get_hexagone_coordinates(),
                                                             positionable_data->get_hexagone_coordinates())){
            if(IsometricApi::get_instance()->is_box_in_front(static_element->get_aabb(), element->get_aabb())){
                static_element->behind_statics.append(element);
            }
            else{
                element->behind_statics.append(static_element);
            }
        }
    }
}
