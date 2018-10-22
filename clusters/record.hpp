#ifndef RECORD_H 
#define RECORD_H
#include "../datasets/dcattrinfo.hpp"
#include "../datasets/attrinfo.hpp"
#include<boost/shared_ptr.hpp>
////////////////////////模板类
template <typename T>
class Container//基类模板
{
    public:
       typedef typename std::vector<T>::iterator iterator;
       iterator begin();
       iterator end();
       void erase(const T& val);
       void add(const T&val);//将val添加到向量中
       Size size() const; //返回_data的长度
       T& operator[](Size i);//下标索引，建立Schema与data的关系
    protected:
        ~Container(){}
        std::vector<T>_data;
};
template<typename T>
    inline typename Container<T>::iterator Container<T>::begin() {
        return _data.begin();
    }
template<typename T>
    inline typename Container<T>::iterator Container<T>::end() {
        return _data.end();
    }
template<typename T>
    inline void Container<T>::erase(const T& val) {
        for(iterator it=_data.begin();it!=_data.end();++it){
            if(val == *it){
                _data.erase(it);
                break;
            }
        }
    }
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
        _data.resize(schema->size());//初始化_data,长度为schema的长度，元素为Attrivalue类型
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

class Cluster:public Container<boost::shared_ptr<Record> >
{
   public:
        virtual ~Cluster() {}

        void set_id(Size id);
        Size get_id() const;

    protected:
        Size _id;

};
inline void Cluster::set_id(Size id) {
        _id = id;
}

inline Size Cluster::get_id() const {
    return _id;
}
/////////////////////////中心簇类
class CenterCluster : public Cluster
{
    public:
      CenterCluster(){}
      CenterCluster(const boost::shared_ptr<Record>& center);//构造函数传入一个record
      const boost::shared_ptr<Record>& center() const;//返回中心点的record,不可更改
      boost::shared_ptr<Record>& center();//返回中心点1record,可更改   
    protected: 
      boost::shared_ptr<Record>_center; //成员变量,中心点的record
};
//函数申明 
CenterCluster::CenterCluster(const boost::shared_ptr<Record>& center):_center(center){}
const boost::shared_ptr<Record>& CenterCluster::center() 
        const {
        return _center;
    }
boost::shared_ptr<Record>& CenterCluster::center()
{
    return _center;
}
//////////////////////////簇的集合//可能要删除一些成员函数
class PClustering:public Container<boost::shared_ptr<Cluster> >  
{
    public:
      PClustering();//构造函数
      void removeEmptyClusters();
        void createClusterID();

    private: 
        void calculate();
        bool _bCalculated;
        Size _numclust;
        Size _numclustGiven;
        std::vector<Size> _clustsize;
        std::vector<std::string> _clustLabel;
        std::vector<Size> _CM;
        std::vector<Size> _CMGiven;

};
PClustering::PClustering(): _bCalculated(false),
        _numclustGiven(Null<Size>()) {
    }
void PClustering::removeEmptyClusters() {
        std::vector<boost::shared_ptr<Cluster> > 
            temp(_data.begin(), _data.end());
        _data.clear();
        for(iterator it=temp.begin();it!=temp.end();++it){
            if((*it)->size() == 0) {
                continue;
            }
            _data.push_back(*it);
        }
    }
void PClustering::createClusterID() { 
        removeEmptyClusters(); 
        for(Size i=0;i<_data.size();++i){
            _data[i]->set_id(i);
        }
    }
void PClustering::calculate() {
        if(_bCalculated) {
            return;
        }
        createClusterID();
        _numclust = _data.size();
        boost::shared_ptr<Cluster> c;
        boost::shared_ptr<Record> r;
        _CM.resize(
            (*_data[0])[0]->schema()->idInfo()->num_values());
        for(Size i=0;i<_numclust;++i){
            c = _data[i];
            _clustsize.push_back(c->size());
            for(Size j=0;j<c->size();++j){ 
                r = (*c)[j];
                _CM[r->get_id()] = c->get_id();
            }
        }
        boost::shared_ptr<DAttrInfo> info = 
            (*_data[0])[0]->schema()->labelInfo();
        if(!info) {
            _bCalculated = true;
            return;
        }
        _numclustGiven = info->num_values(); 
        for(Size i=0;i<_numclustGiven;++i){
            _clustLabel.push_back(info->int_to_str(i));
        }

        _CMGiven.resize(_CM.size());
        for(Size i=0;i<_numclust;++i){
            c = _data[i];
            for(Size j=0;j<c->size();++j){ 
                r = (*c)[j];
                _CMGiven[r->get_id()] = r->get_label();
            }
        }
        _bCalculated = true;
    }
#endif