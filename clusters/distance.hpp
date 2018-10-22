//这里采用的是欧几里得距离算法
#ifndef DISTANCE_H
#define DISTANCE_H
#include <boost/shared_ptr.hpp>
#include "record.hpp"
#include <functional>
#include <cmath>

//一个带有三个参数的模板类：前两个指定操作数的类型，第三个参数指定函数返回值的类型。在classDistance中
//，这两个操作数是指向数据集记录的共享指针，返回值是实数。
class Distance:std::binary_function<boost::shared_ptr<Record>,boost::shared_ptr<Record>,Real> 
{
    public:
       Real operator()(const boost::shared_ptr<Record>&,const boost::shared_ptr<Record>&);
};
Real Distance::operator()(const boost::shared_ptr<Record>& x,const boost::shared_ptr<Record>& y)
{
    boost::shared_ptr<Schema> schema = x->schema();
    Real temp = 0.0;
    for(Size i=0;i<schema->size();++i)
    {
        //(*schema)[i]:DAttrivalue/CAttrivalue;   (*x)[i]:Attrivalue
        temp += std::pow(std::fabs((*schema)[i]->distance((*x)[i],(*y)[i])),2);
    }
    return std::pow(temp,0.5);
}
#endif