#ifndef ISOMETRIC_MAPS_LOGGING_H
#define ISOMETRIC_MAPS_LOGGING_H

#define LOG_VERBOSE(message) print_verbose(vformat("Godotopia/Isometric-Maps: %s", message))

#define LOG_INFO(message) print_line(vformat("Godotopia/Isometric-Maps: %s", message))

#define LOG_WARNING(message) WARN_PRINT(vformat("Godotopia/Isometric-Maps: %s", message))

#define LOG_ERROR(message) ERR_PRINT(vformat("Godotopia/Isometric-Maps: %s", message))

#define MAPS_ERR_FAIL_COND_MSG(condition, message) ERR_FAIL_COND_MSG(condition, vformat("Godotopia/Isometric-Maps: %s", message))

#define MAPS_ERR_FAIL_COND_V_MSG(condition, ret_var, message) ERR_FAIL_COND_V_MSG(condition, ret_var, vformat("Godotopia/Isometric-Maps: %s", message))

#define MAPS_ERR_FAIL_COND_MSG(condition, message) ERR_FAIL_COND_MSG(condition, vformat("Godotopia/Isometric-Maps: %s", message))

#define MAPS_CRASH_COND_MSG(condition, message) CRASH_COND_MSG(condition, vformat("Godotopia/Isometric-Maps: %s", message))

#define MAPS_CRASH_NOW_MSG(message) CRASH_NOW_MSG(vformat("Godotopia/Isometric-Maps: %s", message))

#endif// ISOMETRIC_MAPS_LOGGING_H
