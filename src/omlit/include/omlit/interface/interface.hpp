#pragma once
#include "../common/common.hpp"
#include "module_id.hpp"
#include "../core/value.hpp"

namespace omlit
{
    class ILibraryRegistry;

    class IModuleRegistry
    {
    public:
        virtual ~IModuleRegistry() = default;
        virtual ModuleID GetModuleID() = 0;
        virtual void Register(const std::string & id, const Value & value) = 0;
        virtual bool Query(const std::string & id, Value & outValue) const = 0;
        virtual void ListIDs(std::vector<std::string> & list) const = 0;
    };

    class ILibraryRegistry
    {
    public:
        virtual ~ILibraryRegistry() = default;
        virtual IModuleRegistry * CreateModule(const ModuleID & id) = 0;
        virtual IModuleRegistry * FindModule(const ModuleID & id) = 0;
        virtual void DestroyModule(const ModuleID & id) = 0;
        virtual void ListIDs(std::vector<ModuleID> & list) const = 0;
    };
    
    class IModuleManager : public std::enable_shared_from_this<IModuleManager>
    {
    public:
        static const std::filesystem::path IMAGE_PATH;
        static std::shared_ptr<IModuleManager> Create();
    public:
        IModuleManager(const IModuleManager &) = delete;
        virtual ~IModuleManager() = default;
        virtual bool LoadLibrary(const std::filesystem::path & path, std::string & msg) = 0;
        virtual bool QueryModule(const ModuleID & moduleID, IModuleRegistry * & module) = 0;
    protected:
        IModuleManager() = default;
    };
}

