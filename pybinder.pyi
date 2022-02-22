from typing import List, Tuple

from typing import overload

class DataSchema:
    default_value_: Value
    description_: str
    enums_: List[Tuple[str,Value]]
    help_: str
    name_: str
    def __init__(self, arg0: str, arg1: str) -> None: ...
    def getDims(self) -> List[int]: ...
    def getMapList(self) -> List[DataSchema]: ...
    def getRepresentationType(self) -> str: ...
    def getType(self) -> str: ...
    def init(self, arg0: ExtensionManager) -> None: ...
    def isArray(self) -> bool: ...
    def isMap(self) -> bool: ...
    def isValid(self) -> bool: ...
    def validate(self, arg0: HierarchicalData_ifs) -> bool: ...
    def __del__(self) -> None: ...

class DeviceViewWrapper:
    def __init__(self) -> None: ...
    def addAction(self, arg0: ExrtAction_ifs) -> None: ...
    def addSignal(self, arg0: Signal_ifs) -> status: ...
    def addToSelected(self, arg0: str, arg1: str) -> bool: ...
    def getActiveDevice(self) -> Device: ...
    def getActiveDeviceSource(self) -> str: ...
    def getActiveModule(self) -> Module_ifs: ...
    def getActiveModulePath(self) -> Tuple[str,str]: ...
    def getSelected(self) -> List[Module_ifs]: ...
    def getSelectedPath(self) -> List[Tuple[str,str]]: ...
    def removeFromActive(self) -> bool: ...
    def removeFromSelected(self, arg0: str, arg1: str) -> bool: ...
    def setActive(self, arg0: str, arg1: str) -> bool: ...

class ExrtAction:
    def __init__(self) -> None: ...
    def getDescription(self) -> str: ...
    def getInfo(self) -> str: ...
    def run(self) -> bool: ...

class ExtensionManager:
    def __init__(self) -> None: ...
    def getAvailableExtensionTypes(self, arg0: str) -> List[str]: ...
    def getExtensionUnitSet(self, arg0: str, arg1: str) -> Set[ExtensionUnit]: ...
    def getLastVersionExtensionUnit(self, arg0: str, arg1: str) -> ExtensionUnit: ...
    def getLastVersionExtensionUnitsByType(self, arg0: str) -> List[ExtensionUnit]: ...

class ExtensionUnit:
    def __init__(self, *args, **kwargs) -> None: ...

class HierarchicalData:
    def __init__(self) -> None: ...
    def getArray(self) -> List[HierarchicalData]: ...
    def getArrayUnit(self, arg0: int) -> HierarchicalData: ...
    def getMap(self) -> List[Tuple[str,HierarchicalData]]: ...
    def getMapUnit(self, arg0: str) -> HierarchicalData: ...
    def getValue(self) -> Value: ...
    def isArray(self) -> bool: ...
    def isMap(self) -> bool: ...
    def isValue(self) -> bool: ...

class Value:
    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, arg0: Value) -> None: ...
    @overload
    def __init__(self, arg0: bool) -> None: ...
    @overload
    def __init__(self, arg0: float) -> None: ...
    @overload
    def __init__(self, arg0: int) -> None: ...
    @overload
    def __init__(self, arg0: str) -> None: ...
    @overload
    def __init__(self, arg0: str, arg1: str) -> None: ...
    def asString(self) -> str: ...
    def __bool__(self) -> bool: ...

class pyIO:
    def __init__(self, arg0: str, arg1: str, arg2: str) -> None: ...
    def readDocument(self, arg0: ExtensionManager, arg1: str) -> bool: ...
    def saveDocument(self, arg0: ExtensionManager, arg1: str, arg2: str) -> bool: ...
    @property
    def file_info_(self) -> str: ...
    @property
    def file_type_(self) -> str: ...
    @property
    def filename_pattern_(self) -> str: ...

def getDataSchema(arg0: ExtensionManager, arg1: str) -> DataSchema: ...
def getDeviceViewWrapper(arg0: ExtensionManager) -> DeviceViewWrapper: ...
def getExrtAction(arg0: ExtensionManager, arg1: str) -> ExrtAction: ...
def getPyIO(arg0: ExtensionManager, arg1: str) -> pyIO: ...
def isNum(arg0: str) -> bool: ...
def normalizeType(arg0: str) -> str: ...
def printer_debug(arg0: DataSchema) -> str: ...