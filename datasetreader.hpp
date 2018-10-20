#ifndef DATASETREADER_H
#define DATASETREADER_H

#include"types.hpp"
#include"dataset.hpp"
#include"attrinfo.hpp"
#include<boost/shared_ptr.hpp>
#include<boost/lexical_cast.hpp>
#include<string>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string.hpp>
#include<fstream>
#include<iostream>

class DatasetReader
{
    public: 
        DatasetReader(const std::string& fileName);//构造函数,传入文件名
        void fill(boost::shared_ptr<Dataset>& ds);//将文件中的数据解析进dataset

    private: 
        void createSchema();//创建schema
        void fillData();
        boost::shared_ptr<Record> createRecord(const std::vector<std::string> &val);
        std::vector<std::string> split(const std::string&);
        std::string _fileName;
        Size _labelColumn; 
        Size _idColumn; 
        Size _numColumn;
        boost::shared_ptr<Schema> _schema;
        boost::shared_ptr<Dataset> _ds;
};
/////////////////
DatasetReader::DatasetReader(const std::string& fileName)
        : _fileName(fileName), _labelColumn(Null<Size>()),
        _idColumn(Null<Size>()), _numColumn(0) {
    }

void DatasetReader::fill(boost::shared_ptr<Dataset>& ds) {
        createSchema();
        fillData();
        if(_idColumn == Null<Size>()) { 
            for(Size i=0; i<_ds->size(); ++i) {
                _schema->set_id((*_ds)[i],boost::lexical_cast<std::string>(i));
            }
        }
        ds = _ds;
    }
void DatasetReader::fillData() {
        std::ifstream file;
        std::string line;
        file.open(_fileName.c_str());
        _ds = boost::shared_ptr<Dataset>(new Dataset(_schema));
        std::vector<std::string> temp;
        std::string ms, id;
        while(getline(file,line)){ 
            boost::trim(line);
            if(line.empty()){
                break;
            }
            temp = split(line); 
            boost::shared_ptr<Record> pr = createRecord(temp);
            _ds->add(pr); 
        }
    }
boost::shared_ptr<Record> DatasetReader::createRecord(
        const std::vector<std::string>& val) { 
        boost::shared_ptr<Record> rec = boost::shared_ptr<Record>(new Record(_schema));
        std::string label, id;
        Size j = 0;
        Size s;
        for(Size i=0;i<val.size();++i){ 
            if(i == _labelColumn) {
                label = val[i];
                continue;
            }
            if(i == _idColumn) {
                id = val[i];
                continue;
            }
            switch((*_schema)[j]->type()){
                case Continuous:
                    if(val[i]==""){
                        (*_schema)[j]->set_c_val((*rec)[j], 0.0);
                    }else{
                        (*_schema)[j]->set_c_val((*rec)[j], 
                            boost::lexical_cast<Real>(val[i]));
                    }
                    break;
                case Discrete:
                    s = (*_schema)[j]->cast_to_d().add_value(
                        val[i]);
                    (*_schema)[j]->set_d_val((*rec)[j], s);
                    break;
            }
            ++j;
        }
        if(_labelColumn != Null<Size>()) {
            _schema->set_label(rec, label);
        } 

        if(_idColumn != Null<Size>()) {
            _schema->set_id(rec, id);
        }

        return rec;
    }

    void DatasetReader::createSchema() {
        size_t ind = _fileName.find_last_of('.');
        std::string schemaFile = 
            _fileName.substr(0,ind+1) + "names"; 

        std::ifstream file;
        std::string line;
        file.open(schemaFile.c_str());
        
        bool bTag = false;
        while(getline(file,line)){ 
            ind = line.find("///:");
            if(ind!=std::string::npos){
                bTag = true;
                break;
            }
        }
        std::vector<std::string> temp;
        _schema = boost::shared_ptr<Schema>(new Schema());
        _schema->idInfo() = boost::shared_ptr<DAttrInfo>(
            new DAttrInfo("Identifier"));
        Size nLine = 0;
        bool bClass = false;
        bool bId = false;
        while(getline(file,line)){ 
            boost::trim(line);
            if(line.empty()){
                break;
            }
            temp = split(line);
            if(temp[1]=="Class"){
                if (!bClass) {
                    _schema->labelInfo() = 
                        boost::shared_ptr<DAttrInfo>(
                        new DAttrInfo("Membership"));
                    bClass = true;
                    _labelColumn = nLine;
                } 
            } else if (temp[1] == "Continuous") {
                _schema->add(boost::shared_ptr<CAttrInfo>(
                    new CAttrInfo(temp[0])));
            } else if (temp[1] == "Discrete") {
                _schema->add(boost::shared_ptr<DAttrInfo>(
                    new DAttrInfo(temp[0])));
            } else if (temp[1] == "RecordID") {
                if (!bId) {
                    bId = true;
                    _idColumn = nLine;
                } 
            } 
            ++nLine;
        }

        _numColumn = nLine;
        file.close();
    }

    std::vector<std::string> DatasetReader::split(
        const std::string& str) {
        std::vector<std::string> ret;
        boost::char_separator<char> sep(",", "", 
            boost::keep_empty_tokens);
        boost::tokenizer<boost::char_separator<char> > 
            tokens(str, sep);
        for(boost::tokenizer<boost::char_separator<char> 
            >::iterator it = tokens.begin(); 
            it != tokens.end(); ++it) {
            std::string temp = *it;
            boost::trim(temp);
            ret.push_back(temp);
        } 

        return ret; 
    }    
#endif