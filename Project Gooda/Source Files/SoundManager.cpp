#include "SoundManager.h"

SoundManager::SoundManager()
{
	m_studioSystem = nullptr;
	ErrorCheck(FMOD::Studio::System::create(&m_studioSystem));
	ErrorCheck(m_studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));
	m_system = nullptr;
	ErrorCheck(m_studioSystem->getLowLevelSystem(&m_system));
}

SoundManager::~SoundManager()
{
	ErrorCheck(m_studioSystem->unloadAll());
	ErrorCheck(m_studioSystem->release());
}

void SoundManager::Frame()
{
	std::vector<ChannelMap::iterator> stoppedChannels;

	for (auto it = m_Channels.begin(), itEnd = m_Channels.end(); it != itEnd; ++it)
	{
		bool isPlaying = false;
		it->second->isPlaying(&isPlaying);
		if (!isPlaying)
			stoppedChannels.push_back(it);
	}

	for (auto it : stoppedChannels)
		m_Channels.erase(it);

	ErrorCheck(m_studioSystem->update());
}

void SoundManager::Load(std::string soundName, bool begin3D, bool beginLoop, bool beginStream)
{
	auto foundIt = m_Sounds.find(soundName);

	if (foundIt != m_Sounds.end())
		return;

	FMOD_MODE mode = FMOD_DEFAULT;
	mode |= begin3D ? FMOD_3D : FMOD_2D;
	mode |= beginLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= beginStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* sound = nullptr;
	ErrorCheck(m_system->createSound(soundName.c_str(), mode, NULL, &sound));
	if (sound)
		m_Sounds[soundName] = sound;
}

void SoundManager::Unload(std::string soundName)
{
	auto foundIt = m_Sounds.find(soundName);
	if (foundIt == m_Sounds.end())
		return;

	ErrorCheck(foundIt->second->release());
	m_Sounds.erase(foundIt);
}

int SoundManager::Play(std::string soundName, XMFLOAT3 position, float volumedB)
{
	int channelId = m_nextChannelId++;
	auto foundIt = m_Sounds.find(soundName);

	FMOD::Channel* channel = nullptr;
	ErrorCheck(m_system->playSound(foundIt->second, NULL, true, &channel));
	if (channel)
	{
		FMOD_MODE currentMode;
		foundIt->second->getMode(&currentMode);

		if (currentMode & FMOD_3D)
		{
			FMOD_VECTOR fposition = VectorToFmod(position);
			ErrorCheck(channel->set3DAttributes(&fposition, NULL));
		}

		ErrorCheck(channel->setVolume(dbToVolume(volumedB)));
		ErrorCheck(channel->setPaused(false));
		m_Channels[channelId] = channel;
	}
	
	return channelId;
}

void SoundManager::Set3dListenerAndOrientation(XMFLOAT3 position, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	m_system->set3DListenerAttributes(0, &VectorToFmod(position), NULL, &VectorToFmod(lookAt), &VectorToFmod(up));
}

void SoundManager::SetChannel3dPosition(int channelId, XMFLOAT3 position)
{
	auto foundIt = m_Channels.find(channelId);

	if (foundIt == m_Channels.end())
		return;

	FMOD_VECTOR fposition = VectorToFmod(position);
	ErrorCheck(foundIt->second->set3DAttributes(&fposition, NULL));
}

void SoundManager::SetChannelVolume(int channelId, float volumedB)
{
	auto foundIt = m_Channels.find(channelId);

	if (foundIt == m_Channels.end())
		return;

	ErrorCheck(foundIt->second->setVolume(dbToVolume(volumedB)));
}

float SoundManager::dbToVolume(float db)
{
	return powf(10.0f, 0.05f * db);
}

float SoundManager::VolumeTodB(float volume)
{
	return 20.0f * log10f(volume);
}

FMOD_VECTOR SoundManager::VectorToFmod(XMFLOAT3 position)
{
	FMOD_VECTOR fVec;
	fVec.x = position.x;
	fVec.y = position.y;
	fVec.z = position.z;

	return fVec;
}

int SoundManager::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD ERROR" << result << std::endl;
		return 1;
	}

	return 0;
}
