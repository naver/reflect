# reflect
Reflect is a small, yet efficient Java-inspired library which provides C++ class reflection without enable RTTI. The library allows you to write code in C++, just like writing code in Java using java.lang.reflect package.

## Getting Started
### Windows
* Requires Visual Studio 2015.
* Open vs/reflect.sln, "Build Solution".
* Run test.exe, See what's going on.

### Other Platforms
Currently reflect library does not support platforms except Windows. However, it might not be difficult to make the library to be built and running on non-Windows platforms. Everything you need is a C++ compiler which supports C++11 features.

## Code examples
* First, make a class reflected. Adding TYPE_DESCRIPTOR(class name) will do so.

```
class Reflected {
...
};

TYPE_DESCRIPTOR(Reflected);
```
* You also need to register class members like member variables and methods. This is done by implementing `reflect::describeClass<class name>`.
```
namespace reflect {

template<> void describeClass<Reflected>(Class<Reflected>& d)
{
    d.setDeclaredField("m_intField", &Reflected::m_intField);
    d.setDeclaredMethod("testMethod", &Reflected::testMethod);
    ...
}

}
```

* Then you can instantiate the class in this way, Java-like:

```
    auto type = reflect::getClass<Reflected>();
    auto instance = type.newInstance();
```
* Please note using default `new` C++ keyword cannot create a reflected class instance.

* Then you can access to class member variable like Java, writing code like below:
```
    auto intField = type.getDeclaredField("m_intField");
    intField->set(object, 1000);
```
* Of course you can access to class methods.
```
    auto testMethod = type.getDeclaredMethod("testMethod");
    testMethod->invoke(object);
```
You can see more code examples in test.cpp.

## License
reflect is licensed under BSD license.

```
Copyright (c) 2016 NAVER Corp.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
```
