#pragma once

#include "../common/common.hpp"

namespace omlit
{
    class Object;
    class BaseFunc;

    class Value final
    {
    public:
        enum Type
        {
            NIL,
            BLN,
            I32,
            I64,
            F32,
            F64,
            STR,
            ARY,
            TAB,
            FCN,
            OBJ,
            CTM,
        };
        
        typedef bool     bool_t;
        typedef int32_t  i32_t;
        typedef int64_t  i64_t;
        typedef float    f32_t;
        typedef double   f64_t;
        typedef std::vector<Value> ArrayType;
        typedef std::map<Value, Value> TableType;
        struct CustomType
        {
            virtual ~CustomType() = default;
        };
    private:
        Type type = NIL;
        union
        {
            bool_t b;
            i32_t i32;
            i64_t i64;
            f32_t f32;
            f64_t f64;
            std::string * pStr;
            std::shared_ptr<CustomType> * pCstm;
            std::shared_ptr<Object> * pObj;
            std::shared_ptr<ArrayType> * pAry;
            std::shared_ptr<TableType> * pTab;
            std::shared_ptr<BaseFunc> * pFunc;
            void * pAll = nullptr;
        };
    public:
        Value() = default;
        Value(Type type, void * ptr)
        {
            this->type = type;
            this->pAll = ptr;
        }
        Value(std::nullptr_t)
        {
        }
        Value(bool_t v)
        {
            type = BLN;
            b = v;
        }
        Value(i32_t v)
        {
            type = I32;
            i32 = v;
        }
        Value(i64_t v)
        {
            type = I64;
            i64 = v;
        }
        Value(f32_t v)
        {
            type = F32;
            f32 = v;
        }
        Value(f64_t v)
        {
            type = F64;
            f64 = v;
        }
        Value(const std::string & v)
        {
            type = STR;
            pStr = new std::string(v);
        }
        Value(const std::shared_ptr<CustomType> & v)
        {
            if (v != nullptr)
            {
                type = CTM;
                pCstm = new std::shared_ptr<CustomType>(v);
            }
        }
        Value(const std::shared_ptr<Object> & v)
        {
            if (v != nullptr)
            {
                type = OBJ;
                pObj = new std::shared_ptr<Object>(v);
            }
        }
        Value(const std::shared_ptr<ArrayType> & v)
        {
            if (v != nullptr)
            {
                type = ARY;
                pAry = new std::shared_ptr<ArrayType>(v);
            }
        }
        Value(const std::shared_ptr<TableType> & v)
        {
            if (v != nullptr)
            {
                type = TAB;
                pTab = new std::shared_ptr<TableType>(v);
            }
        }
        Value(const std::shared_ptr<BaseFunc> & v)
        {
            if (v != nullptr)
            {
                type = FCN;
                pFunc = new std::shared_ptr<BaseFunc>(v);
            }
        }
        Value(const Value & other)
        {
            copy(other);
        }
        Value(Value && other)
        {
            move(std::move(other));
        }
        ~Value()
        {
            release();
        }
        Value & operator = (const Value & other)
        {
            release();
            copy(other);
            return *this;
        }
        Value & operator = (Value && other)
        {
            release();
            move(std::move(other));
            return *this;
        }
        Type getType() const
        {
            return type;
        }
        operator bool_t() const
        {
            return b;
        }
        operator i32_t() const
        {
            return i32;
        }
        operator i64_t() const
        {
            return i64;
        }
        operator f32_t() const
        {
            return f32;
        }
        operator f64_t() const
        {
            return f64;
        }
        std::string asStr() const
        {
            if (type == STR)
            {
                return *pStr;
            }
            return toStr();
        }
        std::shared_ptr<CustomType> asCustom() const
        {
            if (type == CTM)
            {
                return *pCstm;
            }
            return std::shared_ptr<CustomType>();
        }
        std::shared_ptr<Object> asObj() const
        {
            if (type == OBJ)
            {
                return *pObj;
            }
            return std::shared_ptr<Object>();
        }
        std::shared_ptr<ArrayType> asArray() const
        {
            if (type == ARY)
            {
                return *pAry;
            }
            return std::shared_ptr<ArrayType>();
        }
        std::shared_ptr<TableType> asTable() const
        {
            if (type == TAB)
            {
                return *pTab;
            }
            return std::shared_ptr<TableType>();
        }
        std::shared_ptr<BaseFunc> asFunc() const
        {
            if (type == FCN)
            {
                return *pFunc;
            }
            return std::shared_ptr<BaseFunc>();
        }

        std::string toStr() const;
        bool tryConvert(Type type, Value & outValue) const;
        
    private:
        void release()
        {
            switch(type)
            {
            case STR:
                delete pStr;
                break;
            case ARY:
                delete pAry;
                break;
            case TAB:
                delete pTab;
                break;
            case FCN:
                delete pFunc;
                break;
            case OBJ:
                delete pObj;
                break;
            case CTM:
                delete pCstm;
                break;
            default:
                break;
            }
            type = NIL;
            pAll = nullptr;
        }
        void copy(const Value & other)
        {
            type = other.type;
            switch (type)
            {
            case STR:
                pStr = new std::string(*other.pStr);
                break;
            case ARY:
                pAry = new std::shared_ptr<ArrayType>(*other.pAry);
                break;
            case TAB:
                pTab = new std::shared_ptr<TableType>(*other.pTab);
                break;
            case FCN:
                pFunc = new std::shared_ptr<BaseFunc>(*other.pFunc);
                break;
            case OBJ:
                pObj = new std::shared_ptr<Object>(*other.pObj);
                break;
            case CTM:
                pCstm = new std::shared_ptr<CustomType>(*other.pCstm);
                break;
            default:
                pAll = other.pAll;
                break;
            }
        }
        void move(Value && other)
        {
            type = other.type;
            pAll = other.pAll;
            other.type = NIL;
            other.pAll = nullptr;
        }
        friend bool operator == (const Value & v1, const Value & v2);
        friend bool operator != (const Value & v1, const Value & v2);
        friend bool operator < (const Value & v1, const Value & v2);
    };
    bool operator == (const Value & v1, const Value & v2);
    bool operator != (const Value & v1, const Value & v2);
    bool operator < (const Value & v1, const Value & v2);
}

namespace std
{
    using namespace omlit;
    template<>
    struct hash<Value>
    {
        std::size_t operator()(Value const & value) const noexcept
        {
            switch (value.getType())
            {
            case Value::NIL:
                return std::hash<std::nullptr_t>{}(nullptr);
            case Value::BLN:
                return std::hash<Value::bool_t>{}(value);
            case Value::I32:
                return std::hash<Value::i32_t>{}(value);
            case Value::I64:
                return std::hash<Value::i64_t>{}(value);
            case Value::F32:
                return std::hash<Value::f32_t>{}(value);
            case Value::F64:
                return std::hash<Value::f64_t>{}(value);
            case Value::STR:
                return std::hash<std::string>{}(value.asStr());
            case Value::ARY:
                return std::hash<std::shared_ptr<Value::ArrayType>>{}(value.asArray());
            case Value::TAB:
                return std::hash<std::shared_ptr<Value::TableType>>{}(value.asTable());
            case Value::FCN:
                return std::hash<std::shared_ptr<BaseFunc>>{}(value.asFunc());
            case Value::OBJ:
                return std::hash<std::shared_ptr<Object>>{}(value.asObj());
            case Value::CTM:
                return std::hash<std::shared_ptr<Value::CustomType>>{}(value.asCustom());
            }
        }
    };
}