
#include "ReflectedClass.h"

int Reflected::m_staticInt = 0;

Reflected::Reflected()
    : m_intField(0)
{
}

void Reflected::testMethod()
{
    m_intField += 100;
}

bool Reflected::testMethodWithReturnValue()
{
    return true;
}

void Reflected::testMethodNumberValue(int v)
{
    m_intField += v++;
}

void Reflected::testMethodNumberReference(int& v)
{
    m_intField += v++;
}

void Reflected::testMethodNumberConstValue(const int v)
{
    m_intField += v;
}

void Reflected::testMethodNumberConstReference(const int& v)
{
    m_intField += v;
}

void Reflected::testMethodNumberPointer(int* v)
{
    m_intField += (*v)++;
}

void Reflected::testMethodNumberConstPointer(const int* v)
{
    m_intField += (*v);
}

void Reflected::testMethodNumberRValueReference(int&& v)
{
    m_intField += v++;
}

void Reflected::testMethodStringValue(std::string v)
{
    std::istringstream is(v);
    int value;
    is >> value;
    m_intField += value++;

    std::stringstream os;
    os << value;
    v = os.str();
}

void Reflected::testMethodStringReference(std::string& v)
{
    std::istringstream is(v);
    int value;
    is >> value;
    m_intField += value++;

    std::stringstream os;
    os << value;
    v = os.str();
}

void Reflected::testMethodPairValue(std::pair<int, float> v)
{
    m_intField += v.first++;
    v.second = v.first;
}

void Reflected::testMethodPairReference(std::pair<int, float>& v)
{
    m_intField += v.first++;
    v.second = v.first;
}

void Reflected::testMethodObject(std::shared_ptr<void> o)
{
}

void Reflected::testMethodCallback(std::function<int (int)> f)
{
    int result = f(m_intField);
}

void Reflected::staticMethod()
{
    m_staticInt += 100;
}

void Reflected::staticMethodWithParameters(int v)
{
    m_staticInt += v;
}

namespace reflect {

template<> void describeClass<Reflected>(Class<Reflected>& d)
{
    d.setDeclaredField("m_intField", &Reflected::m_intField);
    d.setDeclaredMethod("testMethod", &Reflected::testMethod);
    d.setDeclaredMethod("testMethodWithReturnValue", &Reflected::testMethodWithReturnValue);
    d.setDeclaredMethod("testMethodNumberValue", &Reflected::testMethodNumberValue);
    d.setDeclaredMethod("testMethodNumberReference", &Reflected::testMethodNumberReference);
    d.setDeclaredMethod("testMethodNumberConstValue", &Reflected::testMethodNumberConstValue);
    d.setDeclaredMethod("testMethodNumberConstReference", &Reflected::testMethodNumberConstReference);
    d.setDeclaredMethod("testMethodNumberPointer", &Reflected::testMethodNumberPointer);
    d.setDeclaredMethod("testMethodNumberConstPointer", &Reflected::testMethodNumberConstPointer);
    d.setDeclaredMethod("testMethodNumberRValueReference", &Reflected::testMethodNumberRValueReference);
    d.setDeclaredMethod("testMethodStringValue", &Reflected::testMethodStringValue);
    d.setDeclaredMethod("testMethodStringReference", &Reflected::testMethodStringReference);
    d.setDeclaredMethod("testMethodPairValue", &Reflected::testMethodPairValue);
    d.setDeclaredMethod("testMethodPairReference", &Reflected::testMethodPairReference);
    d.setDeclaredMethod("testMethodObject", &Reflected::testMethodObject);
    d.setDeclaredMethod("testMethodCallback", &Reflected::testMethodCallback);
    d.setDeclaredMethod("staticMethod", &Reflected::staticMethod);
    d.setDeclaredMethod("staticMethodWithParameters", &Reflected::staticMethodWithParameters);
}

}
