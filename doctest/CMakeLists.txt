add_executable(Doctest_run ValueTest.cpp PseudoSyncPrmBufferTest.cpp)


target_link_libraries(Doctest_run PUBLIC CommonLib Reader)