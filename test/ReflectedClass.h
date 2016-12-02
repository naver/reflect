
#ifndef REFLECT_TEST_ReflectedClass_h
#define REFLECT_TEST_ReflectedClass_h

#include "Class.h"

class Reflected {
public:
    Reflected();
    ~Reflected() = default;

    void testMethod();
    bool testMethodWithReturnValue();
    void testMethodNumberValue(int);
    void testMethodNumberReference(int&);
    void testMethodNumberConstValue(const int);
    void testMethodNumberConstReference(const int&);
    void testMethodNumberPointer(int*);
    void testMethodNumberConstPointer(const int*);
    void testMethodNumberRValueReference(int&&);

    void testMethodStringValue(std::string);
    void testMethodStringReference(std::string&);

    void testMethodPairValue(std::pair<int, float>);
    void testMethodPairReference(std::pair<int, float>&);

    void testMethodObject(std::shared_ptr<void>);
    void testMethodCallback(std::function<int (int)>);

    static void staticMethod();
    static void staticMethodWithParameters(int);

    int m_intField;
    static int m_staticInt;
};

TYPE_DESCRIPTOR(Reflected);

#endif // REFLECT_TEST_ReflectedClass_h
