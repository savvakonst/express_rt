#include <cerrno>
#include <fstream>
#include <memory>
#include <string>
//
#include "KsdIO.h"

std::string getFileContents(const std::string &filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw(errno);
}

KsdIO::KsdIO() : IO_ifs("*.ksd", "Ksd files", "") {}
KsdIO::~KsdIO() {}

bool KsdIO::readDocument(ExtensionManager *manager, const std::string &source_path) {
    std::ifstream in(source_path, std::ios::in | std::ios::binary);

    struct {
        uint32_t data_offset_;
        uint32_t task_offset_;
    } head;

    if (in) {
        in.read((char *)&head, sizeof(head));

        in.seekg(head.task_offset_, std::ios::end);
        auto task_size = head.data_offset_ - head.task_offset_;

        std::unique_ptr<char[]> u_buffer(new char[task_size]);
        in.read(u_buffer.get(), task_size);


    }
    return false;
}
bool KsdIO::saveDocument(const std::string &id, const std::string &dst_path) { return false; }
