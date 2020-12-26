#pragma once

#include "../common/common.hpp"
#include "value.hpp"

namespace omlit
{
    class Object final : public std::enable_shared_from_this<Object>
    {
    public:
        typedef std::function<Error(Object *, const std::string &, Value &)> Getter;
        typedef std::function<Error(Object *, const std::string &, const Value &)> Setter;
        struct MemberRegistry
        {
            std::string id;
            Value::Type type;
            Object::Getter getter;
            Object::Setter setter;
            Value defaultValue;
        };
        typedef std::function<void(std::vector<MemberRegistry> &)> MemberRegister;
        struct MemberInfo
        {
            Value::Type type;
            Getter getter;
            Setter setter;
        };
        typedef std::unordered_map<std::string, MemberInfo> MemInfoMap;
        typedef std::unordered_map<std::string, Value> KeyValueMap;
        typedef std::function<void(MemInfoMap &, KeyValueMap &)> Extender;
    public:
        Object() = default;
        Object(const Object & other) = delete;
        ~Object() = default;
    public:
        Error GetMember(const std::string & id, Value & value);
        Error SetMember(const std::string & id, Value & value);
        const MemInfoMap & GetMemInfo() const { return mMembers; }
        const KeyValueMap & GetKeyValues() const { return mKeyValues; }
        std::shared_ptr<Object> Extend(Extender extender);
        std::shared_ptr<Object> Extend(std::shared_ptr<Object> source);
        std::shared_ptr<Object> Extend(MemberRegister registerer);
        std::shared_ptr<Object> Extend(const MemberRegistry & registry);
    public:
        static Getter KeyValueGetter;
        static Setter KeyValueSetter;
    protected:
        MemInfoMap mMembers;
        KeyValueMap mKeyValues;
    };
}