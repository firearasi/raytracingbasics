#ifndef TRANSFORMATIONSH
#define TRANSFORMATIONSH

#include "hitable.h"
#include "matrix.h"

class translate: public hitable
{
public:
	translate(vec3 o, hitable *_obj):offset(o), obj(_obj){}
	~translate(){free();}
	virtual void free();
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const ;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
	vec3 offset;
	hitable *obj;
};

void translate::free()
{
	if(obj)
		delete obj;
}

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	ray r_tl(r.origin()-offset, r.direction());
	bool bHit = obj->hit(r_tl, t_min, t_max, rec);
	if(bHit)
	{
		rec.p += offset;
	}
	return bHit;
}

bool translate::bounding_box(float t0, float t1, aabb& box) const
{
	bool bBound = obj->bounding_box(t0, t1, box);
	if(bBound)
	{
		box._min += offset;
		box._max += offset;
	}
	return bBound;
}

// linear
class linear: public hitable
{
public:
	linear(mat3 _m, hitable *_obj):m(_m), obj(_obj)
	{
		invTranspose = m.invTranspose();
		inv=invTranspose.transpose();
	}
	~linear()
	{
		free();
	}
	virtual void free();
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const ;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
	mat3 m;
	mat3 inv;
	mat3 invTranspose;
	hitable *obj;
};

void linear::free()
{
	if(obj)
	{
		delete obj;
	}
}
bool linear::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	ray r_inv=inv*r;
	bool bHit = obj->hit(r_inv, t_min, t_max, rec);
	if(bHit)
	{
		rec.p = m * rec.p;

		//  normal is in the cotangent space and is contravariant, tr(inv(m))
		rec.normal = unit_vector(invTranspose * rec.normal);
	}
	return bHit;
}

bool linear::bounding_box(float t0, float t1, aabb& box) const
{
	bool bBound = obj->bounding_box(t0, t1, box);
	if(bBound)
	{
		box=m*box;
	}
	return bBound;
}


#endif
