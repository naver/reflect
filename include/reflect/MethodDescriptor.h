
#ifndef REFLECT_MethodDescriptor_h
#define REFLECT_MethodDescriptor_h

#include "TypeDescriptor.h"
#include <sstream>

namespace reflect {

std::string methodDescriptor(const std::string& classDescriptor, const std::string& name,
    size_t numParameters, const std::string parameterTypeDescriptors[]);

template<typename>
class MethodDescriptor;

template<typename R>
class MethodDescriptor<R (*) ()> {
public:
    static std::string descriptor(const std::string& className, const std::string& methodName)
    {
        return methodDescriptor(className, methodName, 0, nullptr);
    }
};

template<typename R, typename... P>
class MethodDescriptor<R (*) (P...)> {
public:
    static std::string descriptor(const std::string& className, const std::string& methodName)
    {
        std::string descriptors[] = { TypeDescriptor<P>::descriptor()... };
        return methodDescriptor(className, methodName, sizeof...(P), descriptors);
    }
};

template<typename R, typename C>
class MethodDescriptor<R (C::*) ()> {
public:
    static std::string descriptor(const std::string& className, const std::string& methodName)
    {
        return methodDescriptor(className, methodName, 0, nullptr);
    }
};

template<typename R, typename C, typename... P>
class MethodDescriptor<R (C::*) (P...)> {
public:
    static std::string descriptor(const std::string& className, const std::string& methodName)
    {
        std::string descriptors[] = { TypeDescriptor<P>::descriptor()... };
        return methodDescriptor(className, methodName, sizeof...(P), descriptors);
    }
};

}

#endif // REFLECT_MethodDescriptor_h
