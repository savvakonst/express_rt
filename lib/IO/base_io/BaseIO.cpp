//
#include <memory>
#include <regex>
#include <vector>

//
#include "BaseIO.h"
#include "common/DataSchema_ifs.h"
#include "common/ExtensionManager.h"
#include "convtemplate/ConversionTemplate.h"
#include "convtemplate/ConversionTemplateManager.h"
#include "extensions/PDefaultBaseIO_ifs.h"
//
#include "HDYamlWrapper.h"

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

YAML::Node get(std::string key, const YAML::Node &node) {
    auto res = node[key];
    if (!res) throw YAML::Exception(node.Mark(), "cant find field with key: \"" + key + "\"");
    return res;
}

template <typename T>
T getVal(std::string key, const YAML::Node &node, T default_v) {
    auto res = node[key];
    if (!res) return default_v;
    return res.as<T>();
}

template <typename T>
T getVal(std::string key, const YAML::Node &node) {
    auto res = node[key];
    if (!res) throw YAML::Exception(node.Mark(), "cant find field with key: \"" + key + "\"");
    return res.as<T>();
}

std::string getStrID(std::string key, const YAML::Node &node) {
    auto id = get(key, node).as<uint32_t>();
    uint64_t u64_id = uint64_t(id) & 0xffffffff;
    return std::string((char *)&u64_id);
}

std::string getPath(const YAML::Node &i, const std::string &prefix = "") {
    auto module_name = getStrID("Module.ID", i);
    if (module_name == "CALC") return "";

    auto slot = getVal<uint32_t>("Module.Slot", i, 0);
    auto sub_slot = getVal<uint32_t>("Module.Subslot", i, 0);

    auto path = std::to_string(slot) + ((sub_slot == 0) ? "" : "." + std::to_string(sub_slot)) + "/" + module_name;
    return path.empty() ? "" : prefix + path;
}

/*
 *
 *
 *
 *
 */

BaseIO::BaseIO() : IO_ifs("*.base", "express base file", "") {}

BaseIO::~BaseIO() = default;

YAML::Node findParametersSubInformation(const std::string &parameter_name, const YAML::Node &node) {
    for (auto &i : node) {
        if (parameter_name == i[" Name"].as<std::string>()) return i;
    }
    return {};
}

bool BaseIO::readDocument(ExtensionManager *manager, const std::string &source_path) {
    auto ctm = (ConversionTemplateManager *)manager
                   ->getLastVersionExtensionUnit("cnv_template_manager", "cnv_template_manager")
                   ->ptr;

    auto conv_template = readOrParseDocument(manager, true, "", source_path);
    if (conv_template) {
        ctm->addConversionTemplate(conv_template);
        return true;
    }
    return false;
}

bool BaseIO::saveDocument(ExtensionManager *manager, const std::string &id, const std::string &dst_path) {
    return false;
}

ConversionTemplate *BaseIO::parseDocument(ExtensionManager *manager, const std::string &str,
                                          const std::string &source_path) {
    return readOrParseDocument(manager, false, str, source_path);
}

ConversionTemplate *BaseIO::readOrParseDocument(ExtensionManager *manager, bool is_file, const std::string &str,
                                                const std::string &source_path) {
    std::unique_ptr<ConversionTemplate> conv_template(new ConversionTemplate(manager));

    try {
        YAML::Node doc = is_file ? YAML::LoadFile(source_path) : YAML::Load(str);

        if (!doc.IsMap()) {
            error_message_ = "invalid structure.\n";
            return nullptr;
        }

        auto node = get("Parameters.List", doc);

        conv_template->setName(get("Base. Name", doc).as<std::string>());
        conv_template->setProperty("company", Value(get(" File.Company", doc).as<std::string>()));
        conv_template->setProperty("source", Value(source_path));

        std::string device_id = getStrID("Device.ID", doc);

        /*
         *
         *
         */
        size_t counter = 0;
        auto setAlteration = [&](const auto &path, const std::string &time, const std::string &edited_by) {
            conv_template->setProperty(path, nullptr);
            conv_template->setProperty(path + "/time", Value(time));
            conv_template->setProperty(path + "/by", Value(edited_by));
        };

        auto edited_by = getVal<std::string>("Base.CreatedBy", doc, "");
        auto time = getVal<std::string>("Base.Date", doc, "") + " " + getVal<std::string>("Base.Time", doc, "");
        auto path = "changes/" + std::to_string(counter);

        setAlteration(path, time, edited_by);
        
        for (const auto &i : get("Base.Alteration.List", doc)) {
            counter++;
            edited_by = getVal<std::string>("EditedBy", i, "");
            time = getVal<std::string>("Date", i, "") + " " + getVal<std::string>("Time", i, "");
            path = "changes/" + std::to_string(counter);

            setAlteration(path, time, edited_by);
        }

        for (const auto &i : get("Device.Modules.List", doc)) {
            auto full_path = getPath(i, device_id + "/0/*/");

            // Base.Alteration.List
            // Base.Date

            if (!full_path.empty())
                if (!conv_template->addModule(full_path)) {
                    warning_messages_.push_back(conv_template->getErrorMessage());
                    conv_template->clearErrorMessage();
                }
        }
        // std::cout << "---------------------------------\n";

        for (const auto &header : get("Parameters.List", doc)) {
            auto parameter_type = get("Type", header).as<uint32_t>();
            auto parameter_name = get(" Name", header).as<std::string>();

            auto list = getPPBMList(parameter_type);
            if (list && (!list->empty())) {  // TODO: this section is slow
                const YAML::Node additional_list = doc[list->front()->getTypeIdentifier()];
                const YAML::Node other = findParametersSubInformation(parameter_name, additional_list);

                auto hd_header = HierarchicalDataYamlWrapper(header);
                auto hd_other = HierarchicalDataYamlWrapper(other);

                auto full_path = getPath(header, device_id + "/0/*/");
                for (auto i : *list) {
                    Parameter_ifs *prm = i->parse(manager, &hd_other, &hd_header, full_path);
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

void BaseIO::addPpbm(PDefaultBaseIO_ifs *p) {
    auto PP = PPBMap_.find(p->getPrmType());
    if (PP == PPBMap_.end()) {
        PPBMap_[p->getPrmType()] = new PPBList{p};
    } else {
        (*PP).second->push_back(p);
    }
}

const BaseIO::PPBList *BaseIO::getPPBMList(uint32_t type) const {
    auto PP = PPBMap_.find(type);
    if (PP == PPBMap_.end()) {
        return nullptr;
    }
    return PP->second;
}
