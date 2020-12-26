#include <omlit/omlit.hpp>
#include "test_class.hpp"

using namespace omlit;

extern "C"
{
    bool omlit_library_open(omlit::ILibraryRegistry * registry)
    {
        auto * module1 = registry->CreateModule({ "demo", {0,0,0,1} });
        module1->Register(
            "Calc", 
            std::make_shared<omlit::Object>()
                ->Extend(my_demo::Register_Test1_Class));
        auto * module2 = registry->CreateModule({ "demo", {0,0,0,2} });
        module2->Register(
            "Calc", 
            std::make_shared<omlit::Object>()
                ->Extend(my_demo::Register_Test2_Class));
        return true;
    }
}

