#include"../clusters/record.hpp"
#include "../datasets/dataset.hpp"
#include<iostream>
#include<sstream>
#include<iomanip>
using namespace std;
//mpic++ -o datasettest datasettest.cpp -L/usr/local/lib -lboost_program_options -lboost_serialization
int main()
{
    boost::shared_ptr<Schema> schema(new Schema);
    boost::shared_ptr<DAttrInfo> labelInfo(new DAttrInfo("Label"));
    boost::shared_ptr<DAttrInfo>idInfo(new DAttrInfo("id"));
    schema->labelInfo() = labelInfo;
    schema->idInfo() = idInfo;
    
    stringstream ss;
    boost::shared_ptr<AttrInfo> ai;
    for(Size j=0;j<3;++j)
    {
        ss.str("");
        ss<<"Attr"<<j+1;
        if(j==0||j==2)
        {
            ai = boost::shared_ptr<CAttrInfo>(new CAttrInfo(ss.str()));
        }
        else{
            ai = boost::shared_ptr<DAttrInfo>(new DAttrInfo(ss.str()));
        }
        schema->add(ai);
    }
    boost::shared_ptr<Dataset> ds(new Dataset(schema));
    Size val;
    boost::shared_ptr<Record> r;

    r = boost::shared_ptr<Record>(new Record(schema));
    schema->set_id(r,"r1");
    schema->set_label(r,"1");
    (*schema)[0]->set_c_val((*r)[0],1.2);
    val = (*schema)[1]->cast_to_d().add_value("A");
    (*schema)[1]->set_d_val((*r)[1],val);
    (*schema)[2]->set_c_val((*r)[2],-0.5);
    ds->add(r);

    r = boost::shared_ptr<Record>(new Record(schema));
    schema->set_id(r, "r2");
    schema->set_label(r, "2");
    (*schema)[0]->set_c_val((*r)[0], -2.1);
    val = (*schema)[1]->cast_to_d().add_value("B");
    (*schema)[1]->set_d_val((*r)[1], val);
    (*schema)[2]->set_c_val((*r)[2], 1.5);
    ds->add(r);

    r = boost::shared_ptr<Record>(new Record(schema));
    schema->set_id(r, "r3");
    schema->set_label(r, "1");
    (*schema)[0]->set_c_val((*r)[0], 1.5);
    val = (*schema)[1]->cast_to_d().add_value("A");
    (*schema)[1]->set_d_val((*r)[1], val);
    (*schema)[2]->set_c_val((*r)[2], -0.1);
    ds->add(r);

    cout<<"Data: \n";
    cout<<setw(10)<<left<<"RecordID";
    for(Size j=0; j<ds->num_attr(); ++j) {
        stringstream ss;
        ss<<"Attr("<<j+1<<")";
        cout<<setw(10)<<left<<ss.str();
    }
    cout<<setw(6)<<left<<"Label"<<endl;
    for(Size i=0; i<ds->size(); ++i) { 
        cout<<setw(10)<<left<<(*ds)[i]->get_id();
        for(Size j=0; j<ds->num_attr(); ++j) {
            if((*schema)[j]->can_cast_to_c()) {
                cout<<setw(10)<<left
                    <<(*schema)[j]->get_c_val((*ds)(i,j));
            } else {
                cout<<setw(10)<<left
                    <<(*schema)[j]->get_d_val((*ds)(i,j));
            }
        }
        cout<<setw(6)<<left<<(*ds)[i]->get_label()<<endl;
    }
    return 0;  
}