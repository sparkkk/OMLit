#include <omlit/core/value.hpp>

namespace omlit
{

std::string Value::toStr() const
{
    std::stringstream ss;
    switch (type)
    {
    case NIL:
        return "null";
    case BLN:
        return b ? "true" : "false";
    case I32:
        ss << i32;
        return ss.str();
    case I64:
        ss << i64;
        return ss.str();
    case F32:
        ss << F32;
        return ss.str();
    case F64:
        ss << f64;
        return ss.str();
    case STR:
        ss << "\"" << *pStr << "\"";
        return ss.str();
    case ARY:
        ss << "[";
        for (auto & elem : (**pAry))
        {
            ss << elem.toStr() << ",";
        }
        ss << "]";
        return ss.str();
    case TAB:
        ss << "{";
        for (auto & entry : (**pTab))
        {
            ss << entry.first.toStr() << ":" << entry.second.toStr() << ",";
        }
        ss << "}";
        return ss.str();
    case FCN:
        ss << "<Func-" << pFunc->get() << ">";
        return ss.str();
    case OBJ:
        ss << "<Object-" << pObj->get() << ">";
        return ss.str();
    case CTM:
        ss << "<Raw>";
        return ss.str();
    }
    return std::string();
}

bool Value::tryConvert(Type type, Value & outValue) const
{
    if (this->type == type)
    {
        outValue = *this;
        return true;
    }
    switch (this->type)
    {
    case I32:
        switch(type)
        {
        case I64:
            outValue = (int64_t) i32;
            return true;
        case F32:
            outValue = (float) i32;
            return true;
        case F64:
            outValue = (double) i32;
            return true;
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    case I64:
        switch(type)
        {
        case I32:
            outValue = (int32_t) i64;
            return true;
        case F32:
            outValue = (float) i64;
            return true;
        case F64:
            outValue = (double) i64;
            return true;
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    case F32:
        switch(type)
        {
        case I32:
            outValue = (int32_t) f32;
            return true;
        case I64:
            outValue = (int64_t) f32;
            return true;
        case F64:
            outValue = (double) f32;
            return true;
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    case F64:
        switch(type)
        {
        case I32:
            outValue = (int32_t) f64;
            return true;
        case I64:
            outValue = (int64_t) f64;
            return true;
        case F32:
            outValue = (float) f64;
            return true;
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    case ARY:
        switch(type)
        {
        case TAB:
        {
            auto & array = *pAry;
            auto table = std::make_shared<TableType>();
            for (int i = 0; i < array->size(); ++i)
            {
                table->insert({
                    Value(i),
                    array->at(i)
                });
            }
            outValue = table;
            return true;
        }
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    case TAB:
        switch(type)
        {
        case ARY:
        {
            auto & table = *pTab;
            auto array = std::make_shared<ArrayType>();
            for (auto & entry : *table)
            {
                array->push_back(entry.second);
            }
            outValue = array;
            return true;
        }
        case STR:
            outValue = toStr();
            return true;
        default:
            return false;
        }
    default:
        return false;
    }
}


bool operator == (const Value & v1, const Value & v2)
{
    if (v1.type != v2.type)
    {
        return false;
    }
    switch (v1.type)
    {
    case Value::NIL:
        return true;
    case Value::BLN:
        return v1.b == v2.b;
    case Value::I32:
        return v1.i32 == v2.i32;
    case Value::I64:
        return v1.i64 == v2.i64;
    case Value::F32:
        return v1.f32 == v2.f32;
    case Value::F64:
        return v1.f64 == v2.f64;
    case Value::STR:
        return *v1.pStr == *v2.pStr;
    case Value::ARY:
        return *v1.pAry == *v2.pAry;
    case Value::TAB:
        return *v1.pTab == *v2.pTab;
    case Value::FCN:
        return *v1.pFunc == *v2.pFunc;
    case Value::OBJ:
        return *v1.pObj == *v2.pObj;
    case Value::CTM:
        return *v1.pCstm == *v2.pCstm;
    default:
        return false;
    }
}

bool operator != (const Value & v1, const Value & v2)
{
    return !(v1 == v2);
}

bool operator < (const Value & v1, const Value & v2)
{
    if (v1.type != v2.type)
    {
        return false;
    }
    Value::Type type = v1.type;
    switch (type)
    {
    case Value::NIL:
        return false;
    case Value::BLN:
        return (!v1.b) && v2.b;
    case Value::I32:
        return v1.i32 < v2.i32;
    case Value::I64:
        return v1.i64 < v2.i64;
    case Value::F32:
        return v1.f32 < v2.f32;
    case Value::F64:
        return v1.f64 < v2.f64;
    case Value::STR:
        return *v1.pStr < *v2.pStr;
    case Value::ARY:
        return *v1.pAry < *v2.pAry;
    case Value::TAB:
        return *v1.pTab < *v2.pTab;
    case Value::FCN:
        return *v1.pFunc < *v2.pFunc;
    case Value::OBJ:
        return *v1.pObj < *v2.pObj;
    case Value::CTM:
        return *v1.pCstm < *v2.pCstm;
    default:
        return false;
    }
}

}
