
# ifndef EXRT_KSDMMODULEA08__H
# define EXRT_KSDMMODULEA08__H


# include <map>
# include <cstdint>
# include "KSDModule_ifs.h"
# include "device/ModuleStream_ifs.h"


class Module_A08_ : public KSDModule_ifs {

   protected:
# pragma pack(1)
    
	struct CHANNEL_A_08 {
		 uint8_t frequency;
		 uint8_t reserved[2];
		 uint8_t flags;
		 int32_t range;
		 uint32_t ureserved[2];};

	struct Task {
		 MODULE_HEADER header;
		 CHANNEL_A_08 cnl[9];
		 uint8_t reserved[112];};

# pragma pop()

    Task task_;
    TaskMapper field_map_;
   public:

    Module_A08_():field_map_(TaskMapper({
{"header",u32 },
{"cnl",{9,TaskMapper({
{"frequency",u8 },
{"reserved",{2,u8} },
{"flags",u8 },
{"range",i32 },
{"ureserved",{2,u32} } })} },
{"reserved",{112,u8} } })){
        field_map_.setReferencePtr(&task_);
    }

    Module_A08_(void * ptr, size_t size):Module_A08_(){
        if (size!=getTaskSize()){
            // print error or throw error
        }
        task_ = *((Task*)ptr);
    }


    ~Module_A08_(){ }

    std::string getID() const override { return "A08_"; }


    const InfoList& getPropertiesInfoList() override;

    const ResValue_ifs* getProperty(const std::string& prop_path) const override;
    const std::string getPropertyAsTxt(const std::string& prop_path) const override;

    bool setProperty(const std::string& prop_path, const Value value) override;
    bool setPropertyAsTxt(const std::string& prop_path, const std::string& valie) override;


    const void* getTaskPtr() const override { return (const void*)&task_; }
    size_t getTaskSize() const override { return sizeof(Task); }


};

# endif
