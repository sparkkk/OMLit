#pragma once

#include "../common/common.hpp"

namespace omlit
{
    struct ModuleVersion
    {
        static ModuleVersion EARLIEST;
        static ModuleVersion LATEST;
        uint32_t n1, n2, n3, n4;
    };
    bool operator == (const ModuleVersion & v1, const ModuleVersion & v2);
    bool operator < (const ModuleVersion & v1, const ModuleVersion & v2);

    struct ModuleID
    {
        std::string name;
        ModuleVersion version;
    };
    bool operator == (const ModuleID & v1, const ModuleID & v2);
}

namespace std
{
    template<>
    struct hash<omlit::ModuleVersion>
    {
        std::size_t operator()(omlit::ModuleVersion const & value) const noexcept
        {
            return std::hash<uint32_t>{}(value.n1) ^ 
                std::hash<uint32_t>{}(value.n2) ^ 
                std::hash<uint32_t>{}(value.n3) ^ 
                std::hash<uint32_t>{}(value.n4);
        }
    };
    template<>
    struct hash<omlit::ModuleID>
    {
        std::size_t operator()(omlit::ModuleID const & value) const noexcept
        {
            return std::hash<omlit::ModuleVersion>{}(value.version) ^
                std::hash<std::string>{}(value.name);
        }
    };
}