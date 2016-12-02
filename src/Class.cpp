
#include "Class.h"

#include <unordered_map>

namespace reflect {

int64_t ClassSharedData::nextTypeid = 1;

ClassSharedData::ClassSharedData()
    : m_typeid(nextTypeid++)
{
}

ClassSharedData& ClassUnknown::classSharedData(const std::string& key)
{
    static std::unordered_map<std::string, std::unique_ptr<ClassSharedData>> map;
    if (map.count(key) == 0)
        map[key] = std::make_unique<ClassSharedData>();
    return *map[key];
}

std::map<void*, ClassUnknown*>& ClassUnknown::instances()
{
    static std::map<void*, ClassUnknown*> map;
    return map;
}

}
