from pyexrt import getExtensionManager
from pybinder import getPyIO, getExrtAction, getDeviceViewWrapper, ExrtAction

manager = getExtensionManager()

ksd_io = getPyIO(manager, "ksd_io")
ksd_io.readDocument(manager, "data-003.ksd")
ksd_io.readDocument(manager, "data-004.ksd")

base_io = getPyIO(manager, "base_io")
base_io.readDocument(manager, "analog.base")
base_io.readDocument(manager, "extended_analog.base")
base_io.readDocument(manager, "res_analog.base")

refresh_ethernet_action = getExrtAction(manager, "file/refresh_ethernet_devices")
refresh_ethernet_action.run()

view_wrapper = getDeviceViewWrapper(manager)

# view_wrapper.addToSelected("file://data-003.ksd", "")
#
view_wrapper.addToSelected("udp://192.168.000.201:4880", "")
s = view_wrapper.getSelected()

# s[0].getParentModule()
# print(s[0].getPropertyAsTxt(""))
for i in s:
    mapping = i.getSubModulesFromPath("0/CH*/2/A*")
    for j in mapping:
        print(j.getPropertyAsTxt(""))
        print("path:", j.getModulePath(True))

    # print("path", i.getModulePath(True))

del (s)
