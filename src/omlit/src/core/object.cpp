#include <omlit/core/object.hpp>

namespace omlit
{
    Object::Getter Object::KeyValueGetter = 
    [] (Object * thiz, const std::string & id, Value & val) -> Error
    {
        auto & kvs = thiz->mKeyValues;
        if (kvs.find(id) == kvs.end())
        {
            val = nullptr;
        }
        else
        {
            val = kvs[id];
        }
        return SUCCEED;
    };

    Object::Setter Object::KeyValueSetter =
    [] (Object * thiz, const std::string & id, const Value & val) -> Error
    {
        auto & kvs = thiz->mKeyValues;
        if (val == nullptr)
        {
            kvs.erase(id);
        }
        else
        {
            kvs[id] = val;
        }
        return SUCCEED;
    };

    Error Object::GetMember(const std::string & id, Value & value)
    {
        if (mMembers.find(id) == mMembers.end())
        {
            value = nullptr;
            return INVALID_ID;
        }
        auto & propInfo = mMembers.at(id);
        if (propInfo.getter == nullptr)
        {
            value = nullptr;
            return SUCCEED;
        }
        Value res;
        Error err = propInfo.getter(this, id, res);
        if (err != SUCCEED)
        {
            value = nullptr;
            return err;
        }
        Value cvtValue;
        bool anyType = propInfo.type == Value::NIL;
        bool nilValue = res == nullptr;
        if (!nilValue && !anyType && !res.tryConvert(propInfo.type, cvtValue))
        {
            value = nullptr;
            return INCORRECT_OUTPUT_TYPE;
        }
        value = cvtValue;
        return SUCCEED;
    }

    Error Object::SetMember(const std::string & id, Value & value)
    {
        if (mMembers.find(id) == mMembers.end())
        {
            return INVALID_ID;
        }
        auto & propInfo = mMembers.at(id);
        if (propInfo.setter == nullptr)
        {
            return SUCCEED;
        }
        Value cvtValue;
        bool anyType = propInfo.type == Value::NIL;
        bool nilValue = value == nullptr;
        if (!anyType && !nilValue && !value.tryConvert(propInfo.type, cvtValue))
        {
            return INCORRECT_INPUT_TYPE;
        }
        Error err = propInfo.setter(this, id, cvtValue);
        return err;
    }

    std::shared_ptr<Object> Object::Extend(const Extender extender)
    {
        extender(this->mMembers, this->mKeyValues);
        return shared_from_this();
    }
    
    std::shared_ptr<Object> Object::Extend(std::shared_ptr<Object> source)
    {
        for (auto & entry : source->GetMemInfo())
        {
            mMembers.insert(entry);
        }
        for (auto & entry : source->GetKeyValues())
        {
            mKeyValues.insert(entry);
        }
        return shared_from_this();
    }
    std::shared_ptr<Object> Object::Extend(MemberRegister registerer)
    {
        std::vector<MemberRegistry> R;
        registerer(R);
        for (auto & item : R)
        {
            mMembers[item.id] =
            {
                item.type,
                item.getter,
                item.setter
            };
            mKeyValues[item.id] = std::move(item.defaultValue);
        }
        return shared_from_this();
    }
    
    std::shared_ptr<Object> Object::Extend(const MemberRegistry & registry)
    {
        mMembers[registry.id] =
        {
            registry.type,
            registry.getter,
            registry.setter
        };
        mKeyValues[registry.id] = registry.defaultValue;
        return shared_from_this();
    }
}