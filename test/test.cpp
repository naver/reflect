
#include "stdafx.h"

#include "ReflectedClass.h"

int _tmain(int argc, _TCHAR* argv[])
{
    auto type = reflect::getClass<Reflected>();
    auto instance = type.newInstance();
    assert(type.ClassUnknown::isInstance(instance));
    assert(type.isInstance(instance));
    auto instanceType = reflect::ClassUnknown::getClass(instance);
    assert(instanceType->isInstance(instance));
    Reflected& object = *instance;

    // Test field reflection.
    auto intField = type.getDeclaredField("m_intField");
    intField->set(object, 1000);
    assert(object.m_intField == 1000);
    int intFieldValue = intField->get<int>(object);
    assert(intFieldValue == 1000);
    double intFieldValueAsDouble = *intField->get(&object);
    assert(intFieldValueAsDouble == 1000.0);

    // Test method reflection.
    auto testMethod = type.getDeclaredMethod("testMethod");
    testMethod->invoke(object);
    assert(object.m_intField == 1100);
    auto parameterTypes = testMethod->getParameterTypes();
    assert(parameterTypes.size() == 0);
    auto testMethodWithReturnValue = type.getDeclaredMethod("testMethodWithReturnValue");
    auto returnValue = testMethodWithReturnValue->invoke(object);
    int value = 10;
    auto testMethodNumberValue = type.getDeclaredMethod("testMethodNumberValue", reflect::Class<int>());
    testMethodNumberValue->invoke(object, value);
    assert(object.m_intField == 1110 && value == 10);
    auto parameterTypes1 = testMethodNumberValue->getParameterTypes();
    assert(parameterTypes1.size() == 1);
    auto testMethodNumberReference = type.getDeclaredMethod("testMethodNumberReference", reflect::Class<int&>());
    testMethodNumberReference->invoke(object, value);
    assert(object.m_intField == 1120 && value == 11);
    auto testMethodNumberConstValue = type.getDeclaredMethod("testMethodNumberConstValue", reflect::Class<const int>()); // const int is evaluated as int.
    testMethodNumberConstValue->invoke(object, value);
    assert(object.m_intField == 1131 && value == 11);
    auto testMethodNumberConstReference = type.getDeclaredMethod("testMethodNumberConstReference", reflect::Class<const int&>());
    testMethodNumberConstReference->invoke(object, value);
    assert(object.m_intField == 1142 && value == 11);
    auto testMethodNumberPointer = type.getDeclaredMethod("testMethodNumberPointer", reflect::Class<int*>());
    testMethodNumberPointer->invoke(object, value); // NOTE: int& -> int*
    assert(object.m_intField == 1153 && value == 12);
    auto testMethodNumberConstPointer = type.getDeclaredMethod("testMethodNumberConstPointer", reflect::Class<const int*>());
    testMethodNumberConstPointer->invoke(object, value);
    assert(object.m_intField == 1165 && value == 12);
    auto testMethodNumberRValueReference = type.getDeclaredMethod("testMethodNumberRValueReference", reflect::Class<int&&>());
    testMethodNumberRValueReference->invoke(object, value);
    assert(object.m_intField == 1177 && value == 12);

    std::string valuestr = "100";
    auto testMethodStringValue = type.getDeclaredMethod("testMethodStringValue", reflect::Class<std::string>());
    testMethodStringValue->invoke(object, valuestr);
    assert(object.m_intField == 1277 && valuestr == "100");
    auto testMethodStringReference = type.getDeclaredMethod("testMethodStringReference", reflect::Class<std::string&>());
    testMethodStringReference->invoke(object, valuestr);
    assert(object.m_intField == 1377 && valuestr == "101");

    std::pair<int, float> valuepair { 10, 0.0f };
    auto testMethodPairValue = type.getDeclaredMethod("testMethodPairValue", reflect::Class<std::pair<int, float>>());
    testMethodPairValue->invoke(object, valuepair);
    assert(object.m_intField == 1387 && valuepair.second == 0.0);
    auto testMethodPairReference = type.getDeclaredMethod("testMethodPairReference", reflect::Class<std::pair<int, float>&>());
    testMethodPairReference->invoke(object, valuepair);
    assert(object.m_intField == 1397 && valuepair.second == 11.0);

    auto sharedInteger = std::make_shared<int>(16);
    std::shared_ptr<void> sharedObject = sharedInteger;
    auto testMethodObject = type.getDeclaredMethod("testMethodObject", reflect::Class<std::shared_ptr<void>>());
    testMethodObject->invoke(object, sharedObject);

    std::function<int (int)> callback = [=] (int param) {
            assert(object.m_intField == param);
            return 0;
        };
    auto testMethodCallback = type.getDeclaredMethod("testMethodCallback", reflect::Class<std::function<int (int)>>());
    testMethodCallback->invoke(object, callback);

    auto staticMethod = type.getDeclaredMethod("staticMethod");
    staticMethod->invoke(nullptr);
    assert(object.m_staticInt == 100);
    auto staticMethodWithParameters = type.getDeclaredMethod("staticMethodWithParameters", reflect::Class<int>());
    staticMethodWithParameters->invoke(nullptr, 11);
    assert(object.m_staticInt == 111);

    printf("Test completed\r\n");
    return 0;
}

