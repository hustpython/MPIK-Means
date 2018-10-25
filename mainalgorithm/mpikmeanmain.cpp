#include<boost/timer.hpp>
#include<boost/mpi.hpp>
#include<boost/program_options.hpp>
#include<iostream>
#include<sstream>
#include<iomanip>
#include<functional>
#include "mpikmean.hpp"
#include "../utilities/datasetreader.hpp"

using namespace std;
using namespace boost::program_options;
namespace mpi=boost::mpi;
//mpic++ -o mpikmean mpikmeanmain.cpp -L/usr/local/lib -lboost_program_options -lboost_mpi -lboost_serialization
//mpirun -n 8 ./mpikmean --datafile=../testdata/15000points.csv --k=10 --numrun=50
int main(int ac, char* av[]){
    try{
        mpi::environment env(ac, av);
        mpi::communicator world;

        options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("datafile", value<string>(), "the data file")
            ("k", value<Size>()->default_value(3), 
             "number of clusters")
            ("seed", value<Size>()->default_value(1), 
             "seed used to choose random initial centers")
            ("maxiter", value<Size>()->default_value(100), 
             "maximum number of iterations")
            ("numrun", value<Size>()->default_value(1), 
             "number of runs");
        variables_map vm;        
        store(parse_command_line(ac, av, desc), vm);
        notify(vm);    
        if (vm.count("help") || ac==1) {
            cout << desc << "\n";
            return 1;
        }
        Size numclust = vm["k"].as<Size>(); 
        Size maxiter = vm["maxiter"].as<Size>(); 
        Size numrun = vm["numrun"].as<Size>(); 
        Size seed = vm["seed"].as<Size>();
        string datafile;
        if (vm.count("datafile")) {
            datafile = vm["datafile"].as<string>();
        } else {
            cout << "Please provide a data file\n";
            return 1;
        }
        boost::shared_ptr<Dataset> ds; 
        if (world.rank() ==0) {
            DatasetReader reader(datafile);
            reader.fill(ds);
        }
        boost::timer t;
        t.restart();
        Results Res;
        Real avgiter = 0.0;
        Real avgerror = 0.0;
        Real dMin = MAX_REAL;
        Real error;
        for(Size i=1; i<=numrun; ++i) {
            MPIKmean ca;
            Arguments &Arg = ca.getArguments();
            Arg.ds = ds;
            Arg.insert("numclust", numclust);
            Arg.insert("maxiter", maxiter);
            Arg.insert("seed", seed);
            if (numrun == 1) {
                Arg.additional["seed"] = seed;
            } else {
                Arg.additional["seed"] = i;
            }
            ca.clusterize();
            if(world.rank() == 0) { 
                const Results &tmp = ca.getResults();
                avgiter += 
                    boost::any_cast<Size>(tmp.get("numiter"));
                error = boost::any_cast<Real>(tmp.get("error"));
                avgerror += error;  
                if (error < dMin) {
                    dMin = error;
                    Res = tmp;
                }
            }
        }
        double seconds = t.elapsed();
        if(world.rank() == 0) {
            avgiter /= numrun;
            avgerror /= numrun;
            std::cout<<"completed in "<<seconds
                <<" seconds"<<std::endl;
            std::cout<<"number of processes: "
                <<world.size()<<std::endl;
            PClustering pc = 
                boost::any_cast<PClustering>(Res.get("pc"));
            std::cout<<pc<<std::endl;
            std::cout<<"Number of runs: "<<numrun<<std::endl;
            std::cout<<"Average number of iterations: "
                <<avgiter<<std::endl;
            std::cout<<"Average error: "<<avgerror<<std::endl;
            std::cout<<"Best error: "<<dMin<<std::endl;
            std::string prefix;
            size_t ind = datafile.find_last_of('.');
            if(ind != std::string::npos ) {
                prefix = datafile.substr(0,ind);
            } else {
                prefix = datafile;
            }
            std::stringstream ss;
            ss<<prefix<<"-kmean-k"<<numclust<<"-s"<<seed<<".txt";
            pc.save(ss.str());
        }
        return 0;
    } catch (std::exception& e) {
        std::cout<<e.what()<<std::endl;
        return 1;
    } catch (...){
        std::cout<<"unknown error"<<std::endl;
        return 2;
    }
}
