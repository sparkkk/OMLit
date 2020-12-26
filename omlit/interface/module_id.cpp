#include <omlit/interface/module_id.hpp>

namespace omlit
{
    bool operator == (const ModuleVersion & v1, const ModuleVersion & v2)
    {
        return v1.n1 == v2.n1 && v1.n2 == v2.n2 && v1.n3 == v2.n3 && v1.n4 == v2.n4;
    }

    bool operator < (const ModuleVersion & v1, const ModuleVersion & v2)
    {
        if (v1.n1 != v2.n1)
        {
            return v1.n1 < v2.n1;
        }
        if (v1.n2 != v2.n2)
        {
            return v1.n2 < v2.n2;
        }
        if (v1.n3 != v2.n3)
        {
            return v1.n3 < v2.n3;
        }
        return v1.n4 < v2.n4;
    }

    bool operator == (const ModuleID & v1, const ModuleID & v2)
    {
        return v1.version == v2.version && v1.name == v2.name;
    }

    
    ModuleVersion ModuleVersion::EARLIEST { 0, 0, 0, 0};
    ModuleVersion ModuleVersion::LATEST { (uint32_t)-1, (uint32_t)-1, (uint32_t)-1, (uint32_t)-1 };
}