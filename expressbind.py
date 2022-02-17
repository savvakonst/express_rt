from pybinder import Value, HierarchicalData, DataSchema, normalizeType, isNum, printer_debug, getDataSchema
from ruamel.yaml import YAML
from ruamel.yaml.comments import CommentedMap, CommentedSeq
import glob
from pyextr import getExtensionManager


# PYTHONUNBUFFERED=1;Path=C:\Express_expr_compiler\nik\pybindings\build\temp.win-amd64-3.8\Release\bin\Release
# C:\Express_expr_compiler\nik\test_pybind\venv\Scripts\pip.exe
# install .
# $ProjectFileDir$

class DataSchemaInst(DataSchema):

    # def __del__(self):
    #    DataSchema.__del__(self)
    #    print("__del__(self)")
    def init(self, manager):
        for i in range(len(self.map_list_)):
            include_name = self.map_list_[i].include_
            if (include_name):
                ds = getDataSchema(getExtensionManager(), include_name)
                if (not ds is None):
                    self.map_list_[i] = ds
                else:
                    print("error: cant find DataSchema which is named as \"" + include_name + "\"")

    def __init__(self, name, description="", map_list=[], ex_type="unknown"):
        DataSchema.__init__(self, name, description)
        self.name_ = name
        self.description_ = description
        self.help_ = ""
        self.dims_ = []
        self.map_list_ = map_list
        self.type_ = normalizeType(ex_type)
        self.representation_ = None
        self.include_ = None

    def getType(self):
        return self.type_

    def getRepresentationType(self):

        if self.representation_:
            return self.representation_

        if len(self.enums_):
            return "enum"
        elif (isNum(self.type_)):
            return "number"
        elif (self.type_ == "bool_v"):
            return "bool"
        elif (self.type_ == "lstr_v"):
            return "large_text"
        elif (self.type_ == "str_v"):
            return "text"
        return "none"

    def isValid(self):
        is_none = self.type_ == "none_v"
        is_map = bool(len(self.map_))
        return is_none == is_map

    def isMap(self):
        return bool(self.map_list_)

    def isArray(self):
        return bool(self.dims_)

    def getDims(self):
        return self.dims_

    def getMapList(self):
        return self.map_list_

    def validate(self):
        return True


def loadConfig():
    def createDataSchemaInst(i: CommentedMap):
        def getItem(name, default):
            return i[name] if name in i else default

        name = i["name"]

        description = getItem("description", name)

        dtype = getItem("type", "none")
        is_set = not type(dtype) is str

        if is_set:
            ds = DataSchemaInst(name, description, map_list=[createDataSchemaInst(j) for j in dtype])
        else:
            ds = DataSchemaInst(name, description, ex_type=dtype)

        ds.help_ = getItem("help", "")
        ds.dims_ = getItem("dims", [])
        ds.representation_ = getItem("representation", None)
        ds.include_ = getItem("include", None)

        if "enum" in i:
            enum = i["enum"]

            if type(enum) is CommentedMap:
                ds.enums_ = [(key, Value(str(val), ds.type_)) for key, val in enum.items()]

            if type(enum) is CommentedSeq:
                ds.enums_ = [(str(val), Value(str(val), ds.type_)) for val in enum]
            # print(ds.enums_)
        return ds

    ret = []
    try:
        filenames = glob.glob('*ds_.yml')
        for filename in filenames:
            # "example_config_ds_.yml"
            f = open(filename, "r", encoding="utf-8")
            config = f.read()
            f.close()

            yaml = YAML()
            code: CommentedMap = yaml.load(config)

            ext_set: CommentedSeq = code["ext_set"];
            for i in ext_set:
                ds = createDataSchemaInst(i)
                print("createDataSchemaInst: ", ds.name_)
                ret.append(ds)

    except Exception as e:
        print(e)

    return ret
