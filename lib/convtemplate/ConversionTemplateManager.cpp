//
// Created by SVK on 12.09.2022.
//
#include "convtemplate/ConversionTemplateManager.h"
int keyComparison(void* arg_a, void* arg_b) {
    auto* a = (CTNode*)arg_a;
    auto* b = (CTNode*)arg_b;

    std::string a_str =
        a->tree ? std::string((char*)a->cnv_template) : a->cnv_template->getPropertyAsTxt("common/name");

    std::string b_str =
        b->tree ? std::string((char*)b->cnv_template) : b->cnv_template->getPropertyAsTxt("common/name");

    return a_str < b_str;
}