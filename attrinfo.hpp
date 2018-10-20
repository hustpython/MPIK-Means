#ifndef ATTRINFO_H_
#define ATTRINFO_H_

#include<boost/variant.hpp>
#include "null.hpp"
#include "types.hpp"

//////////////////////
enum AttrType 
{
    Unknow,
    Continuous,
    Discrete
};

//////////////////////
class AttrValue 
{
    public:
       friend class DAttrInfo;
       friend class CAttrInfo;
       typedef boost::variant<Real,Size> value_type;
       AttrValue();
    private:
       value_type _value;
};

inline AttrValue::AttrValue(): _value(Null<Size>()) {
    }

//////////////////////

class DAttrInfo;
class CAttrInfo;

//////////////////////
//变量信息基类
class AttrInfo 
{
public:
  AttrInfo(const std::string &name,AttrType type);//每一栏的属性名(id,attr,label,...)和该属性的数据类型(离散或连续)
  virtual ~AttrInfo(){}//虚析构函数
  std::string &name();//返回标签
  AttrType type() const;//返回数据类型
  virtual Real distance(const AttrValue&,const AttrValue&) const = 0;

  virtual void set_d_val(AttrValue&, Size) const;//AttrValue赋值;适用于DAttrInfo
  virtual Size get_d_val(const AttrValue&) const;//获取_value
  virtual void set_c_val(AttrValue&, Real) const;//AttrValue赋值;适用于CAttrInfo
  virtual Real get_c_val(const AttrValue&) const;//获取_value
  virtual bool can_cast_to_d() const;
  virtual bool can_cast_to_c() const;
  virtual DAttrInfo& cast_to_d();
  virtual bool is_unknown(const AttrValue&) const = 0;
  virtual void set_unknown(AttrValue&) const = 0;
private:
   std::string _name;
   AttrType _type;
};

AttrInfo::AttrInfo(const std::string &name, AttrType type)
    : _name(name), _type(type) {
    }
inline std::string& AttrInfo::name() {
        return _name;
    }

inline AttrType AttrInfo::type() const {
    return _type;
}
inline bool AttrInfo::can_cast_to_d() const {
    return false;
}

inline bool AttrInfo::can_cast_to_c() const {
    return false;
}
void AttrInfo::set_d_val(AttrValue&, Size) const {
    }

Size AttrInfo::get_d_val(const AttrValue&) const {
    return 0;
}

void AttrInfo::set_c_val(AttrValue&, Real) const {
}

Real AttrInfo::get_c_val(const AttrValue&) const {
    return 0.0;
}
DAttrInfo& AttrInfo::cast_to_d() {
        return *(DAttrInfo*)NULL;
    }
////////////////////////
#endif