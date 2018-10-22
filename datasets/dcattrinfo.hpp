#ifndef DCATTRINFO_H
#define DCATTRINFO_H
#include <vector>
#include "../datasets/attrinfo.hpp"
#include<boost/variant/get.hpp>
#include "../utilities/exceptions.hpp"
//////////////////////离散型数据
class DAttrInfo: public  AttrInfo //继承AttrInfo
{
    public: 
        DAttrInfo(const std::string& name);//构造函数，传入属性字符串
        const std::string& int_to_str(Size i) const;
        Size num_values() const;//获取长度
        Size get_d_val(const AttrValue&) const; //接口定义
        void set_d_val(AttrValue& , Size)const;//接口定义
        Size add_value(const std::string&, 
                bool bAllowDuplicate = true);//将一组离散值加入到_values中,比如“X,X,Y,Z"，
                                                //则values=[X,Y,Z],对应的二进制数字为[0,0,1,2]
                                                //对于属性值，则可以重复，但对于id则具有唯一性，不能重复
        DAttrInfo& cast_to_d();
        Real distance(const AttrValue&, const AttrValue&) const; //比较两个离散型变量的距离   
        bool is_unknown(const AttrValue& av) const;//值有缺省  
        bool can_cast_to_d() const;                           
        void set_unknown(AttrValue&) const;
    protected:
        std::vector<std::string> _values;
};
DAttrInfo::DAttrInfo(const std::string& name)
    : AttrInfo(name,Discrete) {
    }
inline DAttrInfo& DAttrInfo::cast_to_d() {
        return *this;
    }
Size DAttrInfo::num_values() const {
        return _values.size(); //3
    }

Size DAttrInfo::add_value(const std::string& s,
        bool bAllowDuplicate) {
        Size ind = Null<Size>();
        for(Size i=0;i<_values.size();++i) {
            if(_values[i] == s) {
                ind = i;
                break;
            }
        }

        if(ind == Null<Size>()) {
            _values.push_back(s);
            return _values.size()-1;
        } else {
            if(bAllowDuplicate) {
                return ind;
            } else {
                return Null<Size>();
            }
        } 
    }
const std::string& DAttrInfo::int_to_str(Size i) const {
        ASSERT(i>=0 && i<_values.size(), "index out of range");
        return _values[i];
    }
Real DAttrInfo::distance(const AttrValue& av1, 
                             const AttrValue& av2) const { 
        if(is_unknown(av1) && is_unknown(av2)) { 
            return 0.0; //如果两个值都有缺省,则距离为0
        }
        if(is_unknown(av1) ^ is_unknown(av2)) { 
            return 1.0;//如果有一个值缺省,距离为1
        }
        if(boost::get<Size>(av1._value) == 
           boost::get<Size>(av2._value) ) {
            return 0.0;//如果两个值相等，则无差距
        } else {
            return 1.0;//否则为最大距离1
        } 
    }

bool DAttrInfo::is_unknown(const AttrValue& av) const {
        return (boost::get<Size>(av._value) == Null<Size>());
    }
inline bool DAttrInfo::can_cast_to_d() const {
    return true;
}
void DAttrInfo::set_unknown(AttrValue& av) const { 
        av._value = Null<Size>();
    }
inline Size DAttrInfo::get_d_val(const AttrValue& av) const 
{
        return boost::get<Size>(av._value);
}

void DAttrInfo::set_d_val(AttrValue &av, Size value) const
{
       av._value = value;
}
//////////////////////连续型数据
class CAttrInfo: public AttrInfo 
{
    public: 
      CAttrInfo(const std::string& name);//构造函数
      Real distance(const AttrValue&,const AttrValue&)const;//两个距离
      void set_c_val(AttrValue &, Real) const;
      void set_min(Real);//设置最小值
      void set_max(Real);//设置最大值
      Real get_min() const;//获取最小值
      Real get_max() const;//获取最大值
      Real get_c_val(const AttrValue&) const;
      bool is_unknown(const AttrValue&) const;
      bool can_cast_to_c() const;
      void set_unknown(AttrValue&) const;

    protected:
      Real _min;
      Real _max;
};
CAttrInfo::CAttrInfo(const std::string& name)
    : AttrInfo(name, Continuous) { 
        _min = Null<Real>();
        _max = Null<Real>();
    }
inline Real CAttrInfo::get_min() const {
        return _min;
}
inline Real CAttrInfo::get_max() const {
       return _max;
}
inline Real CAttrInfo::get_c_val(const AttrValue& av)
       	const {
        return boost::get<Real>(av._value);
    }
void CAttrInfo::set_c_val(AttrValue& av, Real value)const{
        av._value = value;
    }
inline bool CAttrInfo::can_cast_to_c() const {
    return true;
}
inline void CAttrInfo::set_min(Real min) {
        _min = min;
    }
 inline void CAttrInfo::set_max(Real max) {
        _max = max;
    }
bool CAttrInfo::is_unknown(const AttrValue& av) const {
        return (boost::get<Real>(av._value) == Null<Real>());
    }
void CAttrInfo::set_unknown(AttrValue& av) const {
        av._value = Null<Real>();
    }
Real CAttrInfo::distance(const AttrValue& av1, 
                             const AttrValue& av2) const {
        if(is_unknown(av1) && is_unknown(av2)){
	    return 0.0;
	}

        if(is_unknown(av1) ^ is_unknown(av2)){
	    return 1.0;
        }

        return boost::get<Real>(av1._value) - //当两个变量值已知时则计算其差值
               boost::get<Real>(av2._value);
    }

#endif