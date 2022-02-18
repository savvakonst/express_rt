from pyexrt import getExtensionManager
from pybinder import getPyIO, getExrtAction, getDeviceViewWrapper

manager = getExtensionManager()

ksd_io = getPyIO(manager, "ksd_io")
ksd_io.readDocument(manager, "data-003.ksd")
ksd_io.readDocument(manager, "3-003.ksd")

base_io = getPyIO(manager, "base_io")
base_io.readDocument(manager, "analog.base")

refresh_ethernet_action = getExrtAction(manager, "file/refresh_ethernet_devices")
refresh_ethernet_action.run();

view_wrapper = getDeviceViewWrapper(manager);
view_wrapper.setActive(1)
