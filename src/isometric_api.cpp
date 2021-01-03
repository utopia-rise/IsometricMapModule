#include "isometric_api.h"

IsometricApi* IsometricApi::get_instance() {
    static IsometricApi singleton;
    return &singleton;
}
