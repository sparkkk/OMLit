#pragma once

#include "../common/common.hpp"
#include "../core/object.hpp"

namespace omlit
{
    template<typename HostType, typename StorageType>
    class HostHolder
    {
    public:
        HostHolder(const StorageType & storage) : mStorage(storage)
        {

        }
        HostHolder(const HostHolder<HostType, StorageType> & other)
        : mStorage(other.mStorage)
        {

        }
        HostType * get()
        {
            return static_cast<HostType *>(&mStorage);
        }
    private:
        StorageType mStorage;
    };

    template<typename HostType>
    class HostHolder<HostType, HostType*>
    {
    public:
        HostHolder(const HostType* & storage) : mStorage(storage)
        {

        }
        HostHolder(const HostHolder<HostType, HostType*> & other)
        : mStorage(other.mStorage)
        {

        }
        HostType * get()
        {
            return mStorage;
        }
    private:
        HostType* mStorage;
    };

    template<typename HostType>
    class HostHolder<HostType, std::shared_ptr<HostType>>
    {
    public:
        HostHolder(const std::shared_ptr<HostType> & storage) : mStorage(storage)
        {

        }
        HostHolder(const HostHolder<HostType, std::shared_ptr<HostType>> & other)
        : mStorage(other.mStorage)
        {

        }
        HostType * get()
        {
            return mStorage.get();
        }
    private:
        std::shared_ptr<HostType> mStorage;
    };

    template<typename HostType>
    class HostedCustomType : public Value::CustomType
    {
    public:
        virtual HostType * GetHost() = 0;
    };

    template<typename HostType, typename StorageType>
    class WrapperCustomType : public HostedCustomType<HostType>
    {
    public:
        WrapperCustomType(const StorageType & storage) :
            mHostHolder(storage)
        {

        }
        virtual HostType * GetHost() override
        {
            return mHostHolder.get();
        }
    private:
        HostHolder<HostType, StorageType> mHostHolder;
    };

    template<typename HostType>
    class ValueWrapper
    {
    public:
        template<typename StorageType>
        static Value Wrap(const StorageType & host)
        {
            std::shared_ptr<Value::CustomType> res =
                std::make_shared<WrapperCustomType<HostType, StorageType>>(host);
            return res;
        }
        static HostType * Extract(const Value & val)
        {
            auto custom = val.asCustom();
            if (custom == nullptr)
            {
                return nullptr;
            }
            return std::static_pointer_cast<HostedCustomType<HostType>, Value::CustomType>(custom)->GetHost();
        }
    };

    template<typename HostType>
    static HostType * ExtractFromObj(const Value & objValue, const std::string & id, Error & err)
    {
        auto obj = objValue.asObj();
        if (obj == nullptr)
        {
            err = INCORRECT_INPUT_TYPE;
            return nullptr;
        }
        Value instValue;
        if ((err = obj->GetMember(id, instValue)) != SUCCEED)
        {
            return nullptr;
        }
        auto * host = ValueWrapper<HostType>::Extract(instValue);
        if (host == nullptr)
        {
            err = INNER_ERROR;
        }
        return host;
    }
}