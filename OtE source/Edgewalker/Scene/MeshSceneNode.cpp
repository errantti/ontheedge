// On the Edge
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "MeshSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshSceneNode::CMeshSceneNode(CMesh *mesh, ISceneNode *parent, ISceneManager *manager, DWORD id,
		D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale)
	: IGeometricObject(parent, manager, id, position, rotation, scale), m_mesh(mesh)
{
	UpdateBoundingVolumes();
}

CMeshSceneNode::~CMeshSceneNode()
{
}

void CMeshSceneNode::OnPreRender()
{
	if(m_bVisible && m_mesh)
	{
		m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
	}
	ISceneNode::OnPreRender();
}

void CMeshSceneNode::Render()
{
	if(m_mesh == NULL)
		return;

	// Render the mesh
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);
	m_mesh->Render();
}

void CMeshSceneNode::Update(float frameTime)
{
	ISceneNode::Update(frameTime);
}

void CMeshSceneNode::SetMesh(CMesh *mesh)
{
	if(!mesh)
		return;

	m_mesh = mesh;

	UpdateBoundingVolumes();
}

void CMeshSceneNode::UpdateBoundingVolumes()
{
	if(m_mesh == NULL)
		return;

	m_bbox = m_mesh->GetBoundingBox();
	m_bs = m_mesh->GetBoudingSphere();

	m_bbox.minCorner = m_bvCoefficient * m_bbox.minCorner;
	m_bbox.maxCorner = m_bvCoefficient * m_bbox.maxCorner;
	m_bs.radius = m_bvCoefficient * m_bs.radius;
}

void CMeshSceneNode::CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions)
{
	if(object == NULL || object == this)
		return;

	if(m_bStatic && object->IsStatic())
		return;

	// Check collision using simple bounding sphere test
	SCollision collision;
	collision.v = object->GetAbsolutePosition() - GetTranslation();
	float r = m_bs.radius + object->GetBoundingSphere().radius;
	float vp2 = collision.v.x*collision.v.x + collision.v.y*collision.v.y + collision.v.z*collision.v.z;
	if(vp2<r*r)
	{
		// Collision
		//coefficient = sqrtfast(r/vp2) -1.f; // = (sqrt(r) - sqrt(vp2)) / sqrt(vp2)
		//collisionvector = coefficient * (c2-c1); // c = center
		if(object->CollidesWithObjects() == TRUE && CollidesWithObjects() == TRUE)
		{
			collision.v *= r*sqrtfast(1.f/vp2) -1.f;
			collision.o1 = this;
			collision.o2 = object;
			collisions.push_back(collision);
		}

		// Add collision direction and depth to the vector
		RegisterCollision(object);
		object->RegisterCollision(this);
	}
}