//====================================================================================================
// Filename:	Config.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Config.h"

#include <RapidJSON/Inc/document.h>
#include <RapidJSON/Inc/filereadstream.h>

using namespace X;

namespace
{
	Config* sConfig = nullptr;
}

//----------------------------------------------------------------------------------------------------

class Config::Impl
{
public:
	void Load(const char* fileName);

	int GetInt(const char* key, int default = 0) const;
	bool GetBool(const char* key, bool default = false) const;
	float GetFloat(const char* key, float default = 0.0f) const;
	const char* GetString(const char* key, const char* default = "") const;

private:
	rapidjson::Document mDocument;
};

//----------------------------------------------------------------------------------------------------

void Config::Impl::Load(const char* fileName)
{
	if (fileName == nullptr)
	{
		XLOG("[Config] Not using config file. Default settings will be used.");
		return;
	}

	FILE* file = nullptr;
	fopen_s(&file, fileName, "rb");
	if (file == nullptr)
	{
		XLOG("[Config] Failed to open config file. Default settings will be used.");
		return;
	}

	char buffer[65536];
	rapidjson::FileReadStream readStream(file, buffer, std::size(buffer));
	mDocument.ParseStream(readStream);

	fclose(file);
}

//----------------------------------------------------------------------------------------------------

int Config::Impl::GetInt(const char* key, int default) const
{
	if (!mDocument.IsObject())
	{
		return default;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsInt())
	{
		return iter->value.GetInt();
	}
	return default;
}

//----------------------------------------------------------------------------------------------------

bool Config::Impl::GetBool(const char* key, bool default) const
{
	if (!mDocument.IsObject())
	{
		return default;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsBool())
	{
		return iter->value.GetBool();
	}
	return default;
}

//----------------------------------------------------------------------------------------------------

float Config::Impl::GetFloat(const char* key, float default) const
{
	if (!mDocument.IsObject())
	{
		return default;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsFloat())
	{
		return iter->value.GetFloat();
	}
	return default;
}

//----------------------------------------------------------------------------------------------------

const char* Config::Impl::GetString(const char* key, const char* default) const
{
	if (!mDocument.IsObject())
	{
		return default;
	}
	auto iter = mDocument.FindMember(key);
	if (iter != mDocument.MemberEnd() && iter->value.IsString())
	{
		return iter->value.GetString();
	}
	return default;
}

//----------------------------------------------------------------------------------------------------

void Config::StaticInitialize(const char* fileName)
{
	XASSERT(sConfig == nullptr, "[Config] System already initialized!");
	sConfig = new Config();
	sConfig->Load(fileName);
}

//----------------------------------------------------------------------------------------------------

void Config::StaticTerminate()
{
	if (sConfig != nullptr)
	{
		SafeDelete(sConfig);
	}
}

//----------------------------------------------------------------------------------------------------

Config* Config::Get()
{
	XASSERT(sConfig != nullptr, "[Config] No system registered.");
	return sConfig;
}

//----------------------------------------------------------------------------------------------------

Config::Config()
	: mImpl(std::make_unique<Impl>())
{
}

//----------------------------------------------------------------------------------------------------

void Config::Load(const char* fileName)
{
	mImpl->Load(fileName);
}

//----------------------------------------------------------------------------------------------------

int Config::GetInt(const char* key, int default) const
{
	return mImpl->GetInt(key, default);
}

//----------------------------------------------------------------------------------------------------

bool Config::GetBool(const char* key, bool default) const
{
	return mImpl->GetBool(key, default);
}

//----------------------------------------------------------------------------------------------------

float Config::GetFloat(const char* key, float default) const
{
	return mImpl->GetFloat(key, default);
}

//----------------------------------------------------------------------------------------------------

const char* Config::GetString(const char* key, const char* default) const
{
	return mImpl->GetString(key, default);
}