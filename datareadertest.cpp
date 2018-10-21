#include "datasetreader.hpp"
#include <iostream>
#include<sstream>
#include<iomanip>
using namespace std;
int main()
{
    DatasetReader *p = new DatasetReader("4points.data");
    boost::shared_ptr<Dataset> ds;
    p->fill(ds);

    cout<<setw(10)<<left<<"RecordID";
        for(Size j=0; j<ds->num_attr(); ++j) {
            stringstream ss;
            ss<<"Attribute("<<j+1<<")";
            cout<<setw(14)<<left<<ss.str();
        }
        cout<<setw(10)<<left<<"Label"<<endl;
        for(Size i=0; i<ds->size(); ++i) { 
            cout<<setw(10)<<left<<(*ds)[i]->get_id();
            for(Size j=0; j<ds->num_attr(); ++j) {
                boost::shared_ptr<Schema> schema = ds->schema();
                if((*schema)[j]->can_cast_to_c()) {
                    cout<<setw(14)<<left
                        <<(*schema)[j]->get_c_val((*ds)(i,j));
                } else {
                    cout<<setw(14)<<left
                        <<(*schema)[j]->get_d_val((*ds)(i,j));
                }
            }
            cout<<setw(10)<<left<<(*ds)[i]->get_label()<<endl;
        }
    delete p;
}