
#ifndef REFLECT_FieldDescriptor_h
#define REFLECT_FieldDescriptor_h

#include "TypeDescriptor.h"
#include <sstream>

namespace reflect {

std::string fieldDescriptor(const std::string& classDescriptor, const std::string& name);

template<typename>
class FieldDescriptor;

template<typename T, typename C>
class FieldDescriptor<T (C::*)> {
public:
    static std::string descriptor(const std::string& className, const std::string& fieldName)
    {
        return fieldDescriptor(className, fieldName);
    }
};

}

#endif // REFLECT_FieldDescriptor_h
