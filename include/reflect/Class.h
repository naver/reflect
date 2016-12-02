
#ifndef REFLECT_Class_h
#define REFLECT_Class_h

#include "Field.h"
#include "Method.h"

#include <functional>
#include <map>
#include <memory>
#include <set>

namespace reflect {

template<typename>
class Class;

class ClassSharedData {
    template<typename> friend class ClassBase;
public:
    ClassSharedData();

private:
    static int64_t nextTypeid;
    int64_t m_typeid;

    std::map<std::string, std::shared_ptr<Method>> m_methods;
    std::map<std::string, std::shared_ptr<Field>> m_fields;
    std::set<void*> m_instances;
};

class ClassUnknown {
public:
    static ClassUnknown* getClass(void*);
    static ClassUnknown* getClass(const std::shared_ptr<void>& ptr) { return getClass(ptr.get()); }
    virtual std::string getName() const = 0;

    std::shared_ptr<Method> getDeclaredMethod(const std::string&) const;
    template<typename... P> std::shared_ptr<Method> getDeclaredMethod(const std::string&, const Class<P>&...) const;
    virtual std::vector<std::shared_ptr<Method>> getDeclaredMethods() const = 0;

    std::shared_ptr<Field> getDeclaredField(const std::string&) const;
    virtual std::vector<std::shared_ptr<Field>> getDeclaredFields() const = 0;

    bool isAssignableFrom(ClassUnknown&) const;
    virtual bool isInstance(void*) const;
    bool isInstance(const std::shared_ptr<void>& ptr) const { return isInstance(ptr.get()); }

protected:
    virtual int64_t getTypeid() const = 0;
    virtual std::shared_ptr<Method> getDeclaredMethodFromDescription(const std::string&) const = 0;
    virtual std::shared_ptr<Field> getDeclaredFieldFromDescription(const std::string&) const = 0;

    void trackInstance(void*);
    void forgetInstance(void*);

    static ClassSharedData& classSharedData(const std::string&);

private:
    static std::map<void*, ClassUnknown*>& instances();
};

enum LeakPtrTag { LeakPtr };

template<typename T>
class ClassBase : public ClassUnknown {
public:
    std::vector<std::shared_ptr<Method>> getDeclaredMethods() const override;
    std::vector<std::shared_ptr<Field>> getDeclaredFields() const override;

    template<typename... Args> T* newInstance(LeakPtrTag, Args&&... args);
    template<typename... Args> std::shared_ptr<T> newInstance(Args&&... args);
    bool isInstance(void*) const override;
    bool isInstance(const std::shared_ptr<void>& ptr) const { return isInstance(ptr.get()); }

protected:
    template<typename M> void setDeclaredMethod(const std::string&, M);
    template<typename F> void setDeclaredField(const std::string&, F);

    int64_t getTypeid() const override { return m_sharedData->m_typeid; }
    std::shared_ptr<Method> getDeclaredMethodFromDescription(const std::string&) const override;
    std::shared_ptr<Field> getDeclaredFieldFromDescription(const std::string&) const override;

    void deleteInstance(T*);

    ClassBase() { if (!m_sharedData) m_sharedData = &ClassUnknown::classSharedData(TypeDescriptor<T>::descriptor()); }

private:
    static ClassSharedData* m_sharedData;
};

template<typename T, bool is_pointer, bool is_reference> struct remove_const_if_helper { typedef T type; };
template<typename T> struct remove_const_if_helper<T, false, false> { typedef typename std::remove_cv<T>::type type; };
template<typename T> struct remove_const_if : public remove_const_if_helper<T, std::is_pointer<T>::value, std::is_reference<T>::value> { };

template<typename T>
class Class : public ClassBase<typename TypeValueTraits<T>::basetype> {
    friend T;
    friend void describeClass<T>(Class&);
public:
    Class();

