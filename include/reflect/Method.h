
#ifndef REFLECT_Method_h
#define REFLECT_Method_h

#include "MethodDescriptor.h"
#include "Value.h"

#include <array>

namespace reflect {

class ClassUnknown;

class Method {
public:
    Method(const std::string& name)
        : m_name(name)
    {
    }
    virtual ~Method() = default;

    std::string getName() { return m_name; }
    virtual std::vector<ClassUnknown*> getParameterTypes() = 0;

    template<typename... P> std::shared_ptr<Value> invoke(nullptr_t, P&&...);
    template<typename C, typename... P> std::shared_ptr<Value> invoke(C& receiver, P&&...);

    virtual std::shared_ptr<Value> invoke(nullptr_t, Value*[]) = 0;
    virtual std::shared_ptr<Value> invoke(void* receiver, Value*[]) = 0;

private:
    std::string m_name;
};

template<typename>
class DeducedMethod;

template<typename R, typename... P>
class DeducedMethod<R (*) (P...)> : public Method {
public:
    DeducedMethod(const std::string& name, R (* method) (P...))
        : Method(name), m_method(method)
    {
    }

    R invoke(nullptr_t, P&&... arguments)
    {
        return (*m_method)(std::forward<P>(arguments)...);
    }

private:
    std::vector<ClassUnknown*> getParameterTypes() override;
    std::shared_ptr<Value> invoke(nullptr_t, Value*[]) override;
    std::shared_ptr<Value> invoke(void* receiver, Value*[]) override;

    template<size_t... I> std::shared_ptr<Value> forward(nullptr_t, Value*[], std::index_sequence<I...>);

    R (* m_method) (P...);
};

template<typename R, typename C, typename... P>
class DeducedMethod<R (C::*) (P...)> : public Method {
public:
    DeducedMethod(const std::string& name, R (C::* method) (P...))
        : Method(name), m_method(method)
    {
    }

    R invoke(C& receiver, P&&... arguments)
    {
        return (receiver.*m_method)(std::forward<P>(arguments)...);
    }

private:
    std::vector<ClassUnknown*> getParameterTypes() override;
    std::shared_ptr<Value> invoke(nullptr_t, Value*[]) override;
    std::shared_ptr<Value> invoke(void* receiver, Value*[]) override;

    template<size_t... I> std::shared_ptr<Value> forward(void* receiver, Value*[], std::index_sequence<I...>);

    R (C::* m_method) (P...);
};

template<typename T, size_t N, typename... P, size_t... I>
void tuple_to_array(std::array<T, N>& a, std::tuple<P...>& t, std::index_sequence<I...>)
{
    a = { static_cast<T>(std::get<I>(t))... };
}

template<typename T, typename... P>
auto to_array(std::tuple<P...>& t) -> std::array<T, sizeof...(P)>
{
    std::array<T, sizeof...(P)> result;
    tuple_to_array(result, t, std::make_index_sequence<sizeof...(P)>());
    return result;
}

template<typename... P>
std::shared_ptr<Value> Method::invoke(nullptr_t, P&&... arguments)
{
    std::tuple<TypedValue<P>...> typedValues = std::make_tuple(TypedValue<P>(std::forward<P>(arguments))...);
    auto values = to_array<Value*>(typedValues);
    return invoke(nullptr, values.data());
}

template<typename C, typename... P>
std::shared_ptr<Value> Method::invoke(C& receiver, P&&... arguments)
{
    std::tuple<TypedValue<P>...> typedValues = std::make_tuple(TypedValue<P>(std::forward<P>(arguments))...);
    auto values = to_array<Value*>(typedValues);
    return invoke(&receiver, values.data());
}

template<typename R, typename... P>
std::vector<ClassUnknown*> DeducedMethod<R (*) (P...)>::getParameterTypes()
{
    if (sizeof...(P) == 0)
        return std::vector<ClassUnknown*>();

    auto typeClasses = std::make_tuple(&reflect::getClass<P>()...);
    auto classes = to_array<ClassUnknown*>(typeClasses);
    return std::vector<ClassUnknown*>(sizeof...(P), classes.front());
}

template<typename R, typename... P>
std::shared_ptr<Value> DeducedMethod<R (*) (P...)>::invoke(nullptr_t, Value* values[])
{
    return forward(nullptr, values, std::make_index_sequence<sizeof...(P)>());
}

template<typename R, typename... P>
std::shared_ptr<Value> DeducedMethod<R (*) (P...)>::invoke(void* receiver, Value* values[])
{
    return nullptr;
}

template<typename R, typename... P> template<size_t... I>
std::shared_ptr<Value> DeducedMethod<R (*) (P...)>::forward(nullptr_t, Value* values[], std::index_sequence<I...>)
{
    return reflect::forward<R>(0, m_method, ValueCastFunction<P>::cast(*values[I])...);
}

template<typename R, typename M, typename... P>
std::shared_ptr<Value> forward(typename std::enable_if<std::is_void<R>::value>::type*, M method, P&&... arguments)
{
    (*method)(std::forward<P>(arguments)...);
    return std::make_shared<Void>();
}

template<typename R, typename M, typename... P>
std::shared_ptr<Value> forward(typename std::enable_if<!std::is_void<R>::value>::type*, M method, P&&... arguments)
{
    return std::make_shared<TypedValue<R>>((*method)(std::forward<P>(arguments)...));
}

template<typename R, typename C, typename... P>
std::vector<ClassUnknown*> DeducedMethod<R (C::*) (P...)>::getParameterTypes()
{
    if (sizeof...(P) == 0)
        return std::vector<ClassUnknown*>();

    auto typeClasses = std::make_tuple(&reflect::getClass<P>()...);
    auto classes = to_array<ClassUnknown*>(typeClasses);
    return std::vector<ClassUnknown*>(sizeof...(P), classes.front());
}

template<typename R, typename C, typename... P>
std::shared_ptr<Value> DeducedMethod<R (C::*) (P...)>::invoke(nullptr_t, Value* values[])
{
    return nullptr;
}

template<typename R, typename C, typename... P>
std::shared_ptr<Value> DeducedMethod<R (C::*) (P...)>::invoke(void* receiver, Value* values[])
{
    return forward(receiver, values, std::make_index_sequence<sizeof...(P)>());
}

template<typename R, typename C, typename... P> template<size_t... I>
std::shared_ptr<Value> DeducedMethod<R (C::*) (P...)>::forward(void* receiver, Value* values[], std::index_sequence<I...>)
{
    return reflect::forward<R>(0, m_method, *(static_cast<C*>(receiver)), ValueCastFunction<P>::cast(*values[I])...);
}

template<typename R, typename M, typename C, typename... P>
std::shared_ptr<Value> forward(typename std::enable_if<std::is_void<R>::value>::type*, M method, C& receiver, P&&... arguments)
{
    (receiver.*method)(std::forward<P>(arguments)...);
    return std::make_shared<Void>();
}

template<typename R, typename M, typename C, typename... P>
std::shared_ptr<Value> forward(typename std::enable_if<!std::is_void<R>::value>::type*, M method, C& receiver, P&&... arguments)
{
    return std::make_shared<TypedValue<R>>((receiver.*method)(std::forward<P>(arguments)...));
}

}

#endif // REFLECT_Method_h
