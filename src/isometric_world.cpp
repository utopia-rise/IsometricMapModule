#include "isometric_world.h"
#include "isometric_api.h"

void IsometricWorld::register_isometric_element(positionable::IsometricPositionable* positionable) {
    if(auto* static_element = dynamic_cast<positionable::StaticIsometricElement*>(positionable)){
        for (int i = 0; i < static_elements.size(); i++) {
            auto* element{Object::cast_to<positionable::IsometricPositionable>(static_elements[i])};
            if(element){
                if(IsometricApi::get_instance()->do_hexagone_overlap(element->get_hexagone_coordinates(),
                                                                    static_element->get_hexagone_coordinates())){
                    if(IsometricApi::get_instance()->is_box_in_front(static_element->get_aabb(), element->get_aabb())){
                        static_element->behind_statics.append(element);
                    }
                    else{
                        element->behind_statics.append(static_element);
                    }
                }
            }
        }
        static_elements.push_back(static_element);
    }
    else if(auto* dynamic_element{dynamic_cast<positionable::DynamicIsometricElement*>(positionable)}){
        dynamic_elements.push_back(dynamic_element);
    }
    else{
        WARN_PRINT(positionable->get_name().operator String() + String(" is a invalid element for the IsometricWorld."))
    }
}

void IsometricWorld::unregister_isometric_element(positionable::IsometricPositionable* positionable) {
    if(auto* static_element = dynamic_cast<positionable::StaticIsometricElement*>(positionable)){
        static_elements.erase(static_element);
        for (int i = 0; i < static_elements.size(); i++) {
            auto *element = Object::cast_to<positionable::IsometricPositionable>(static_elements[i]);
            element->behind_statics.erase(static_element);

        }
    }
    else if(auto* dynamic_element = dynamic_cast<positionable::DynamicIsometricElement*>(positionable)){
        dynamic_elements.erase(dynamic_element);
    }
    positionable->behind_statics.clear();
    positionable->behind_dynamics.clear();
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
            register_isometric_element(positionable);
        }
    }
    for (int i = 0; i < dynamic_elements.size(); ++i) {
        auto *positionable = Object::cast_to<positionable::IsometricPositionable>(dynamic_elements[i]);
        if (positionable && !positionable->is_rendered()) {
            register_isometric_element(positionable);
        }
    }
}

void IsometricWorld::render_isometric_element(positionable::IsometricPositionable* positionable) {

}
