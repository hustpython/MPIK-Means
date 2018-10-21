#ifndef DATASET_H
#define DATASET_H

#include"record.hpp"
#include<iostream>
#include<vector>

class Dataset:public Container<boost::shared_ptr<Record> >
{
    public:
      Dataset(const boost::shared_ptr<Schema>&);//构造函数，传入含有属性值的schema
      Size num_attr() const;//返回属性个数
      const boost::shared_ptr<Schema> &schema() const;//返回_schrma
      const AttrValue& operator()(Size i, Size j) const;//返回第i条第j个属性的值
      std::vector<Size> get_CM() const; 
      Dataset& operator=(const Dataset&);
      bool is_numeric() const;
      bool is_categorical() const;
    protected:
      boost::shared_ptr<Schema> _schema;
};
Dataset::Dataset(const boost::shared_ptr<Schema>& schema)
        : _schema(schema) {
    }
inline Size Dataset::num_attr() const 
{
    return _schema->size();
}
inline const boost::shared_ptr<Schema>& Dataset::schema() 
        const {
        return _schema;
    }
inline const AttrValue& 
        Dataset::operator()(Size i, Size j) const {
        return (*_data[i])[j];
    }
bool Dataset::is_numeric() const {
        bool ret = true;
        for(Size i=0;i<_schema->size();++i){
            if( !(*_schema)[i]->can_cast_to_c()) {
                ret = false;
            }
        } 

        return ret;
    }

bool Dataset::is_categorical() const {
    bool ret = true;
    for(Size i=0;i<_schema->size();++i){
        if( !(*_schema)[i]->can_cast_to_d()) {
            ret = false;
        }
    } 

    return ret;
}
#endif