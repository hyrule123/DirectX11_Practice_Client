#include "EnginePCH.h"

#include "Fmod.h"

#ifdef _DEBUG
#pragma comment(lib, "Fmod/Debug/fmodL_vc.lib")
#pragma comment(lib, "Fmod/Debug/fmodstudioL_vc.lib")
#else
#pragma comment(lib, "Fmod/Release/fmod_vc.lib")
#pragma comment(lib, "Fmod/Release/fmodstudio_vc.lib")
#endif

namespace mh
{
	FMOD::Studio::System* Fmod::mSystem = nullptr;
	FMOD::System* Fmod::mCoreSystem = nullptr;

	void Fmod::Initialize()
	{
		void* extraDriverData = NULL;

		FMOD::Studio::System::create(&mSystem);

		// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
		mSystem->getCoreSystem(&mCoreSystem);
		mCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);
		
		mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDriverData);
	}

	bool Fmod::CreateSound(const std::string_view path,  FMOD::Sound** sound)
	{
		if (FMOD_OK != mCoreSystem->createSound(std::string(path).c_str(), FMOD_3D, 0, sound))
			return false;

		return true;
	}

	void Fmod::SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel)
	{
		mCoreSystem->playSound(sound, 0, false, channel);
	}

	void Fmod::Set3DListenerAttributes(const math::Vector3* _position, const math::Vector3* _velocity,
									   const math::Vector3* _forward, const math::Vector3* _up)
	{
		FMOD_VECTOR fmodPosition(_position->x, _position->y, _position->z);
		FMOD_VECTOR fmodVelocity(_velocity->x, _velocity->y, _velocity->z);
		FMOD_VECTOR fmodForward(_forward->x, _forward->y, _forward->z);
		FMOD_VECTOR fmodUp(_up->x, _up->y, _up->z);

		mCoreSystem->set3DListenerAttributes(0, &fmodPosition, &fmodVelocity, &fmodForward, &fmodUp);
	}

	void Fmod::Release()
	{
		mSystem->release();
		mSystem = nullptr;
	}


}
