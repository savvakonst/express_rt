# -*- coding: utf-8 -*-
import re
import ctypes


def struct2string(cbw):
    sizeof_cbw = ctypes.sizeof(cbw)
    return ctypes.string_at(ctypes.addressof(cbw), sizeof_cbw)


file = open("m.txt", "r", encoding='UTF-8')
s = file.read()
file.close()
del(file)


c_exp = re.compile('#[^#]*')

c_exp_fields = re.compile(
    r"([_\w]+)[ \t]+([_\w]+)((\[([0-9]+)\])*)(\s([^\n]*))?")
c_exp_dims = re.compile(r"\[([0-9]*)\]")

pattern_list = c_exp.findall(s)


define_txt = """
# ifndef EXRT_KSDMMODULE{0:s}_H
# define EXRT_KSDMMODULE{0:s}_H


# include <map>
# include <cstdint>
# include "KSDModule_ifs.h"
# include "device/ModuleStream_ifs.h"

"""


template = """
class Module_{0:s} : public KSDModule_ifs {{

   protected:
# pragma pack(1)
    {2:s}
# pragma pop()

    {1:s} task_;
    TaskMapper field_map_;
   public:

    Module_{0:s}():field_map_({3:s}){{
        field_map_.setReferencePtr(&task_);
    }}

    Module_{0:s}(void * ptr, size_t size):Module_{0:s}(){{
        if (size!=getTaskSize()){{
            // print error or throw error
        }}
        task_ = *(({1:s}*)ptr);
    }}


    ~Module_{0:s}(){{ }}

    std::string getID() const override {{ return "{0:s}"; }}


    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;


    const void* getTaskPtr() const override {{ return (const void*)&task_; }}
    size_t getTaskSize() const override {{ return sizeof({1:s}); }}


}};

# endif
"""


file = None

sizes = [8*2**x for x in range(4)]
uint_types = {"uint{0:d}_t".format(i): "u{0:d}".format(i) for i in sizes}
int_types = {"int{0:d}_t".format(i): "i{0:d}".format(i) for i in sizes}

type_mapper = {**uint_types, **int_types}

other_structs = ""
struct_name = ""


class Field:
    def __init__(self, name, dims=[]) -> None:
        self.name = name
        self.dims = dims

    def __str__(self) -> str:
        if len(self.dims):
            return self.name + "".join(["[{0:d}]".format(i) for i in self.dims])
        else:
            return self.name

    def __repr__(self) -> str:
        if len(self.dims):
            return self.name + "".join(["[{0:d}]".format(i) for i in self.dims])
        else:
            return self.name


complex_types = {
    "MODULE_HEADER": "i32"
}


for i in pattern_list:
    id = None
    m = re.search(r'#(0x\w*)', i)

    if m:
        id = struct2string(ctypes.c_uint32(
            int(m.group(1), base=0))).decode("utf-8")

        if file:
            file.close()
        fpath = "Module"+id+".cpp"
        file = open(fpath, "w")

        struct_name = "Task"
    else:
        struct_name = re.search('#([\w]*)', i).group(1)

    x = c_exp_fields.findall(i)

    fields_list = []
    new_complex = []
    for i in x:

        f_type, f_name, arr_dim, f_descr = (i[0], i[1], i[2], i[6])
        dim_vals = [int(i) for i in c_exp_dims.findall(arr_dim)]

        new_complex.append((Field(
            f_name, dim_vals), type_mapper[f_type] if f_type in type_mapper else complex_types[f_type]))

        dims = "".join(["[{0:d}]".format(i) for i in dim_vals])
        f = " {0:s} {1:s}{2:s}".format(f_type, f_name, dims)

        fields_list.append(f)

    complex_types[struct_name] = new_complex

    fields = "{\n\t\t" + ";\n\t\t".join(fields_list) + ";};"+"\n"
    other_structs += "\n\tstruct {0:s} {1:s}".format(struct_name, fields)

    if id:

        TaskMapper = ""

        print("-----------")
        if new_complex is None:
            break

        def genArr(type_, dims=[]):
            if(len(dims)):
                dim = dims.pop()
                return '{{{0:d},{1:s}}}'.format(dim, genArr(type_, dims))
            else:
                return type_

        def genMap(struct):
            fields = []
            for key, type_ in struct:

                if not(type(type_) is str):
                    type_ = genMap(type_)
                s = '{{"{0:s}",{1:s} }}'.format(
                    key.name, genArr(type_, key.dims))
                fields.append(s)
            return "TaskMapper({{\n{0:s} }})".format(",\n".join(fields))

        s = genMap(new_complex)
        print(s)

        file.write(
            define_txt.format(id) +
            template.format(id, "Task", other_structs, s))

        file.close()
        file = None

        # print(new_complex)
        print()
        complex_types = {"MODULE_HEADER": "u32"}

        other_structs = ""
