#pragma once
#include <Engine/Game/Component/Script/Script.h>

namespace ehw
{
    class Script_Test2 :
        public Script
    {
        REGISTER_CLASS_INFO(Script_Test2);
        SET_INSTANCE_ABLE(Script_Test2);
    public:
        Script_Test2();
        virtual ~Script_Test2();

        virtual void Update() override;
    };
}


