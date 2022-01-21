

#include "convtemplate/ParameterFieldTree.h"


ParameterFieldTree::StructType getStructType(DataSchema_ifs *data_schema){
    if(data_schema->isArray())
        return ParameterFieldTree::StructType::array;
    if(data_schema->isMap())
        return ParameterFieldTree::StructType::map;
    return ParameterFieldTree::StructType::value;
}




ParameterFieldTree::ParameterFieldTree(DataSchema_ifs *data_schema, size_t dim ):struct_type_(getStructType(data_schema)) {

    switch (struct_type_) {
    case map:{
        auto map_list = data_schema->getMapList();
        vecmap_.reserve(map_list.size());
        for(auto i:map_list)
            vecmap_.push_back({ i->name_,ParameterFieldTree(i)}  );

        for (auto& i : vecmap_)
            map_[i.first] = &i.second;

        break;
    }
    case value:{
        type_ = createDataType(data_schema->getType());
        value_ = data_schema->default_value_;
        break;
    }
    case array:{
        auto len = data_schema->getDims()[dim];
        vector_.reserve(len);
        for (size_t i = 0; i < len; i++)
            vector_.push_back( ParameterFieldTree(data_schema,dim+1));
        break;
    }
    }
}

ParameterFieldTree::~ParameterFieldTree() {}

std::vector<HierarchicalData_ifs *> ParameterFieldTree::getArray() const {
    std::vector<HierarchicalData_ifs*> ret;

    ret.reserve(vector_.size());
    for (auto& i : vector_) {
        ret.push_back((ParameterFieldTree*)&i);
    }
    return ret;
}

std::map<std::string, HierarchicalData_ifs *> ParameterFieldTree::getMap() const {
    std::map<std::string, HierarchicalData_ifs*> ret;
    for (auto& i : vecmap_) {
        ret[i.first] = (HierarchicalData_ifs*)&i.second;
    }
    return ret;
}

HierarchicalData_ifs* ParameterFieldTree::getArrayUint(size_t id) const {
    if (id >= vector_.size()) return nullptr;
    return (ParameterFieldTree*)&(vector_[id]);
}

HierarchicalData_ifs* ParameterFieldTree::getMapUint(std::string id) const {
    auto map_uint = map_.find(id);
    if (map_uint != map_.end()) return (ParameterFieldTree*)&(map_uint->second);
    return nullptr;
}

bool ParameterFieldTree::setValue(Value data) { return false; }

bool ParameterFieldTree::setValue(std::string data) { return false; }



