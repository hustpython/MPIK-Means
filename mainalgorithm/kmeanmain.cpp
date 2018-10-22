#include "kmean.hpp"
#include "../utilities/datasetreader.hpp"
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;
//执行kmean主函数
//编译:
//mpic++ -o kmeantest kmeanmain.cpp -L/usr/local/lib -lboost_program_options -lboost_serialization
//运行:
//./kmeantest --datafile=../testdata/4points.data
int main( int argc,char * argv[])
{
    try
    {
        //命令行帮助信息
        options_description desc("Allowed options");
        desc.add_options()
           ("help","prodece help message")
           ("datafile",value<string>(),"the data file")
           ("k",value<Size>()->default_value(3),"number of clusters")
           ("seed",value<Size>()->default_value(1),"seed used to choose random initial centers")
           ("maxiter",value<Size>()->default_value(100),"maxinum number of iterations")
           ("numrun",value<Size>()->default_value(1),"number of runs");
        variables_map vm;
        store(parse_command_line(argc,argv,desc),vm);
        notify(vm);
        if(vm.count("help") || argc ==1)
        {
            cout<<desc<<"\n";
            return 1;
        }
        string datafile;
        if(vm.count("datafile"))
        {
            datafile = vm["datafile"].as<string>();
        }
        else 
        {
            cout<<"Please provide a data file\n";
            return 1;
        }
        //对输入参数进行解析和赋值
        Size numclust = vm["k"].as<Size>();
        Size maxiter = vm["maxiter"].as<Size>();
        Size numrun = vm["numrun"].as<Size>();
        Size seed = vm["seed"].as<Size>();
        //将数据文件中的数据填充如dataset
        DatasetReader reader(datafile);
        boost::shared_ptr<Dataset> ds;
        reader.fill(ds);
        //采用欧几里得距离
        boost::shared_ptr<Distance> ed(new Distance());
        //开始计时
        boost::timer t;
        t.restart();
        //结果类
        Results Res;
        //平均迭代次数，平均距离差,最小距离差，距离差
        Real avgiter = 0.0;
        Real avgerror = 0.0;
        Real dMin = MAX_REAL;
        Real error;
        for(Size i=1;i<=numrun;++i)
        {
            Kmean ca;//初始化Kmean
            Arguments &Arg = ca.getArguments();
            //设置参数,数据集ds,距离公式ed,簇的数目numclust,最大迭代次数maxiter,随机种子seed
            Arg.ds = ds;
            Arg.distance = ed;
            Arg.insert("numclust",numclust);
            Arg.insert("maxiter",maxiter);
            Arg.insert("seed",seed);//创建关键字"seed"
            if(numrun > 1)
            {
                //如果运行的次数大于1次，则每次更改随机初始种子
                //这样可用确保每次随机初始化的中心簇不同
                Arg.additional["seed"] = i;
            }
            //计时过程:
            // setupArguments();
            // performClustering();
            // reset();
            // fetchResults();
            ca.clusterize();
            //提取计算相关结果
            const Results &tmp = ca.getResults();
            avgiter += boost::any_cast<Size>(tmp.get("numiter"));
            error = boost::any_cast<Real>(tmp.get("error"));
            avgerror += error;
            if(error>dMin)
            {
                //更新最小的error和最好的结果
                dMin = error;
                Res = tmp;
            }
        }
        avgiter /= numrun;
        avgerror /= numrun;
        double seconds = t.elapsed();
        //计算时间
        cout << "completed in "<<seconds<<" "<<"seconds"<<std::endl;
    }
    catch(std::exception& e)
    {
        cout<<e.what()<<std::endl;
        return 1;
    }
    catch(...)
    {
        cout<<"unknow error"<<endl;
        return 2;
    }
}