    std::string getName() const override { return TypeDescriptor<typename remove_const_if<T>::type>::descriptor(); }
};

inline ClassUnknown* ClassUnknown::getClass(void* instance)
{
    if (!instance || instances().count(instance) == 0)
        return nullptr;

    return instances()[instance];
}

inline std::shared_ptr<Method> ClassUnknown::getDeclaredMethod(const std::string& methodName) const
{
    std::string descriptor = methodDescriptor(getName(), methodName, 0, nullptr);
    return getDeclaredMethodFromDescription(descriptor);
}

template<typename... P>
std::shared_ptr<Method> ClassUnknown::getDeclaredMethod(const std::string& methodName, const Class<P>&... parameterTypes) const
{
    std::string parameterTypeDescriptors[] = { parameterTypes.getName()... };
    std::string descriptor = methodDescriptor(getName(), methodName, sizeof...(P), parameterTypeDescriptors);
    return getDeclaredMethodFromDescription(descriptor);
}

inline std::shared_ptr<Field> ClassUnknown::getDeclaredField(const std::string& fieldName) const
{
    std::string descriptor = fieldDescriptor(getName(), fieldName);
    return getDeclaredFieldFromDescription(descriptor);
}

inline bool ClassUnknown::isAssignableFrom(ClassUnknown& other) const
{
    if (getTypeid() == other.getTypeid())
        return true;

    return false;
}

inline bool ClassUnknown::isInstance(void* ptr) const
{
    return instances().count(ptr) > 0;
}

inline void ClassUnknown::trackInstance(void* instance)
{
    instances()[instance] = this;
}

inline void ClassUnknown::forgetInstance(void* instance)
{
    instances().erase(instance);
}

template<typename T> ClassSharedData* ClassBase<T>::m_sharedData;

template<typename T>
std::vector<std::shared_ptr<Method>> ClassBase<T>::getDeclaredMethods() const
{
    std::vector<std::shared_ptr<Method>> methods;
    for (auto& method : m_sharedData->m_methods)
        methods.push_back(method.second);
    return methods;
}

template<typename T>
std::vector<std::shared_ptr<Field>> ClassBase<T>::getDeclaredFields() const
{
    std::vector<std::shared_ptr<Field>> fields;
    for (auto& field : m_sharedData->m_fields)
        fields.push_back(field.second);
    return fields;
}

template<typename T> template<typename... Args>
T* ClassBase<T>::newInstance(LeakPtrTag, Args&&... args)
{
    T* instance = new T(args...);
    ClassUnknown::trackInstance(instance);
    m_sharedData->m_instances.insert(instance);
    return instance;
}

template<typename T> template<typename... Args>
std::shared_ptr<T> ClassBase<T>::newInstance(Args&&... args)
{
    T* instance = newInstance(LeakPtr, std::forward(args)...);

    struct Deleter {
        void operator()(T* ptr) const {
            m_deleter(ptr);
        }

        std::function<void (T*)> m_deleter;
    };

    return std::shared_ptr<T>(instance, Deleter({
        [=] (T* ptr) { deleteInstance(ptr); }
    }));
}

template<typename T>
void ClassBase<T>::deleteInstance(T* instance)
{
    ClassUnknown::forgetInstance(instance);
    m_sharedData->m_instances.erase(instance);
    delete instance;
}

template<typename T>
bool ClassBase<T>::isInstance(void* ptr) const
{
    return m_sharedData->m_instances.count(ptr) > 0;
}

template<typename T> template<typename M>
void ClassBase<T>::setDeclaredMethod(const std::string& methodName, M method)
{
    std::string descriptor = MethodDescriptor<M>::descriptor(getName(), methodName);
    m_sharedData->m_methods[descriptor] = std::make_shared<DeducedMethod<M>>(methodName, method);
}

template<typename T> template<typename F>
void ClassBase<T>::setDeclaredField(const std::string& fieldName, F field)
{
    std::string descriptor = FieldDescriptor<F>::descriptor(getName(), fieldName);
    m_sharedData->m_fields[descriptor] = std::make_shared<DeducedField<F>>(fieldName, field);
}

template<typename T>
std::shared_ptr<Method> ClassBase<T>::getDeclaredMethodFromDescription(const std::string& descriptor) const
{
    return m_sharedData->m_methods[descriptor];
}

template<typename T>
std::shared_ptr<Field> ClassBase<T>::getDeclaredFieldFromDescription(const std::string& descriptor) const
{
    return m_sharedData->m_fields[descriptor];
}

template<typename T> Class<T>& getClass()
{
    static bool initialized = false;
    static std::unique_ptr<Class<T>> sharedClass;
    if (!initialized) {
        initialized = true;
        sharedClass = std::make_unique<Class<T>>();
        describeClass(*sharedClass);
    }
    return *sharedClass;
}

template<typename T>
Class<T>::Class()
{
    reflect::getClass<T>();
}

}

#endif // REFLECT_Class_h
