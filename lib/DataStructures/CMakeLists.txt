


add_library(Ati
        ImplicitKeyThreadedATI.c
        ImplicitKeyThreadedATI.h
        ThreadedATI.h
        ThreadedATI.c
        )

target_include_directories(Ati PUBLIC .)