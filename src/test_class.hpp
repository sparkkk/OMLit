#pragma once
#include <omlit/omlit.hpp>
#include <omlit/utils/wrapper_value.hpp>

using namespace omlit;

namespace my_demo
{
    class Test1
    {
    public:
        Test1() = default;
        Test1(int num)
        {
            mNum = num;
        }
        Test1(const Test1 & other) = default;
        Test1 & operator = (const Test1 & other) = default;
        int Add(int a)
        {
            return mNum += a;
        }
        int Sub(int a)
        {
            return mNum -= a;
        }
        int Value() const
        {
            return mNum;
        }
    protected:
        int mNum = 0;
    };

    void Register_Test1(std::vector<Object::MemberRegistry> & R)
    {
        R.push_back({
            "Add",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::OBJ, Value::I32, },
                { Value::I32 },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Error err;
                    auto * host = ExtractFromObj<Test1>(paramsIn[0], "<inst>", err);
                    if (host == nullptr)
                    {
                        return err;
                    }
                    paramsOut.emplace_back(host->Add(paramsIn[1]));
                    return SUCCEED;
                }
            )
        });
        R.push_back({
            "Sub",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::OBJ, Value::I32, },
                { Value::I32 },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Error err;
                    auto * host = ExtractFromObj<Test1>(paramsIn[0], "<inst>", err);
                    if (host == nullptr)
                    {
                        return err;
                    }
                    paramsOut.emplace_back(host->Sub(paramsIn[1]));
                    return SUCCEED;
                }
            )
        });
        R.push_back({
            "Value",
            Value::I32,
            [] (Object * thiz, const std::string & id, Value & val) -> Error
            {
                Error err;
                auto * host = ExtractFromObj<Test1>(thiz->shared_from_this(), "<inst>", err);
                if (host == nullptr)
                {
                    return err;
                }
                val = host->Value();
                return SUCCEED;
            },
            nullptr,
            nullptr
        });
    }

    
    void Register_Test1_Class(std::vector<Object::MemberRegistry> & R)
    {
        R.push_back({
            "<CTOR>",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::I32 },
                { Value::OBJ },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Object::MemberRegistry instRegistry =
                    {
                        "<inst>",
                        Value::CTM,
                        Object::KeyValueGetter,
                        nullptr,
                        ValueWrapper<Test1>::Wrap(Test1(paramsIn[0]))
                    };
                    paramsOut.emplace_back(
                        std::make_shared<Object>()
                            ->Extend(Register_Test1)
                            ->Extend(instRegistry));
                    return SUCCEED;
                }
            )
        });
    }

    class Test2 : public Test1
    {
    public:
        using Test1::Test1;
        using Test1::operator=;
        int Mul(int a)
        {
            return mNum *= a;
        }
        int Div(int a)
        {
            return mNum /= a;
        }
        int Mod(int a)
        {
            return mNum %= a;
        }
    };

    void Register_Test2(std::vector<Object::MemberRegistry> & R)
    {
        Register_Test1(R);
        R.push_back({
            "Mul",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::OBJ, Value::I32, },
                { Value::I32 },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Error err;
                    auto * host = ExtractFromObj<Test2>(paramsIn[0], "<inst>", err);
                    if (host == nullptr)
                    {
                        return err;
                    }
                    paramsOut.emplace_back(host->Mul(paramsIn[1]));
                    return SUCCEED;
                }
            )
        });
        R.push_back({
            "Div",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::OBJ, Value::I32, },
                { Value::I32 },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Error err;
                    auto * host = ExtractFromObj<Test2>(paramsIn[0], "<inst>", err);
                    if (host == nullptr)
                    {
                        return err;
                    }
                    paramsOut.emplace_back(host->Div(paramsIn[1]));
                    return SUCCEED;
                }
            )
        });
        R.push_back({
            "Mod",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::OBJ, Value::I32, },
                { Value::I32 },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Error err;
                    auto * host = ExtractFromObj<Test2>(paramsIn[0], "<inst>", err);
                    if (host == nullptr)
                    {
                        return err;
                    }
                    paramsOut.emplace_back(host->Mod(paramsIn[1]));
                    return SUCCEED;
                }
            )
        });
    }
    void Register_Test2_Class(std::vector<Object::MemberRegistry> & R)
    {
        R.push_back({
            "<CTOR>",
            Value::FCN,
            Object::KeyValueGetter,
            nullptr,
            NativeFunc::CreateAsValue(
                { Value::I32 },
                { Value::OBJ },
                [] (const NativeFunc::Params & paramsIn, NativeFunc::Params & paramsOut) -> Error
                {
                    Object::MemberRegistry instRegistry =
                    {
                        "<inst>",
                        Value::CTM,
                        Object::KeyValueGetter,
                        nullptr,
                        ValueWrapper<Test2>::Wrap(Test2(paramsIn[0]))
                    };
                    paramsOut.emplace_back(
                        std::make_shared<Object>()
                            ->Extend(Register_Test2)
                            ->Extend(instRegistry));
                    return SUCCEED;
                }
            )
        });
    }
}