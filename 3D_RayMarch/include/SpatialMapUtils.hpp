#include <functional>
#include <unordered_map>
#include <unordered_set>

#ifndef SPATIAL_MAP_UTILS_HPP
#define SPATIAL_MAP_UTILS_HPP

struct Vec3i {
    int x, y, z;
    bool operator==(const Vec3i& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

namespace std {
    template <>
    struct hash<Vec3i> {
        std::size_t operator()(const Vec3i& k) const {
            size_t hx = hash<int>()(k.x);
            size_t hy = hash<int>()(k.y);
            size_t hz = hash<int>()(k.z);
            return ((hx * 73856093) ^ (hy * 19349663) ^ (hz * 83492791));
        }
    };
}

#endif