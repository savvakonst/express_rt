
# ifndef EXRT_KSDMMODULEA07__H
# define EXRT_KSDMMODULEA07__H


# include <map>
# include <cstdint>
# include "KSDModule_ifs.h"
# include "device/ModuleStream_ifs.h"


class Module_A07_ : public KSDModule_ifs {

   protected:
# pragma pack(1)
    
	struct CHANNEL_A_07 {
		 uint8_t frequency;
		 uint8_t reserved[2];
		 uint8_t flags;
		 int32_t range;
		 int32_t current;
		 uint32_t ureserved;};

	struct Task {
		 MODULE_HEADER header;
		 CHANNEL_A_07 cnl[8];
		 uint8_t reserved[128];};

# pragma pop()

    Task task_;
    TaskMapper field_map_;
   public:

    Module_A07_():field_map_(TaskMapper({
{"header",u32 },
{"cnl",{8,TaskMapper({
{"frequency",u8 },
{"reserved",{2,u8} },
{"flags",u8 },
{"range",i32 },
{"current",i32 },
{"ureserved",u32 } })} },
{"reserved",{128,u8} } })){
        field_map_.setReferencePtr(&task_);
    }

    Module_A07_(void * ptr, size_t size):Module_A07_(){
        if (size!=getTaskSize()){
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }


    ~Module_A07_(){ }

    std::string getID() const override { return "A07_"; }


    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;


    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }


};

# endif
