

//=========================================================
//This Code is Automatically generated by CodeGenerator.exe
//=========================================================


#include "PCH_UserContents.h"

#include "ContentsClassInitializer.h"
#include <EngineBase/Engine/SceneMgr.h>
#include "strKey_Scene.h"


#include "Scene_Title.h"

#define CONSTRUCTOR_T(T) SceneMgr::AddSceneConstructor<T>(strKey::Scene::##T)

namespace ehw
{
	void ContentsClassInitializer::InitScene()
	{
		CONSTRUCTOR_T(Scene_Title);
	}
}
