#include "resmgr.hpp"

ResourceMgr::resource::resource()
{
	type = RESMGR_RESTYPE_BITMAP;
	res = NULL;
	loaded = false;
	last_used = -311.0;
	filename = NULL;
}

ResourceMgr::ResourceMgr()
{
	ReleaseTime = RESMGR_DEFAULT_RELEASE_TIME;
	NoReleaseThreshold = RESMGR_DEFAULT_NORELEASE_THRESHOLD;
	NullBmp = NULL;
	NullSmp = NULL;
	GameFont = NULL;
	resources = new resource[RESMGR_MAX_RESOURCES];
	memset(resources, 0, RESMGR_MAX_RESOURCES*sizeof(resource*));
}
ResourceMgr::~ResourceMgr()
{
	//ReleaseAllResources();
}
void ResourceMgr::SetReleaseTime(double milliseconds){
	ReleaseTime = milliseconds/1000.0;
}
void ResourceMgr::SetReleaseTime(uint32_t milliseconds){
	ReleaseTime = (double)(milliseconds)/1000.0;
}
void ResourceMgr::SetNoReleaseThreshold(size_t bytes){
	NoReleaseThreshold = bytes;
}
bool ResourceMgr::ReloadBitmap(const char* filename){
	return this->LoadBitmap(filename);
}
bool ResourceMgr::ReloadSample(const char* filename){
	return this->LoadSample(filename);
}

ResourceMgr::resource* ResourceMgr::FindResource(const char* filename)
{
	for(int i=0; i<RESMGR_MAX_RESOURCES&&resources[i].filename; i++)
	{
		if(strcmp(filename, resources[i].filename) == 0)
		{
			return &resources[i];
		}
	}
	return NULL;
}

bool ResourceMgr::LoadBitmap(const char* filename)
{
	printf("loading %s\n", filename);
	ALLEGRO_BITMAP* ptr = al_load_bitmap(filename);
	if(!ptr)
	{
		return false;
	}
	for(int i=0; i<RESMGR_MAX_RESOURCES; i++)
	{
		if(resources[i].filename == NULL)
		{
			char* nName = new char[strlen(filename)+1];
			strcpy(nName, filename);
			resource n;
			n.type = RESMGR_RESTYPE_BITMAP;
			n.res = ptr;
			n.loaded = true;
			n.last_used = al_get_time();
			n.filename = nName;

			resources[i] = n;
			return true;
		}
		if(strcmp(filename, resources[i].filename) == 0)
		{
			if(!resources[i].loaded)
			{
				resources[i].res = ptr;
				resources[i].loaded = true;
				resources[i].last_used = al_get_time();
			}
			else
			{
				al_destroy_bitmap(ptr);
			}
			return true;
		}
	}
	return false;
}

bool ResourceMgr::LoadSample(const char* filename)
{

	printf("loading %s\n", filename);
	ALLEGRO_SAMPLE* ptr = al_load_sample(filename);
	if(!ptr)
	{
		return false;
	}
	for(int i=0; i<RESMGR_MAX_RESOURCES; i++)
	{
		if(resources[i].filename == NULL)
		{
			char* nName = new char[strlen(filename)];
			strcpy(nName, filename);

			resource n;
			n.type = RESMGR_RESTYPE_BITMAP;
			n.res = ptr;
			n.loaded = true;
			n.last_used = al_get_time();
			n.filename = nName;

			resources[i] = n;
			return true;
		}
		if(strcmp(filename, resources[i].filename) == 0)
		{
			if(!resources[i].loaded)
			{
				resources[i].res = ptr;
				resources[i].loaded = true;
				resources[i].last_used = al_get_time();
			}
			else
			{
				al_destroy_sample(ptr);
			}
			return true;
		}
	}
	return false;
}

ALLEGRO_BITMAP* ResourceMgr::GetBitmap(const char* filename)
{
	resource* r = FindResource(filename);
	if(!r)
	{
		if(!this->LoadBitmap(filename))
		{
			return NullBmp;
		}
		return (ALLEGRO_BITMAP*)(FindResource(filename)->res);
	}
	else
	{
		if(r->type == RESMGR_RESTYPE_SAMPLE)
		{
			return NullBmp;
		}
		else if(!r->loaded)
		{
			if(!this->LoadBitmap(r->filename))
			{
				return NullBmp;
			}
			r->last_used = al_get_time();
			return (ALLEGRO_BITMAP*)r->res;
		}
		else
		{
			r->last_used = al_get_time();
			return (ALLEGRO_BITMAP*)r->res;
		}
	}
	return NullBmp;
}

