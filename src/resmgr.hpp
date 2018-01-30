#pragma once

#include "incl_full.hpp"

#define RESMGR_RESTYPE_BITMAP 0
#define RESMGR_RESTYPE_SAMPLE 1

#define RESMGR_DEFAULT_RELEASE_TIME 30.0
#define RESMGR_DEFAULT_NORELEASE_THRESHOLD 262144

#define RESMGR_MAX_RESOURCES 4096
#define RESMGR_MAX_FONTS 64

class ResourceMgr
{
	struct resource
	{
		resource();
		int type; //0=bitmap 1=sample
		void* res;
		bool loaded;
		double last_used;
		char* filename;
	};
	struct sFont
	{
		ALLEGRO_FONT* f;
		bool l;
	};
	struct fontres
	{
		sFont* s;
		uint16_t capacity;
	};

	double ReleaseTime;
	size_t NoReleaseThreshold; //bytes
	resource* resources;
	resource* FindResource(const char* name);
	int DetectExtension(const char* filename);
public:
	ALLEGRO_FONT* GameFont;
	ALLEGRO_BITMAP* NullBmp; //returned by GetBitmap()/GetSample() when the actual resource can't be loaded
	ALLEGRO_SAMPLE* NullSmp;
	ResourceMgr();
	~ResourceMgr();

	void SetReleaseTime(double seconds);
	void SetReleaseTime(uint32_t milliseconds);
	void SetNoReleaseThreshold(size_t bytes);

	bool LoadBitmap(const char* filename);
	bool LoadSample(const char* filename);
	bool ReloadBitmap(const char* filename);
	bool ReloadSample(const char* filename);

	ALLEGRO_BITMAP* GetBitmap(const char* filename);
	ALLEGRO_SAMPLE* GetSample(const char* filename);

	ALLEGRO_FONT* GetFont(const char* filename);

	void ReleaseUnusedResources();
	void ReleaseAllResources();
	void ReloadAllResources();
};

extern ResourceMgr MainRM;