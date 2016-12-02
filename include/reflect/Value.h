
#ifndef REFLECT_Value_h
#define REFLECT_Value_h

#include "TypeDescriptor.h"

#include <assert.h>
#include <sstream>

namespace reflect {

class ClassUnknown;

class Value {
public:
    virtual bool isVoid() const { return true; }
    virtual bool isNumber() const { return false; }
    virtual bool isBoolean() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isFunction() const { return false; }

    virtual int intValue() const { return std::numeric_limits<int>::quiet_NaN(); }
    virtual void setIntValue(int) { }

    virtual int64_t int64Value() const { return std::numeric_limits<int64_t>::quiet_NaN(); }
    virtual void setInt64Value(int64_t) { }

    virtual char charValue() const { return std::numeric_limits<char>::quiet_NaN(); }
    virtual void setCharValue(char) { }

    virtual int8_t byteValue() const { return std::numeric_limits<int8_t>::quiet_NaN(); }
    virtual void setByteValue(int8_t) { }

    virtual short shortValue() const { return std::numeric_limits<short>::quiet_NaN(); }
    virtual void setShortValue(short) { }

    virtual float floatValue() const { return std::numeric_limits<float>::quiet_NaN(); }
    virtual void setFloatValue(float) { }

    virtual double doubleValue() const { return std::numeric_limits<double>::quiet_NaN(); }
    virtual void setDoubleValue(double) { }

    virtual bool booleanValue() const { return std::numeric_limits<bool>::quiet_NaN(); }
    virtual void setBooleanValue(bool) { }

    virtual std::string stringValue() const { return std::string(); }
    virtual void setStringValue(const std::string&) { }

    operator int() { return intValue(); }
    operator int64_t() { return int64Value(); }
    operator char() { return charValue(); }
    operator int8_t() { return byteValue(); }
    operator short() { return shortValue(); }
    operator float() { return floatValue(); }
    operator double() { return doubleValue(); }
    operator bool() { return booleanValue(); }
    operator std::string() { return stringValue(); }

    template<typename T> operator T&();
    template<typename T> operator T*();

    template<typename R, typename... P> std::function<R (P...)>& functionObject();

    Value() = default;
    virtual ~Value() = default;

    virtual ClassUnknown& getClass() const;

protected:
    virtual void* valueDataReference();
};

template<typename T> Class<T>& getClass();

template<typename T> Value::operator T&()
{
    if (!reflect::getClass<T>().isAssignableFrom(getClass())) {
        assert(false);
    }
    return *(reinterpret_cast<T*>(valueDataReference()));
}

template<typename T> Value::operator T*()
{
    if (!reflect::getClass<T>().isAssignableFrom(getClass())) {
        assert(false);
    }
    return reinterpret_cast<T*>(valueDataReference());
}

template<typename R, typename... P> std::function<R (P...)>& Value::functionObject()
{
    typedef std::function<R (P...)> T;
    if (!isFunction() || !reflect::getClass<T>().isAssignableFrom(getClass())) {
        assert(false);
    }
    return *(reinterpret_cast<T*>(valueDataReference()));
}

template<typename T> static void assign(T& a, const T& b) { a = b; }
template<typename T> static void assign(const T& a, const T& b) { }
template<typename T> static void assign(T& a, const std::string& b) { std::istringstream s(b); s >> a; }
template<typename T> static void assign(const T& a, const std::string& b) { }

template<typename T>
class NumberValue : public Value {
public:
    bool isVoid() const override { return false; }
    bool isNumber() const override { return std::is_arithmetic<T>::value; }
    bool isBoolean() const override { return std::is_same<typename std::decay<T>::type, bool>::value; }

