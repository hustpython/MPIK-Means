#ifndef RECORD_H 
#define RECORD_H
#include "dcattrinfo.hpp"
#include "attrinfo.hpp"
#include<boost/shared_ptr.hpp>
////////////////////////模板类
template <typename T>
class Container//基类模板
{
    public:
       void add(const T&val);//将val添加到向量中
       Size size() const; //返回_data的长度
       T& operator[](Size i);//下标索引，建立Schema与data的关系
    protected:
        ~Container(){}
        std::vector<T>_data;
};
template<typename T> 
inline void Container<T>::add(const T&val)
{
    _data.push_back(val);
}
template<typename T>
inline Size Container<T>::size() const {
    return _data.size();
}
template<typename T>
inline T& Container<T>::operator[](Size i){
    return _data[i];
}

class Record;
////////////////////////Schema，负责记录每一栏的属性字符串,如"label,attr，id"
class Schema:public Container<boost::shared_ptr<AttrInfo> >
{
    public:
      virtual ~Schema(){}
      const boost::shared_ptr<DAttrInfo>& labelInfo() const;//标签信息，整形
      const boost::shared_ptr<DAttrInfo>& idInfo() const;//id信息，整形
      boost::shared_ptr<DAttrInfo>& idInfo();//可以修改成员变量,_labelInfo
      boost::shared_ptr<DAttrInfo>& labelInfo();//可以修改成员变量,_idInfo
      void set_label(const boost::shared_ptr<Record>& r,const std::string& val);
      //设置记录的label
      void set_id(boost::shared_ptr<Record>& r,const std::string& val);
      //设置记录的id
    protected:
      boost::shared_ptr<DAttrInfo> _labelInfo;
      boost::shared_ptr<DAttrInfo> _idInfo;
};
inline const boost::shared_ptr<DAttrInfo>& Schema::labelInfo() const{
        return _labelInfo;
    }
inline const boost::shared_ptr<DAttrInfo>& Schema::idInfo() const{
        return _idInfo;
    } 
inline boost::shared_ptr<DAttrInfo>& Schema::labelInfo() {
    return _labelInfo;
} 
inline boost::shared_ptr<DAttrInfo>& Schema::idInfo() {
        return _idInfo;
    }
//////////////////////记录类，负责记录每条具体的数据的值
class Record:public Container<AttrValue>
{
    public: 
      Record(const boost::shared_ptr<Schema>& schema);//构造函数
      const boost::shared_ptr<Schema>& schema() const;
      const AttrValue& labelValue() const;
      const AttrValue& idValue() const;
      AttrValue& labelValue();
      AttrValue& idValue();
      Size get_id() const;
      Size get_label() const;
    private: 
        boost::shared_ptr<Schema> _schema;//通过_schema创建记录
        AttrValue _label;
        AttrValue _id;
};
Record::Record(const boost::shared_ptr<Schema>& schema)
    : _schema(schema) {
        _data.resize(schema->size());
        for(Size i=0;i<_schema->size();++i){ 
            (*_schema)[i]->set_unknown(_data[i]);
        } 
    }
    
inline const boost::shared_ptr<Schema>& Record::schema() 
    		const { 
        return _schema;
    }
inline AttrValue& Record::labelValue() {
        return _label;
    }

inline const AttrValue& Record::labelValue() const {
        return _label;
    }  

inline AttrValue& Record::idValue() {
        return _id;
    }
inline const AttrValue& Record::idValue() const {
        return _id;
    } 
inline Size Record::get_id() const {
        return _schema->idInfo()->get_d_val(_id);
    }

inline Size Record::get_label() const {
    return _schema->labelInfo()->get_d_val(_label);
}

void Schema::set_label(const boost::shared_ptr<Record>& r, 
                   const std::string& val) {
        Size label = _labelInfo->add_value(val); 
        _labelInfo->set_d_val(r->labelValue(), label);
    }
void Schema::set_id(boost::shared_ptr<Record>& r, 
                   const std::string& val) {
        Size id = _idInfo->add_value(val, false); 
        _idInfo->set_d_val(r->idValue(), id);
    }

#endif