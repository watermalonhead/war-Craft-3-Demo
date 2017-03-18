#pragma once
#include<d3dx9.h>
struct sRay
{
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;
};
sRay calculate_picking_ray(int x, int y, LPDIRECT3DDEVICE9 g_device)
{
	D3DVIEWPORT9 viewport;
	g_device->GetViewport(&viewport);

	D3DXMATRIX proj_matrix;
	g_device->GetTransform(D3DTS_PROJECTION, &proj_matrix);

	float px = (((2.0f * x) / viewport.Width) - 1.0f) / proj_matrix(0, 0);
	float py = (((-2.0f * y) / viewport.Height) + 1.0f) / proj_matrix(1, 1);

	sRay ray;
	ray.origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ray.direction = D3DXVECTOR3(px, py, 1.0f);

	return ray;
}

void transform_ray(sRay* ray, D3DXMATRIX* trans_matrix)
{
	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(&ray->origin, &ray->origin, trans_matrix);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(&ray->direction, &ray->direction, trans_matrix);

	// normalize the direction
	D3DXVec3Normalize(&ray->direction, &ray->direction);
}

bool IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,float* t,float* u,float* v)
{
	//E1
	D3DXVECTOR3 E1 = v1 - v0;

	//E2
	D3DXVECTOR3 E2 = v2 - v0;

	//P
	D3DXVECTOR3 P;
	D3DXVec3Cross(&P, &dir, &E2);

	// determinant
	float det = D3DXVec3Dot(&E1, &P);

	// keep det > 0, modify T accordingly
	D3DXVECTOR3 T;
	if (det>0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
	{
		return false;
	}

	// Calculate u and make sure u <= 1
	*u = D3DXVec3Dot(&T,&P);
	if (*u < 0.0f || *u > det)
	{
		return false;
	}

	// Q
	D3DXVECTOR3 Q;
	D3DXVec3Cross(&Q, &T, &E1);

	// Calculate v and make sure u + v <= 1
	*v = D3DXVec3Dot(&dir, &Q);
	if (*v < 0.0f || *u + *v > det)
	{
		return false;
	}

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot(&E2, &Q);

	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}
