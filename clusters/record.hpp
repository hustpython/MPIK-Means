#ifndef RECORD_H 
#define RECORD_H
#include "../datasets/dcattrinfo.hpp"
#include "../datasets/attrinfo.hpp"
#include"../utilities/nnmap.hpp"
#include<boost/shared_ptr.hpp>
#include<fstream>
#include<iomanip>
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
      //boost::shared_ptr<Record>& center();//返回中心点1record,可更改   
    protected: 
      boost::shared_ptr<Record>_center; //成员变量,中心点的record
};
//函数申明 
CenterCluster::CenterCluster(const boost::shared_ptr<Record>& center):_center(center){}
const boost::shared_ptr<Record>& CenterCluster::center() 
        const {
        return _center;
    }
// boost::shared_ptr<Record>& CenterCluster::center()
// {
//     return _center;
// }
//////////////////////////簇的集合//可能要删除一些成员函数
class PClustering:public Container<boost::shared_ptr<Cluster> >  
{
    public:
      PClustering();//构造函数
      friend std::ostream& operator<<(std::ostream& os,
                PClustering& pc);//操作符重载,输出聚类结构相关信息
      void removeEmptyClusters();//移除空的record
      void createClusterID();//创建聚类id
      void save(const std::string& filename);//保存聚类结果相关信息至文件

    private: 
        void print(std::ostream &os);//打印聚类结果相关信息
        void calculate();//更新_CM和_CMGiven
        void crosstab();//将一些聚类结果储存为交叉表

        bool _bCalculated;//如果数据文件没有标签信息,则不需要计算_numclustGiven
        Size _numclust;//聚类数
        Size _numclustGiven;//文件提供的label数
        std::vector<Size> _clustsize;//记录每一簇的数据量
        std::vector<std::string> _clustLabel;//记录原文件中的每个分类的数量
        std::vector<Size> _CM;//每一条记录数据的所属index
        std::vector<Size> _CMGiven;//原文件每一条记录所属标签
        iiiMapB _crosstab;//交叉表储存数据
};
PClustering::PClustering(): _bCalculated(false),
        _numclustGiven(Null<Size>()) {
    }

std::ostream& operator<<(std::ostream& os,
            PClustering& pc) {
        pc.print(os);
        return os;
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

void PClustering::print(std::ostream& os) {
        calculate();

        os<<"Clustering Summary:\n";
        os<<"Number of clusters: "<<_numclust<<'\n';
        for(Size i=0;i<_numclust;++i){
            os<<"Size of Cluster "<<i<<": "<<_clustsize[i]<<'\n';
        }
        os<<'\n';
        if (_numclustGiven != Null<Size>()){
            os<<"Number of given clusters: "
                <<_numclustGiven<<'\n';
            os<<"Cross Tabulation:\n"; 
            std::vector<Size> w;
            w.push_back(13);
            os<<std::setw(w[0])<<std::left<<"Cluster ID";
            for(Size j=0;j<_numclustGiven;++j) {
                w.push_back(_clustLabel[j].size()+3);
                os<<std::setw(w[j+1])<<std::left<<_clustLabel[j];
            }
            os<<'\n';
            for(Size i=0;i<_numclust;++i){
                os<<std::setw(w[0])<<std::left<<i;
                for(Size j=0;j<_numclustGiven;++j) {
                    if(_crosstab.contain_key(i,j)){
                        os<<std::setw(w[j+1])<<std::left
                            <<_crosstab(i,j);
                    } else {
                        os<<std::setw(w[j+1])<<std::left<<0;
                    }
                }
                os<<'\n';
            }
        }
    }
void PClustering::save(const std::string& filename) {
        std::ofstream of;
        of.open(filename.c_str()); 
        print(of);

        of<<"\nCluster Membership\n"; 
        of<<"Record ID, Cluster Index, Cluster Index Given\n";
        for(Size i=0; i<_CM.size();++i) {
            of<<i+1<<", "<<_CM[i];
            if(_numclustGiven == Null<Size>()){
                of<<", NA\n";
                continue;
            }
            of<<", "<<_CMGiven[i]<<'\n';
        }
        of.close();
    }

void PClustering::crosstab() {
        Size c1, c2;
        for(Size i=0; i<_CM.size();++i) {
            c1 = _CM[i];
            c2 = _CMGiven[i];
            if (_crosstab.contain_key(c1,c2)) { 
                _crosstab(c1,c2) += 1;
            } else {
                _crosstab.add_item(c1,c2,1);
            }
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
        crosstab();
        _bCalculated = true;
    }
#endif