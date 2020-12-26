#pragma once

#include "interface.hpp"

namespace omlit
{
    class ModuleRegistry : public IModuleRegistry
    {
    public:
        ModuleRegistry(const ModuleID & id);
        virtual ~ModuleRegistry() = default;
        virtual ModuleID GetModuleID() override;
        virtual void Register(const std::string & id, const Value & value) override;
        virtual bool Query(const std::string & id, Value & outValue) const override;
        virtual void ListIDs(std::vector<std::string> & list) const override;
    private:
        ModuleID mID;
        std::unordered_map<std::string, Value> mDict;
    };

    class LibraryRegistry : public ILibraryRegistry
    {
    public:
        virtual ~LibraryRegistry();
        virtual IModuleRegistry * CreateModule(const ModuleID & id) override;
        virtual IModuleRegistry * FindModule(const ModuleID & id) override;
        virtual void DestroyModule(const ModuleID & id) override;
        virtual void ListIDs(std::vector<ModuleID> & list) const override;
    private:
        std::unordered_set<ModuleID> mModuleIDSet;
        std::unordered_map<std::string, std::map<ModuleVersion, IModuleRegistry*>> mModuleMap;
    };

    class ModuleManagerImpl : public IModuleManager
    {
    public:
        virtual ~ModuleManagerImpl();
        virtual bool LoadLibrary(const std::filesystem::path & path, std::string & msg) override;
        virtual bool QueryModule(const ModuleID & moduleID, IModuleRegistry * & module) override;
    protected:
        std::unordered_set<void *> mHandleSet;
        LibraryRegistry mRegistry;
    };
}