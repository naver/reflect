
#ifndef REFLECT_TypeDescriptor_h
#define REFLECT_TypeDescriptor_h

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <stdint.h>

namespace reflect {

template<typename T>
class TypeDescriptor {
public:
    static std::string descriptor() { return "<unknown>"; }
};

static std::string concat(std::initializer_list<std::string>&& strings)
{
    static const std::string space(" ");
    std::string result("<");
    for (auto& string : strings)
        result += (string + space);
    return result;
}

template<typename... A>
class VariadicTypeDescriptor {
public:
    static std::string descriptor() { return concat({ TypeDescriptor<A>::descriptor()... }); }
};

#define TYPE_DESCRIPTOR_MACRO(type) \
    template<> class reflect::TypeDescriptor<type> { \
    public: \
        static std::string descriptor() { return #type; } \
    }

#define TYPE_DESCRIPTOR_MACRO_T(type, ...) \
    template<__VA_ARGS__> class reflect::TypeDescriptor<type<__VA_ARGS__>> { \
    public: \
        static std::string descriptor() { static std::string str = std::string(#type) + VariadicTypeDescriptor<__VA_ARGS__>::descriptor(); return str; } \
    }

#define TYPE_DESCRIBE_CLASS(type) \
    namespace reflect { \
        template<> void describeClass<type>(Class<type>&); \
    }

#define TYPE_DESCRIPTOR_MACROis_void(type)
#define TYPE_DESCRIBE_CLASSis_void(type)
#define TYPE_DESCRIPTOR_MACROis_plain(type) TYPE_DESCRIPTOR_MACRO(type)
#define TYPE_DESCRIBE_CLASSis_plain(type)

#define TYPE_DESCRIPTOR(type, ...) \
    TYPE_DESCRIPTOR_MACRO(type); \
    TYPE_DESCRIPTOR_MACRO(type*); \
    TYPE_DESCRIPTOR_MACRO(const type*); \
    TYPE_DESCRIPTOR_MACRO##__VA_ARGS__##(type&); \
    TYPE_DESCRIPTOR_MACRO##__VA_ARGS__##(const type&); \
    TYPE_DESCRIBE_CLASS##__VA_ARGS__##(type)

#define TYPE_DESCRIPTOR_T(type, ...) \
    TYPE_DESCRIPTOR_MACRO_T(type, __VA_ARGS__);

template<typename>
class Class;

template<typename T>
void describeClass(Class<T>&)
{
}

}

TYPE_DESCRIPTOR(void, is_void);
TYPE_DESCRIPTOR(int, is_plain);
TYPE_DESCRIPTOR(int64_t, is_plain);
TYPE_DESCRIPTOR(char, is_plain);
TYPE_DESCRIPTOR(int8_t, is_plain);
TYPE_DESCRIPTOR(short, is_plain);
TYPE_DESCRIPTOR(float, is_plain);
TYPE_DESCRIPTOR(double, is_plain);
TYPE_DESCRIPTOR(bool, is_plain);
TYPE_DESCRIPTOR(std::string, is_plain);

TYPE_DESCRIPTOR_T(std::pair, typename T, typename U);
TYPE_DESCRIPTOR_T(std::vector, typename T);
TYPE_DESCRIPTOR_T(std::unique_ptr, typename T);
TYPE_DESCRIPTOR_T(std::shared_ptr, typename T);
TYPE_DESCRIPTOR_T(std::function, typename F);

#endif // REFLECT_TypeDescriptor_h
