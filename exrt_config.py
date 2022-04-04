from pyexrt import getExtensionManager
from pybinder import getPyIO, getExrtAction, getDeviceViewWrapper, ExrtAction

manager = getExtensionManager()

ksd_io = getPyIO(manager, "ksd_io")
ksd_io.readDocument(manager, "data-003.ksd")
ksd_io.readDocument(manager, "data-004.ksd")

base_io = getPyIO(manager, "base_io")
base_io.readDocument(manager, "analog.base")
base_io.readDocument(manager, "extended_analog.base")

refresh_ethernet_action = getExrtAction(manager, "file/refresh_ethernet_devices")
refresh_ethernet_action.run()

view_wrapper = getDeviceViewWrapper(manager)

view_wrapper.addToSelected("file://data-003.ksd", "")

s = view_wrapper.getSelected()

# s[0].getParentModule()
print()
for i in s:
    mapping = i.getSubModulesFromPath("0/CH04/**/*")
    for j in mapping:
        print(j.getPropertyAsTxt(""))
        print("path:", j.getModulePath(True))

    # print("path", i.getModulePath(True))

del (s)
