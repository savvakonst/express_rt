#include "baseio/DefaultBaseIO.h"

#include <memory>
#include <regex>
#include <vector>

#include "HDYamlWrapper.h"
#include "convtemplate/ConversionTemplate.h"
#include "extensions/PDefaultBaseIO_ifs.h"

template <typename Ta, typename Tb>
std::stringstream &operator<<(std::stringstream &out, const std::pair<Ta, Tb> &v) {
    out << v.first << ": " << v.second;
    return out;
}

template <typename T>
std::stringstream &operator<<(std::stringstream &out, const std::vector<T> &v) {
    if (!v.empty()) {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    }
    return out;
}

std::stringstream &operator<<(std::stringstream &out, const YAML::Mark &v) {
    out << "line: " << v.line << ", column: " << v.column << ", pos: " << v.pos;
    return out;
}

void saveAsJson(const YAML::Node &node, std::stringstream &s, const std::string &indentation = "");

void saveAsJson(const YAML::Node &node, std::stringstream &s, const std::string &indentation) {
    if (node.IsScalar()) {
        std::string val = node.Scalar();

        static const std::regex new_line("\n");
        static const std::regex other("[ ]*(null|false|true)");
        static const std::regex num_exp("[ ]*(\\+|\\-)?([0-9]+|0x[0-9]+|[0-9]+(.|,)[0-9]*)");

        if (std::regex_match(val, num_exp) || std::regex_match(val, other)) {
            s << "" << val << "";
        } else {
            s << "\"" << std::regex_replace(val, new_line, "\\n") << "\"";
        }
        return;
    }

    const std::string next_indentation = indentation + "    ";

    const char *separator = "";
    if (node.IsMap()) {
        s << "{";
        for (const auto &i : node) {
            s << separator << "\n" << next_indentation << "\"" << i.first.as<std::string>() << "\": ";
            separator = ",";
            saveAsJson(i.second, s, next_indentation);
        }
        s << "\n" << indentation << "}";
    } else if (node.IsSequence()) {
        s << "[";
        for (const auto &i : node) {
            s << separator << "\n" << next_indentation;
            separator = ",";
            saveAsJson(i, s, next_indentation);
        }
        s << "\n" << indentation << "]";
    }
}

std::string getStrID(uint32_t id) {
    uint64_t u64_id = uint64_t(id) & 0xffffffff;
    return std::string((char *)&u64_id);
}

/*
 *
 *
 *
 *
 */

DefaultBaseIO::DefaultBaseIO() {}

DefaultBaseIO::~DefaultBaseIO() {}

YAML::Node findParametersSubInformation(const std::string &parameter_name, const YAML::Node &node) {
    for (auto &i : node) {
        if (parameter_name == i[" Name"].as<std::string>()) return i;
    }
    return YAML::Node();
}

YAML::Node get(std::string key, const YAML::Node &node) {
    auto res = node[key];
    if (!res) throw YAML::Exception(node.Mark(), "cant find field with key: \"" + key + "\"");
    return res;
}

ConversionTemplate *DefaultBaseIO::parseDocument(ExtensionManager *manager, const std::string &str) {
    std::unique_ptr<ConversionTemplate> conv_template(new ConversionTemplate(manager));

    try {
        YAML::Node doc = YAML::Load(str);

        if (!doc.IsMap()) {
            error_message_ = "invalid structure\n";
            return nullptr;
        }

        auto node = get("Parameters.List", doc);

        conv_template->setName(get("Base. Name", doc).as<std::string>());
        conv_template->addInfo("company",Value(get(" File.Company", doc).as<std::string>()));
        conv_template->addInfo("source",Value("*"));




        for (const auto &i : get("Device.Modules.List", doc)) {
            const std::string module_name = getStrID(get("Module.ID", i).as<uint32_t>());
            const std::string module = module_name + ":" + std::to_string(get("Module.Slot", i).as<uint32_t>());

            if (module_name != "CALC")
                if (!conv_template->addModule(module)) {
                    warning_messages_.push_back(conv_template->getErrorMessage());
                    conv_template->clearErrorMessage();
                }
        }

        for (const auto &header : get("Parameters.List", doc)) {
            const auto parameter_type = get("Type", header).as<uint32_t>();
            const auto parameter_name = get(" Name", header).as<std::string>();

            auto list = getPPBMList(parameter_type);
            if (list && ((bool)list->size())) {  // TODO: this section is slow
                const YAML::Node additional_list = doc[list->front()->getTypeIdentifier()];
                const YAML::Node other = findParametersSubInformation(parameter_name, additional_list);

                auto hd_header = HierarchicalDataYamlWrapper(header);
                auto hd_other = HierarchicalDataYamlWrapper(other);

                for (auto i : *list) {
                    Parameter_ifs *prm = i->parse(manager, &hd_other, &hd_header);
                    if (prm) {
                        conv_template->addParameter(prm);
                        break;
                    }
                }

            } else {
                warning_messages_.push_back("unknown parameter type: " + std::to_string(parameter_type));
            }
        }


    } catch (YAML::Exception &e) {
        std::stringstream st;
        st << "\n" << e.msg << "\n\t";
        st << e.mark << "\n";

        error_message_ = st.str();
        return nullptr;
    }

    return conv_template.release();
}

std::string DefaultBaseIO::createDocument(const ConversionTemplate *conv_template) { return std::string(); }

void DefaultBaseIO::addPPBM(PDefaultBaseIO_ifs *p) {
    auto PP = PPBMap_.find(p->getPrmType());
    if (PP == PPBMap_.end()) {
        PPBMap_[p->getPrmType()] = new PPBList{p};
    } else {
        (*PP).second->push_back(p);
    }
}

const DefaultBaseIO::PPBList *DefaultBaseIO::getPPBMList(uint32_t type) const {
    auto PP = PPBMap_.find(type);
    if (PP == PPBMap_.end()) {
        return nullptr;
    }
    return PP->second;
}
