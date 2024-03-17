

//=========================================================
//This Code is Automatically generated by CodeGenerator
//=========================================================


#include "PCH_UserContents.h"

#include "UserContentsInitializer.h"
#include <Base/Engine/Manager/ComponentManager.h>
#include "strKey_Script.h"


#include "Script\Script_Player.h"
#include "Script\Script_CameraMove.h"
#include "Script\Script_Test.h"
#include "Script\Script_Test2.h"

#define CONSTRUCTOR_T(T) ComponentManager::AddComponentConstructor<T>(strKey::script::##T)

namespace ehw
{

	void UserContentsInitializer::InitScript()
	{
		CONSTRUCTOR_T(Script_Player);
		CONSTRUCTOR_T(Script_CameraMove);
		CONSTRUCTOR_T(Script_Test);
		CONSTRUCTOR_T(Script_Test2);
	}
}