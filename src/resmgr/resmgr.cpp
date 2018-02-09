#include "resmgr.hpp"
#include "../hlpfunc.hpp"

ResourceMgr::resource::resource(int type, void* res, bool loaded, double last_used, char* filename)
{
	this->type = type;
	this->res = res;
	this->loaded = loaded;
	this->last_used = last_used;
	this->filename = filename;
}
ResourceMgr::resource::resource()
{
	resource(RESMGR_RESTYPE_BITMAP, NULL, false, -311.0, NULL);
}
ResourceMgr::resource::~resource()
{
}

ResourceMgr::sFont::sFont()
{
	f = NULL;
	loaded = false;
	last_used = -311.0;
}
ResourceMgr::ResourceMgr()
{
	ReleaseTime = RESMGR_DEFAULT_RELEASE_TIME;
	NoReleaseThreshold = RESMGR_DEFAULT_NORELEASE_THRESHOLD;
	NullBmp = NULL;
	NullSmp = NULL;
	memset(resources, 0, RESMGR_MAX_RESOURCES*sizeof(resource));
	memset(fonts, 0, RESMGR_MAX_FONTS*sizeof(fontres));
	numResources = 0;
	numFonts = 0;
	GenerateNullResources();
}
ResourceMgr::~ResourceMgr()
{
	for(int i=0; i<RESMGR_MAX_RESOURCES; i++)
	{
		if(resources[i].filename)
		{
			free(resources[i].filename);
		}
	}
	for(int i=0; i<RESMGR_MAX_FONTS; i++)
	{
		if(fonts[i].filename)
		{
			free(fonts[i].filename);
		}
	}
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
bool ResourceMgr::ReloadFont(const char* filename, uint32_t fSize){
	return this->LoadFont(filename, fSize);
}

void ResourceMgr::GenerateNullResources()
{
	//32 bits (4 bytes) per line, 32x32 monochrome image
	uint32_t img_buf[32] = {0x00000000, 0x00000000, 0x00000000, 0x00000000,
							0x00000000, 0x00003800, 0x02AB8B80, 0x02ABBA00,
							0x02A8AB80, 0x00FB3980, 0x02000000, 0x00000000,
							0x00000000, 0x00000000, 0x00000000, 0x00000000,
							0x00000000, 0x00000000, 0x00000000, 0x00000000,
							0x00000000, 0x00000000, 0x00000070, 0x155DD510,
							0x154CD570, 0x15511550, 0x1F1DC770, 0x00401000,
							0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint16_t* audio_buf = (uint16_t*)calloc(441, sizeof(uint16_t));

	NullFont = al_create_builtin_font();
	NullBmp = al_create_bitmap(32, 32);
	NullSmp = al_create_sample(audio_buf, 441, 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1, false);

	ALLEGRO_BITMAP* tb = al_get_target_bitmap();
	al_set_target_bitmap(NullBmp);
	al_lock_bitmap(NullBmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	for(int i=0; i<32; i++)
	{
		uint32_t h = 0x80000000;
		uint32_t v = img_buf[31-i];
		for(int j=0; j<32; j++)
		{
			ALLEGRO_COLOR cb = al_map_rgb(4, 25, 81);
			ALLEGRO_COLOR cf = al_map_rgb(255, 233, 127);
			ALLEGRO_COLOR f = (v&(h>>j))?cf:cb;

			al_put_pixel(j, i, f);
		}
	}
	al_unlock_bitmap(NullBmp);
	al_set_target_bitmap(tb);

	//free(audio_buf);
}

int qsResCompare(const void* a, const void* b)
{
	ResourceMgr::resource* va = (ResourceMgr::resource*)a;
	ResourceMgr::resource* vb = (ResourceMgr::resource*)b;
	return strcmp_c(vb->filename, va->filename);
}
int qsFontCompare(const void* a, const void* b)
{
	ResourceMgr::fontres* va = (ResourceMgr::fontres*)a;
	ResourceMgr::fontres* vb = (ResourceMgr::fontres*)b;
	return strcmp_c(vb->filename, va->filename);
}

ResourceMgr::fontres* ResourceMgr::FindFont(const char* name)
{
	if(numFonts<1)
		return NULL;
	int l = 0;
	int h = numFonts-1;
	int i = 0;
	while(l<=h)
	{
		i = l + (h-l)/2;
		int c = strcmp_c(fonts[i].filename, name);
		if(!c)
			return &fonts[i];
		if(c<0)
			h = i-1;
		else
			l = i+1;
	}

	return NULL;
}

ResourceMgr::resource* ResourceMgr::FindResource(const char* name)
{
	int l = 0;
	int h = numResources-1;
	int i = 0;
	while(l<=h)
	{
		i = l + (h-l)/2;
		int c = strcmp_c(resources[i].filename, name);
		if(!c)
			return &resources[i];
		if(c<0)
			h = i-1;
		else
			l = i+1;
	}

	return NULL;
}

bool ResourceMgr::LoadBitmap(const char* filename)
{
	ALLEGRO_BITMAP* ptr = al_load_bitmap(filename);
	if(!ptr)
	{
		return false;
	}

	resource* r = FindResource(filename);
	if(!r)
	{
		resources[numResources++] = resource(RESMGR_RESTYPE_BITMAP, ptr, true, al_get_time(), (char*)malloc(strlen(filename)+1));
		strcpy(resources[numResources-1].filename, filename);
		qsort(resources, numResources, sizeof(resource), qsResCompare);
		return true;
	}
	else
	{
		if(!r->loaded)
		{
			r->res = ptr;
			r->loaded = true;
			r->last_used = al_get_time();
		}
		else
		{
			al_destroy_bitmap(ptr);
		}
		return true;
	}

	return false;
}

bool ResourceMgr::LoadSample(const char* filename)
{
	ALLEGRO_SAMPLE* ptr = al_load_sample(filename);
	if(!ptr)
	{
		return false;
	}

	resource* r = FindResource(filename);
	if(!r)
	{
		resources[numResources++] = resource(RESMGR_RESTYPE_SAMPLE, ptr, true, al_get_time(), (char*)malloc(strlen(filename)+1));
		strcpy(resources[numResources-1].filename, filename);
		qsort(resources, numResources, sizeof(resource), qsResCompare);
		return true;
	}
	else
	{
		if(!r->loaded)
		{
			r->res = ptr;
			r->loaded = true;
			r->last_used = al_get_time();
		}
		else
		{
			al_destroy_sample(ptr);
		}
		return true;
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

bool ResourceMgr::LoadFont(const char* filename, uint32_t fSize)
{
	uint32_t rSize = ui32clip(1, fSize, RESMGR_MAX_FONT_SIZE);
	ALLEGRO_FONT* ptr = al_load_font(filename, -(int32_t)rSize, 0);
	if(!ptr)
	{
		al_destroy_font(ptr);
		return false;
	}
	fontres* r = FindFont(filename);

	if(!r)
	{
		fontres n;
		n.capacity = pow2round(rSize)<<1;
		n.s = (sFont*)malloc(n.capacity*sizeof(sFont));
		for(size_t i=0; i<n.capacity; i++)
			n.s[i] = sFont();
		n.s[rSize].f = ptr;
		n.s[rSize].last_used = al_get_time();
		n.s[rSize].loaded = true;
		n.filename = (char*)malloc(strlen(filename)+1);
		strcpy(n.filename,filename);
		fonts[numFonts++] = n;
		qsort(fonts, numFonts, sizeof(fontres), qsFontCompare);
		return true;
	}
	else
	{
		size_t dCap = pow2round(rSize)<<1;
		if(r->capacity < dCap)
		{
			/*sFont* n = new sFont[dCap];
			memcpy(n, r->s, ui32min(r->capacity, dCap)*sizeof(sFont));
			delete[] r->s;
			r->s = n;*/
			r->s = (sFont*)realloc(r->s, dCap*sizeof(sFont));
			for(size_t i=r->capacity; i<dCap; i++)
				r->s[i] = sFont();
			r->capacity = dCap;
		}
		if(r->s[rSize].f == NULL || !r->s[rSize].loaded)
		{
			r->s[rSize].f = ptr;
			r->s[rSize].last_used = al_get_time();
			r->s[rSize].loaded = true;
			return true;
		}
		else
		{
			al_destroy_font(ptr);
			return true;
		}
	}
}

ALLEGRO_FONT* ResourceMgr::GetFont(const char* filename, uint32_t fSize)
{
	uint32_t rSize = ui32clip(1, fSize, RESMGR_MAX_FONT_SIZE);
	fontres* r = FindFont(filename);
	if(!r)
	{
		if(!LoadFont(filename, fSize))
			return NullFont;
		else
			return FindFont(filename)->s[rSize].f;
	}
	else
	{
		if(!r->s[rSize].loaded)
		{
			if(!LoadFont(filename, rSize))
			{
				return NullFont;
			}
			r->s[rSize].last_used = al_get_time();
			return r->s[rSize].f;
		}
		else
		{
			return r->s[rSize].f;
		}
	}
	return NullFont;
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
		if((uint32_t)rSize < NoReleaseThreshold)
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
	for(size_t i=0; i<numFonts; i++)
	{
		fontres* r = &fonts[i];
		for(size_t j=0; j<r->capacity; j++)
		{
			sFont* rs = &(r->s[j]);
			if(rs->last_used < al_get_time()-ReleaseTime && rs->loaded)
			{
				al_destroy_font(rs->f);
				rs->loaded = false;
			}
		}
	}
}

void ResourceMgr::ReleaseAllResources()
{
	for(int i=0; i<RESMGR_MAX_RESOURCES&&resources[i].filename; i++)
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

	for(size_t i=0; i<numFonts; i++)
	{
		fontres* r = &fonts[i];
		for(size_t j=0; j<r->capacity; j++)
		{
			sFont* rs = &(r->s[j]);
			if(rs->loaded)
			{
				al_destroy_font(rs->f);
				rs->loaded = false;
			}
		}
	}
	if(NullFont && NullBmp && NullSmp)
	{
		al_destroy_font(NullFont);
		al_destroy_sample(NullSmp);
		al_destroy_bitmap(NullBmp);
	}
}

void ResourceMgr::ReloadAllResources()
{
	ReleaseAllResources();

	resource rn[numResources];
	fontres fn[numFonts];
	memcpy(rn,resources,numResources*sizeof(resource));
	memcpy(fn,fonts,numFonts*sizeof(fontres));

	for(size_t i=0; i<numResources; i++)
	{
		this->GetBitmap(rn[i].filename);
		this->GetSample(rn[i].filename);
	}
	for(size_t i=0; i<numFonts; i++)
	{
		for(uint16_t j=0; j<fn[i].capacity; j++)
			if(fn[i].s[j].last_used > 0)
				this->GetFont(fn[i].filename, j);
	}
	GenerateNullResources();
}
