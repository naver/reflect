
#ifndef REFLECT_Field_h
#define REFLECT_Field_h

#include "FieldDescriptor.h"
#include "Value.h"

namespace reflect {

class Field {
public:
    Field(const std::string& name)
        : m_name(name)
    {
    }
    virtual ~Field() = default;

    std::string getName() { return m_name; }

    template<typename T, typename C> T get(C& receiver);
    template<typename T, typename C> void set(C& receiver, T&&);

    virtual std::shared_ptr<Value> get(void* receiver) = 0;
    virtual void set(void* receiver, Value*) = 0;

private:
    std::string m_name;
};

template<typename>
class DeducedField;

template<typename T, typename C>
class DeducedField<T (C::*)> : public Field {
public:
    DeducedField(const std::string& name, T (C::* field))
        : Field(name), m_field(field)
    {
    }

    T get(C& object)
    {
        return object.*m_field;
    }
    void set(C& object, T&& value)
    {
        (object.*m_field) = value;
    }

private:
    std::shared_ptr<Value> get(void* receiver) override;
    void set(void* receiver, Value* value) override;

    T (C::* m_field);
};

template<typename T, typename C>
T Field::get(C& receiver)
{
    return static_cast<T>(*get(static_cast<void*>(&receiver)));
}

template<typename T, typename C>
void Field::set(C& receiver, T&& value)
{
    set(static_cast<void*>(&receiver), static_cast<Value*>(TypedValue<T>(value)));
}

template<typename T, typename C>
std::shared_ptr<Value> DeducedField<T (C::*)>::get(void* receiver)
{
    return std::make_shared<TypedValue<T>>(get(*(static_cast<C*>(receiver))));
}

template<typename T, typename C>
void DeducedField<T (C::*)>::set(void* receiver, Value* value)
{
    set(*(static_cast<C*>(receiver)), static_cast<T>(*value));
}

}

#endif // REFLECT_Field_h
