
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <regex>
#include <vector>

#include "common/FieldInfo.h"
#include "yaml-cpp/yaml.h"

YAML::Node get2(std::string key, const YAML::Node& node) {
    auto res = node[key];
    if (!res) throw YAML::Exception(node.Mark(), "cant find field with key: \"" + key + "\"");
    return node;
}

int main() {
    try {
        YAML::Node config = YAML::LoadFile("../test.base");
        std::cout << "file reding successful\n\n";

        for (const auto& i : config) {
            // std::cout << "\t  enum: " << enum_v.size() << "\n";
            if (i.second.IsScalar()) {
                // std::cout << i.first.Scalar() << " : " << i.second.Scalar() << "\n";
            }
        }

        get2("Base. Namea", config).as<std::string>();

    } catch (YAML::Exception e) {
        std::cout << "\n" << e.msg << "\n\t";  //<< e.mark;
    }

    return 0;
}