#include <clocale>
#include <fstream>
#include <iostream>
#include <regex>

#include "SpecificPParserBuilder.h"
#include "baseio/DefaultBaseIO.h"
#include "yaml-cpp/yaml.h"

YAML::Node get2(std::string key, const YAML::Node& node) {
    auto res = node[key];
    if (!res) throw YAML::Exception(node.Mark(), "cant find field with key: \"" + key + "\"");
    return node;
}
/*
int main() {
    try {
        YAML::Node config = YAML::LoadFile("test.base");
        std::cout << "file reding successful\n\n";



        for (const auto& i : config) {
            // std::cout << "\t  enum: " << enum_v.size() << "\n";
            if (i.second.IsScalar()) {
                std::cout << i.first.Scalar() << " : " << i.second.Scalar() << "\n";
            }
        }

        get2("Base. Namea", config).as<std::string>();

    } catch (YAML::Exception e) {
        std::cout << "\n" << e.msg << "\n\t";  //<< e.mark;
    }

    return 0;
}
 */

typedef std::unique_ptr<PDefaultBaseIO_ifs> unique_pb_t;

int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8");

    std::fstream in("test.base", std::ios_base::in | std::ios::binary);
    if (!in) return 1;

    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    DefaultBaseIO base_io;

    std::vector<std::unique_ptr<PDefaultBaseIO_ifs>> vec;
    vec.push_back(unique_pb_t(new AnalogParserBuilder));
    vec.push_back(unique_pb_t(new AnalogVibroParserBuilder));
    vec.push_back(unique_pb_t(new AnalogVoltageParserBuilder));
    vec.push_back(unique_pb_t(new EthernetUdParserBuilder));

    for (auto& i : vec) base_io.addPPBM(i.get());

    base_io.parseDocument(contents);
    std::cout << base_io.getErrorMessage();
    return 0;
}
/*

" File.About": str,
" File.Company": str,
" File.Date": str "year.month.day",
" File.Version": str "\d+.\d+.\d+.\d+",
"Base. Name": str,
"Base.Alteration.List": list
    "Date": str "year.month.day",
    "EditedBy": str,
    "Time": str "dd:dd:dd"

"Base.Alteration.Number": depend_from "Base.Alteration.List",
"Base.CreatedBy": str,
"Base.Date": str "year.month.day",
"Base.Description": str,
"Base.Object": str,
"Base.Time": str "dd:dd:dd",
"Device.ID": uint32_t,

 */