ALLEGRO_SAMPLE* ResourceMgr::GetSample(const char* filename)
{
	resource* r = FindResource(filename);
	if(!r)
	{
		if(!this->LoadSample(filename))
		{
			return NullSmp;
		}
		return (ALLEGRO_SAMPLE*)(FindResource(filename)->res);
	}
	else
	{
		if(r->type == RESMGR_RESTYPE_BITMAP)
		{
			return NullSmp;
		}
		else if(!r->loaded)
		{
			if(!this->LoadSample(r->filename))
			{
				return NullSmp;
			}
			r->last_used = al_get_time();
			return (ALLEGRO_SAMPLE*)r->res;
		}
		else
		{
			r->last_used = al_get_time();
			return (ALLEGRO_SAMPLE*)r->res;
		}
	}
	return NullSmp;
}

int ResourceMgr::DetectExtension(const char* filename)
{
	char extension[5];
	memset(extension, 0, 5);
	char* nIterator = (char*)filename;
	while(*nIterator != '.' && *nIterator)
		nIterator++;					//stops at . or null terminator
	if(!*nIterator)
	{
		return RESMGR_RESTYPE_BITMAP;
	}
	for(int i=0; i<3; i++)
	{
		extension[i] = *(nIterator++);
	}
	if (strcmp(extension, "bmp") == 0 ||
		strcmp(extension, "png") == 0 ||
		strcmp(extension, "jpg") == 0 ||
		strcmp(extension, "pcx") == 0 ||
		strcmp(extension, "tga") == 0)
	{
		return RESMGR_RESTYPE_BITMAP;
	}
	if (strcmp(extension, "wav") == 0 ||
		strcmp(extension, "ogg") == 0 ||
		strcmp(extension, "mod") == 0 ||
		strcmp(extension, "s3m") == 0 ||
		strcmp(extension, "flac") == 0 ||
		strcmp(extension, "xm") == 0)
	{
		return RESMGR_RESTYPE_SAMPLE;
	}
	return RESMGR_RESTYPE_BITMAP;
}

void ResourceMgr::ReleaseUnusedResources()
{
	for(int i=0; i<RESMGR_MAX_RESOURCES&&resources[i].filename; i++)
	{
		resource* r = &resources[i];

		//resource size check
		int rSize = 0x7FFFFFFF;
		if(r->type == RESMGR_RESTYPE_BITMAP && r->loaded)
		{
			ALLEGRO_BITMAP* rr = (ALLEGRO_BITMAP*)r->res;
			int rw = al_get_bitmap_width(rr);
			int rh = al_get_bitmap_height(rr);
			int rd = al_get_pixel_size(al_get_bitmap_format(rr));
			rSize = rw*rh*rd;
		}
		if(r->type == RESMGR_RESTYPE_SAMPLE && r->loaded)
		{
			ALLEGRO_SAMPLE* rr = (ALLEGRO_SAMPLE*)r->res;
			int rl = al_get_sample_length(rr);
			int rd = al_get_audio_depth_size(al_get_sample_depth(rr));
			rSize = rl*rd;
		}
		if(rSize < NoReleaseThreshold)
		{
			continue;
		}

		//release
		if(r->last_used < al_get_time()-ReleaseTime)
		{
			if(r->type == RESMGR_RESTYPE_BITMAP)
			{
				al_destroy_bitmap((ALLEGRO_BITMAP*)r->res);
			}
			else if(r->type == RESMGR_RESTYPE_SAMPLE)
			{
				al_destroy_sample((ALLEGRO_SAMPLE*)r->res);
			}
			r->res = NULL;
			r->loaded = false;
		}
	}
}

void ResourceMgr::ReleaseAllResources()
{
	for(int i=0; i<RESMGR_MAX_RESOURCES&&resources[i].filename; i++)
	{
		if(true)
		{
			resource* r = &resources[i];
			if(r->type == RESMGR_RESTYPE_BITMAP)
			{
				al_destroy_bitmap((ALLEGRO_BITMAP*)r->res);
			}
			else if(r->type == RESMGR_RESTYPE_SAMPLE)
			{
				al_destroy_sample((ALLEGRO_SAMPLE*)r->res);
			}
			r->res = NULL;
			r->loaded = false;
		}
	}
}

void ResourceMgr::ReloadAllResources()
{
	for(int i=0; i<RESMGR_MAX_RESOURCES&&resources[i].filename; i++)
	{
		resource* r = &resources[i];
		if(r->type == RESMGR_RESTYPE_BITMAP)
		{
			this->LoadBitmap(r->filename);
		}
		else if(r->type == RESMGR_RESTYPE_SAMPLE)
		{
			this->LoadSample(r->filename);
		}
	}
}

ResourceMgr MainRM;
