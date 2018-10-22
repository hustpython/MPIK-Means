#ifndef KMEAN_H
#define KMEAN_H
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/random.hpp>
#include <vector>
#include <map>
#include "../datasets/dataset.hpp"
#include "../utilities/types.hpp"
#include "../clusters/distance.hpp"
#include "../utilities/exceptions.hpp"
/////////////////////////
//Additional 类的申明
class Additional 
{
    public: 
      const::boost::any &get(const std::string& name) const;
      void insert(const std::string& name,const boost::any &val);
      std::map<std::string,boost::any> additional;
    protected:
      Additional(){}
};
///////
const boost::any& Additional::get(const std::string& name) const 
{
    std::map<std::string,boost::any>::const_iterator it;
    it = additional.find(name);
    if(it==additional.end())
    {
        FAIL(name << "not found");
    }
    return it->second;
}
void Additional::insert(const std::string& name,const boost::any& val)
{
    additional.insert(std::pair<std::string,boost::any>(name,val));
}

//函数定义

/////////////////////
class Arguments:public Additional
{
    public:
       boost::shared_ptr<Dataset> ds;
       boost::shared_ptr<Distance> distance;

};
class Results:public Additional  
{
    public:
        void reset();
        std::vector<Size> CM;
};
void Results::reset() {
        CM.clear();
        additional.clear();
    }
    
//////////////////////
class Kmean
{
    public:
       Arguments& getArguments();
       const Results& getResults() const;
       void reset() const;
       void clusterize();
    protected:
        void setupArguments();
        void performClustering() const;//
        void fetchResults() const;//
        void initialization() const;
        void iteration() const;
        void updateCenter() const;
        mutable std::vector<boost::shared_ptr<CenterCluster> >_clusters;
        mutable std::vector<Size> _CM;
        mutable Real _error;
        mutable Size _numiter;
        mutable Results _results;
        boost::shared_ptr<Dataset> _ds;
        Arguments _arguments;
        Size _numclust;
        Size _maxiter;
        Size _seed;
        boost::shared_ptr<Distance> _distance;
};
//函数申明
/////////////////1
Arguments &Kmean::getArguments()
{
    return _arguments;
}
const Results& Kmean::getResults() const 
{
    return _results;
}
void Kmean::reset() const 
{
    _results.reset();
}

void Kmean::clusterize()
{
    setupArguments();
    performClustering();
    reset();
    fetchResults();
}
///////////////////2
void Kmean::performClustering() const 
{
    initialization();
    iteration();
}
void Kmean::setupArguments()
{
    _ds = _arguments.ds;
    //Kmean 算法的数据是连续的数值型
    ASSERT(_ds->is_numeric(),"dataset is not numeric");
    _distance = _arguments.distance;

    _numclust = boost::any_cast<Size>(_arguments.get("numclust"));
    ASSERT(_numclust>=2 && _numclust<=_ds->size(),"invalid numclust");
    //(聚类的簇数大于等于2小于等于数据的总个数)
    _maxiter = boost::any_cast<Size>(_arguments.get("maxiter"));
    ASSERT(_maxiter>0,"invalide maxiter");

    _seed = boost::any_cast<Size>(_arguments.get("seed"));
    ASSERT(_seed>0,"invalide seed");
}
void Kmean::fetchResults() const 
{
    PClustering pc;
    for(Size i=0;i<_clusters.size();++i)
    {
        pc.add(_clusters[i]);
    }
    _results.CM = _CM;
    _results.insert("pc",boost::any(pc));

    _error = 0.0;
    //计算总的errors
    for(Size i=0;i<_ds->size();++i)
    {
        _error += (*_distance)((*_ds)[i],_clusters[_CM[i]]->center());
    }
    _results.insert("error",boost::any(_error));
    _results.insert("numiter",boost::any(_numiter));
}
void Kmean::iteration() const 
{
    bool bChanged = true;

    updateCenter();
    _numiter = 1;
    while(bChanged)
    {
        bChanged = false;
        Size s;
        Real dMin,dDist;
        for(Size i=0;i<_ds->size();++i)
        {
            dMin = MAX_REAL;
            //如果所有的record距离中心点的距离比当前的dMin小则更新dMin;
            for(Size k=0;k<_clusters.size();++k)
            {
                dDist = (*_distance)((*_ds)[i],_clusters[k]->center());
                if(dMin > dDist)
                {
                    dMin = dDist;
                    s = k;//得到第i个record属于_cluster的index
                }
            }
            if(_CM[i] !=s)//如果得到的和之前的所属_cluster不同，则认为更改了
            {
                _clusters[_CM[i]]->erase((*_ds)[i]);
                _clusters[s]->add((*_ds)[i]);
                _CM[i] = s;
                bChanged = true;
            }
        }
        updateCenter();
        ++_numiter;
        //达到最大迭代次数跳出循环
        if(_numiter > _maxiter)
        {
            break;
        }
    }
}
//更新集群中心，均值法
void Kmean::updateCenter() const 
{
    Real dTemp;
    boost::shared_ptr<Schema> schema = _ds->schema();
    for(Size k =0;k<_clusters.size();++k)
    {
        for(Size j=0;j<_clusters[k]->size();++j)
        {
            dTemp = 0.0;
            for(Size i=0;i<_clusters[k]->size();++i)
            {
                boost::shared_ptr<Record> rec = (*_clusters[k])[i];
                dTemp += (*schema)[j]->get_c_val((*rec)[j]);
            }
            (*schema)[j]->set_c_val((*_clusters[k]->center())[j],dTemp/_clusters[k]->size());
        }
    }

}
//随机初始化簇
void Kmean::initialization() const 
{
    Size numRecords = _ds->size();
    std::vector<Size> index(numRecords,0);
    _CM.resize(numRecords);
    for(Size i=0;i<index.size();++i)
    {
        index[i] = i;
    }
    boost::minstd_rand generator(_seed);
    for(Size i=0;i<_numclust;++i)
    {
        boost::uniform_int<> uni_dist(0,numRecords-i-1);
        boost::variate_generator<boost::minstd_rand&,boost::uniform_int<> >uni(generator,uni_dist);
        Size r = uni();
        boost::shared_ptr<Record> cr = boost::shared_ptr<Record>(new Record(*(*_ds)[r]));
        boost::shared_ptr<CenterCluster>c = boost::shared_ptr<CenterCluster>(new CenterCluster(cr));//中心record
        c->set_id(i);
        _clusters.push_back(c);
        index.erase(index.begin()+r);
    }
    Size s;
    Real dMin,dDist;
    for(Size i=0;i<numRecords;++i)
    {
        //对每一个record都与center()进行距离计算
        dMin = MAX_REAL;
        for(Size j= 0;j<_numclust;++j)
        {
            dDist = (*_distance)((*_ds)[i],_clusters[j]->center());
            if(dDist<dMin)
            {
                s = j;
                dMin = dDist;
            }
        }
        _clusters[s]->add((*_ds)[i]);
        //第s个中心簇增加一个属于它的record ((*_ds)[i])
        _CM[i] = s;
        //第i个record的所属类别为s
    }
}
#endif