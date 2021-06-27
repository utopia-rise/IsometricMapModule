#ifdef TOOLS_ENABLED

#include <core/os/dir_access.h>
#include <core/os/file_access.h>
#include <core/variant.h>
#include <core/resource.h>
#include <scene/resources/packed_scene.h>
#include <core/io/resource_loader.h>
#include <modules/isometric_maps/src/node/isometric_positionable.h>
#include "editor_utils.h"

using namespace editor;

Error EditorUtils::find_all_positionables_in_path(const String &path, List<String>* r_value) {
    Error error;
    DirAccess* dir_access{DirAccess::open(path, &error)};

    if (error != OK) {
        FileAccess* file_access{FileAccess::open(path, FileAccess::READ, &error)};
        if (error != OK) {
            //TODO : show popup saying wrong path
            WARN_PRINT(vformat("%s cannot be opened", path))
            return Error::ERR_CANT_RESOLVE;
        }
        if (path.ends_with(".tscn")) {
            RES resource{ResourceLoader::load(path)};
            if (auto* packed_scene{Object::cast_to<PackedScene>(resource.ptr())}) {
                //TODO: investigate get_inheritance_list_static generated from GDClass
                if (auto *positionable{Object::cast_to<node::IsometricPositionable>(packed_scene->instance())}) {
                    r_value->push_back(path);
                    memdelete(positionable);
                }
            }
        }
        file_access->close();
        memdelete(file_access);
    } else {
        dir_access->list_dir_begin();
        String item;
        while (!(item = dir_access->get_next()).empty()) {
            if (item == "." || item == "..") {
                continue;
            }
            find_all_positionables_in_path(path.plus_file(item), r_value);
        }
        memdelete(dir_access);
    }
    return Error::OK;
}

#endif