    int intValue() const override { return static_cast<int>(m_value); }
    void setIntValue(int value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    int64_t int64Value() const override { return static_cast<int64_t>(m_value); }
    void setInt64Value(int64_t value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    char charValue() const override { return static_cast<char>(m_value); }
    void setCharValue(char value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    int8_t byteValue() const override { return static_cast<int8_t>(m_value); }
    void setByteValue(int8_t value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    short shortValue() const override { return static_cast<short>(m_value); }
    void setShortValue(short value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    float floatValue() const override { return static_cast<float>(m_value); }
    void setFloatValue(float value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    double doubleValue() const override { return static_cast<double>(m_value); }
    void setDoubleValue(double value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    bool booleanValue() const override { return static_cast<bool>(m_value); }
    void setBooleanValue(bool value) override { assign(m_value, static_cast<std::decay<T>::type>(value)); }

    std::string stringValue() const override { std::stringstream s; s << m_value; return s.str(); }
    void setStringValue(const std::string& str) override { assign(m_value, str); }

    virtual ~NumberValue() = default;

    operator Value*() { return this; }

    ClassUnknown& getClass() const override { return reflect::getClass<T>(); }

protected:
    NumberValue(T value) : m_value(std::forward<T>(value)) { }

    void* valueDataReference() override { return &m_value; }

private:
    T m_value;
};

static void assign(std::string& a, const std::string& b) { a = b; }
static void assign(const std::string& a, const std::string& b) { }
template<typename T> static void assign(std::string& str, T value) { std::stringstream s; s << value; str = s.str(); }
template<typename T> static void assign(const std::string& str, T value) { }

template<typename T> static T toNumber(const std::string& str)
{
    std::istringstream s(str);
    T value;
    s >> value;
    return value;
}

template<typename T>
struct StringValue : public Value {
    bool isVoid() const override { return false; }
    bool isString() const override { return true; }

    int intValue() const override { return toNumber<int>(m_value); }
    void setIntValue(int value) override { assign(m_value, value); }

    int64_t int64Value() const override { return toNumber<int64_t>(m_value); }
    void setInt64Value(int64_t value) override { assign(m_value, value); }

    char charValue() const override { return toNumber<char>(m_value); }
    void setCharValue(char value) override { assign(m_value, value); }

    int8_t byteValue() const override { return toNumber<int8_t>(m_value); }
    void setByteValue(int8_t value) override { assign(m_value, value); }

    short shortValue() const override { return toNumber<short>(m_value); }
    void setShortValue(short value) override { assign(m_value, value); }

    float floatValue() const override { return toNumber<float>(m_value); }
    void setFloatValue(float value) override { assign(m_value, value); }

    double doubleValue() const override { return toNumber<double>(m_value); }
    void setDoubleValue(double value) override { assign(m_value, value); }

    bool booleanValue() const override { return toNumber<bool>(m_value); }
    void setBooleanValue(bool value) override { assign(m_value, value); }

    std::string stringValue() const override { return m_value; }
    void setStringValue(const std::string& str) override { assign(m_value, str); }

    virtual ~StringValue() = default;

    operator Value*() { return this; }

    ClassUnknown& getClass() const override { return reflect::getClass<std::string>(); }

protected:
    StringValue(T value) : m_value(std::forward<T>(value)) { }

    void* valueDataReference() override { return &m_value; }

private:
    T m_value;
};

} // namespace reflect

namespace std {
template<typename R, typename... P>
struct is_function<std::function<R (P...)>> : std::true_type {};
template<typename R, typename... P>
struct is_function<std::function<R (P...)>&> : std::true_type {};
}

namespace reflect {

template<typename T>
struct ObjectValue : public Value {
    bool isVoid() const override { return false; }
    bool isFunction() const override { return std::is_function<T>::value; }

    virtual ~ObjectValue() = default;

    operator Value*() { return this; }

    ClassUnknown& getClass() const override { return reflect::getClass<T>(); }

protected:
    ObjectValue(T value) : m_value(std::forward<T>(value)) { }

    void* valueDataReference() override { return &m_value; }

private:
    T m_value;
};

struct ErrorValue { 
};

typedef Value Void;

template<typename T, bool is_void, bool is_arithmetic, bool is_string>
struct ValueTraits {
    typedef typename T type;
};

template<typename T>
struct ValueBaseType {
    typedef ErrorValue type;
};

template<typename T> struct ValueBaseType<ValueTraits<T, true, false, false>> {
    typedef Value type;
};

template<typename T> struct ValueBaseType<ValueTraits<T, false, true, false>> {
    typedef NumberValue<typename ValueTraits<T, false, true, false>::type> type;
};

template<typename T> struct ValueBaseType<ValueTraits<T, false, false, true>> {
    typedef StringValue<typename ValueTraits<T, false, true, false>::type> type;
};

template<typename T> struct ValueBaseType<ValueTraits<T, false, false, false>> {
    typedef ObjectValue<typename T> type;
};

template<typename T>
struct TypeValueTraits {
    typedef typename std::remove_cv<typename std::remove_reference<typename T>::type>::type basetype;
    typedef ValueTraits<T, std::is_void<basetype>::value, std::is_arithmetic<basetype>::value, std::is_same<basetype, std::string>::value> type;
};

template<typename T>
struct TypeValueBaseType {
    typedef typename ValueBaseType<typename TypeValueTraits<T>::type>::type type;
};

template<typename T>
class TypedValue : public TypeValueBaseType<T>::type {
public:
    TypedValue() = default;
    TypedValue(T t) : TypeValueBaseType<T>::type(std::forward<T>(t)) { }
};

template<typename T>
class ValueCastFunction {
public:
    static T cast(Value& value)
    {
        return static_cast<T>(value);
    }
};

template<typename T>
class ValueCastFunction<T&> {
public:
    static T& cast(Value& value)
    {
        return static_cast<T&>(value);
    }
};

template<typename T>
class ValueCastFunction<T&&> {
public:
    static T cast(Value& value)
    {
        return static_cast<T>(value);
    }
};

template<typename R, typename... P>
class ValueCastFunction<std::function<R (P...)>> {
public:
    static std::function<R (P...)>& cast(Value& value)
    {
        return value.functionObject<R, P...>();
    }
};

} // namespace reflect

#endif // REFLECT_Value_h
