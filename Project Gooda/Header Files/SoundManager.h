#pragma once

#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <d3d12.h>
#include <DirectXMath.h>

#include "fmod_studio.hpp"
#include "fmod.hpp"

using namespace DirectX;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	void Frame();
	void Load(std::string, bool, bool, bool);
	void Unload(std::string);

	int Play(std::string, XMFLOAT3, float);

	void Set3dListenerAndOrientation(XMFLOAT3, XMFLOAT3, XMFLOAT3);
	void SetChannel3dPosition(int, XMFLOAT3);
	void SetChannelVolume(int, float);

private:
	float dbToVolume(float);
	float VolumeTodB(float);
	FMOD_VECTOR VectorToFmod(XMFLOAT3);

	int ErrorCheck(FMOD_RESULT);

	FMOD::Studio::System* m_studioSystem;
	FMOD::System* m_system;
	int m_nextChannelId;

	typedef std::map<std::string, FMOD::Sound*> SoundMap;
	typedef std::map<int, FMOD::Channel*> ChannelMap;
	typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
	typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

	BankMap m_Banks;
	EventMap m_Events;
	SoundMap m_Sounds;
	ChannelMap m_Channels;

};
