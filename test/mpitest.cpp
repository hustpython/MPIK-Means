#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <string>
#include <functional>
#include <iostream>

using namespace std;
using namespace boost::mpi;
//mpic++ -o mpitest mpitest.cpp -L/usr/local/lib -lboost_mpi -lboost_serialization 
//mpirun -n 3 ./mpitest 同时运行多个进程
int main(int argc,char * argv[])
{
    environment env(argc,argv);
    communicator world;

    int rank = world.rank();
    if(rank == 0)
    {
        for(size_t p=1;p<world.size();++p)
        {
            world.send(p,0,string("a msg from master"));
        }
    }
    else
        {
            string msg;
            world.recv(0,0,msg);
            cout<<"Process "<<rank<<": "<<msg<<endl;
        }
        cout<<endl;
    vector<string>names;
    if(rank == 0)
    {
        names.push_back("zero ");
        names.push_back("one ");
        names.push_back("two ");
        names.push_back("three ");
    }
    broadcast(world,names,0);
    string str,strsum,strsum2;
    str = rank<4?names[rank]:"many";
    reduce(world,str,strsum,plus<string>(),0);
    cout<<"Process "<<rank<<": "<<strsum<<endl;
    all_reduce(world,str,strsum2,plus<string>());
    cout<<"Process "<<rank<<": "<<strsum2<<endl;
    return 0;
}