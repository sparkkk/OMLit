#include <dlfcn.h>
#include <omlit/interface/interface_impl.hpp>


namespace omlit
{
    ModuleRegistry::ModuleRegistry(const ModuleID & id)
    {
        mID = id;
    }
    ModuleID ModuleRegistry::GetModuleID()
    {
        return mID;
    }
    void ModuleRegistry::Register(const std::string & id, const Value & value)
    {
        mDict[id] = value;
    }
    bool ModuleRegistry::Query(const std::string & id, Value & outValue) const
    {
        auto iter = mDict.find(id);
        if (iter == mDict.end())
        {
            return false;
        }
        outValue = iter->second;
        return true;
    }
    void ModuleRegistry::ListIDs(std::vector<std::string> & list) const
    {
        list.clear();
        for (auto & entry : mDict)
        {
            list.emplace_back(entry.first);
        }
    }

    ///////////////////////////////////////

    LibraryRegistry::~LibraryRegistry()
    {
        for (auto & entry : mModuleMap)
        {
            for (auto & subEntry : entry.second)
            {
                delete subEntry.second;
            }
        }
    }
    IModuleRegistry * LibraryRegistry::CreateModule(const ModuleID & id)
    {
        if (mModuleIDSet.find(id) != mModuleIDSet.end())
        {
            return nullptr;
        }
        if (id.version == ModuleVersion::EARLIEST ||
            id.version == ModuleVersion::LATEST)
        {
            return nullptr;
        }
        mModuleIDSet.insert(id);
        auto module = new ModuleRegistry(id);
        mModuleMap[id.name][id.version] = module;
        return module;
    }
    IModuleRegistry * LibraryRegistry::FindModule(const ModuleID & id)
    {
        if (mModuleMap.find(id.name) == mModuleMap.end())
        {
            return nullptr;
        }
        auto & verMap = mModuleMap[id.name];
        if (id.version == ModuleVersion::EARLIEST)
        {
            return verMap.begin()->second;
        }
        if (id.version == ModuleVersion::LATEST)
        {
            return verMap.rbegin()->second;
        }
        if (verMap.find(id.version) == verMap.end())
        {
            return nullptr;
        }
        return verMap[id.version];
    }
    void LibraryRegistry::DestroyModule(const ModuleID & id)
    {
        if (mModuleMap.find(id.name) == mModuleMap.end())
        {
            return;
        }
        auto & verMap = mModuleMap[id.name];
        if (id.version == ModuleVersion::EARLIEST)
        {
            delete verMap.begin()->second;
            verMap.erase(verMap.begin()->first);
        }
        if (id.version == ModuleVersion::LATEST)
        {
            delete verMap.rbegin()->second;
            verMap.erase(verMap.rbegin()->first);
        }
        if (verMap.find(id.version) == verMap.end())
        {
            return;
        }
        delete verMap[id.version];
        verMap.erase(id.version);
    }
    void LibraryRegistry::ListIDs(std::vector<ModuleID> & list) const
    {
        list.clear();
        for (auto & id : mModuleIDSet)
        {
            list.emplace_back(id);
        }
    }

    ///////////////////////////////////////

    ModuleManagerImpl::~ModuleManagerImpl()
    {
        for (auto handle : mHandleSet)
        {
            dlclose(handle);
        }
    }

    bool ModuleManagerImpl::LoadLibrary(const std::filesystem::path & path, std::string & msg)
    {
        const char * cpath = nullptr;
        if (path != IMAGE_PATH)
        {
            cpath = path.string().c_str();
        }
        void * handle = nullptr;
        handle = dlopen(cpath, RTLD_NOW);
        if (handle == nullptr)
        {
            msg = dlerror();
            return false;
        }
        //Keep ref count always be one
        if (mHandleSet.find(handle) != mHandleSet.end())
        {
            dlclose(handle);
            return true;
        }

        mHandleSet.insert(handle);
        //void * testsym = dlsym(handle, "main");
        //printf("main addr %p\n", testsym);

        typedef bool (*LIB_REG_FUNC)(ILibraryRegistry *);

        LIB_REG_FUNC regFunc = reinterpret_cast<LIB_REG_FUNC>(dlsym(handle, "omlit_library_open"));
        if (regFunc == nullptr)
        {
            msg = dlerror();
            return false;
        }
        if (!regFunc(&mRegistry))
        {
            return false;
        }
        return true;
    }
    bool ModuleManagerImpl::QueryModule(const ModuleID & moduleID, IModuleRegistry * & module)
    {
        module = mRegistry.FindModule(moduleID);
        if (module == nullptr)
        {
            return false;
        }
        return true;
    }

    ///////////////////////////////////////
    
    const std::filesystem::path IModuleManager::IMAGE_PATH = ".";
    
    std::shared_ptr<IModuleManager> IModuleManager::Create()
    {
        return std::shared_ptr<IModuleManager>(new ModuleManagerImpl);
    }
}