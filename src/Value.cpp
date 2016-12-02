
#include "Value.h"

#include "Class.h"

namespace reflect {

ClassUnknown& Value::getClass() const
{
    return reflect::getClass<void>();
}

void* Value::valueDataReference()
{
    return nullptr;
}

}
