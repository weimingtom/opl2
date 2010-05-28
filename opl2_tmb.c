#include "opl2.h"

pl2Model *pl2ModelLoad(const char *name)
{
	pl2PackageFile *file = pl2PackageGetFile(name);
	
	if(NULL == file) return NULL;
	
	pl2Model *model = NEW(pl2Model);
	
	uint8_t *data = file->data;
	
	uint32_t i, j;
	
	model->magic = READUINT32(data);
	
	model->ntextures = READUINT32(data);
	
	model->textures = NEWARRAY(model->ntextures, pl2Texture);
	
	for(i = 0; i < model->ntextures; i++)
	{
		pl2Texture *t = model->textures + i;
		READSTRING(32, t->name, data);
		t->hsize = READUINT16(data);
		t->vsize = READUINT16(data);
		uint32_t size = t->hsize * t->vsize;
		t->pixels = malloc(4 * size);
		READSTRING(size, t->pixels, data);
	}
	
	model->nmaterials = READUINT32(data);
	
	model->materials = NEWARRAY(model->nmaterials, pl2Material);
	
	for(i = 0; i < model->nmaterials; i++)
	{
		pl2Material *m = model->materials + i;
		/*
		m->ambient .r = READFLOAT(data);
		m->ambient .g = READFLOAT(data);
		m->ambient .b = READFLOAT(data);
		m->ambient .a = READFLOAT(data);
		*/
		READCOLOR4(&m->ambient, data);
		m->diffuse .r = READFLOAT(data);
		m->diffuse .g = READFLOAT(data);
		m->diffuse .b = READFLOAT(data);
		m->diffuse .a = READFLOAT(data);
		m->specular.r = READFLOAT(data);
		m->specular.g = READFLOAT(data);
		m->specular.b = READFLOAT(data);
		m->specular.a = READFLOAT(data);
		m->emission.r = READFLOAT(data);
		m->emission.g = READFLOAT(data);
		m->emission.b = READFLOAT(data);
		m->emission.a = READFLOAT(data);
		m->shininess  = READFLOAT(data);
		m->texid     = READUINT32(data);
	}

	model->nobjects = READUINT32(data);
	
	model->objects = NEWARRAY(model->nobjects, pl2Object);
	
	for(i = 0; i < model->nobjects; i++)
	{
		pl2Object *o = model->objects + i;
		READSTRING(32, o->name, data);
		READMATRIX4(&o->transform, data);
		o->ntriangles = READUINT32(data);
		o->reserved   = READUINT32(data);
		o->nmaterials = READUINT32(data);
		
		o->materials  = NEWARRAY(o->nmaterials, pl2ObjMtl);
		o->vertices   = NEWARRAY(o->ntriangles * 3, pl2Vertex);
		o->glvertices = NEWARRAY(o->ntriangles * 3, pl2GlVertex);
		
		for(j = 0; j < o->nmaterials; j++)
		{
			o->materials[j].mtlid = READUINT32(data);
			o->materials[j].start = READUINT32(data);
			o->materials[j].count = READUINT32(data);
		}
		
		for(j = 0; j < o->ntriangles * 3; j++)
		{
			READVECTOR3(&(o->vertices[j].vertex), data);
			o->vertices[j].weights[0] = READFLOAT(data);
			o->vertices[j].weights[1] = READFLOAT(data);
			o->vertices[j].weights[2] = READFLOAT(data);
			READSTRING(4, o->vertices[j].bones, data);
			READVECTOR3(&(o->vertices[j].normal), data);
			o->vertices[j].color = READUINT32(data);
			READTEXCOORD2(&(o->vertices[j].texcoord), data);
		}
	}
	
	return model;
}

