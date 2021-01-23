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

/*#include "SceneManager.h"
#include "LightSceneNode.h"
#include "CameraSceneNode.h"
#include "CameraFPSSceneNode.h"
#include "CameraFollowSceneNode.h"
#include "SkyBoxSceneNode.h"
#include "TiledTerrain.h"
#include "ParticleSystemSceneNode.h"
#include "MeshSceneNode.h"*/

#include "..\\Edgewalker.h"
#include "..\\ResourceManager.h"
#include "..\\SimpleStaticProfiler.h"

ISceneManager *ISceneManager::m_mgr = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneManager::CSceneManager()
	: ISceneNode(0, 0), m_bSettingCameras(FALSE), m_tiledTerrain(NULL),
	m_objectManager(NULL), m_activeCamera(NULL), m_particleEngine(*(this))
{
	// Scene manager is always created in the program so this is
	// a good place to instantiate the singleton tile manager class
	static CTileManager mgr;
	static CResourceManager rmgr;
}

CSceneManager::~CSceneManager()
{
	m_mgr = NULL;
}

void CSceneManager::Clear()
{
	RemoveAll();
}

void CSceneManager::RemoveAll()
{
	SetActiveCamera(NULL);
	ISceneNode::RemoveAll();
	m_objectManager = NULL;
	m_tiledTerrain = NULL;
}

void CSceneManager::Render()
{
}

/**
 * This should never happen
 **/
aabbox3d &CSceneManager::GetBoundingBox()
{
	return *((aabbox3d*)0);
}

void CSceneManager::SetActiveCamera(CCameraSceneNode *cam)
{
	m_activeCamera = cam;
}

bool CSceneManager::GetIsCulled(ISceneNode *node)
{
	if(!node->GetAutomaticCulling())
		return FALSE;

	// TODO: make faster
	aabbox3d box = node->GetBoundingBox();
	D3DXMATRIX mat = node->GetAbsoluteTransformation();
	CMath::TransformBoundingBox(mat, box);
	return box.IntersectsWithBox(m_activeCamera->GetBoundingBox());
}

/**
 * RegisterNodeForRendering
 * registers a node to be rendered
 **/
void CSceneManager::RegisterNodeForRendering(ISceneNode *node, SCENE_NODE_GROUP group)
{
	if(group == SNG_LIGHT_AND_CAMERA)
	{
		m_lightsAndCams.push_back(node);
		return;
	}
	if(group == SNG_SKYBOX)
	{
		m_skyBoxes.push_back(node);
		return;
	}
	if(group == SNG_DEFAULT)
	{
		if(!GetIsCulled(node))
			m_defaultNodes.push_back(node);
		return;
	}
	if(group == SNG_ALPHA_BLENDED)
	{
		if(!GetIsCulled(node))
			m_alphaBlendedNodes.push_back(node);
		else
			CPrinter::log("Alpha blended node autoculled");
		return;
	}
}

void CSceneManager::RenderAll()
{
	//static CSimpleStaticProfiler profiler("results.txt");

	int i;

	RemoveMarkedNodes();

	// Update the active camera's viewing frustum so it can be used for more effective
	// culling in pre-rendering phase
	m_bSettingCameras = TRUE;
	if(m_activeCamera == NULL)
		return;
	m_activeCamera->OnPreRender();
	m_bSettingCameras = FALSE;

	m_defaultNodes.clear();

	// Prepare for rendering (recurses through the scene tree)
	OnPreRender();

	// Render everything
	// First lights and cameras
	for(i=0;i<m_lightsAndCams.size();++i)
		m_lightsAndCams[i]->Render();

	m_lightsAndCams.clear();

	// Skyboxes
	for(i=0;i<m_skyBoxes.size();++i)
		m_skyBoxes[i]->Render();

	m_skyBoxes.clear();

	// TODO: proper render queue, sort nodes by meshes and minimize texture swaps
	// by all meshs' same subsets before moving to next subset

	// Geometric objects
	//if(m_defaultNodes.size() > 200)
	//	i=0;
	for(i=0;i<m_defaultNodes.size();++i)
	{
	//profiler.StartTiming();
		m_defaultNodes[i]->Render();
	//profiler.StopTiming();
	//if(profiler.GetLongestTake() > 0.1)
	//	m_defaultNodes[i]->Render();
	}
	//profiler.CustomMax(m_defaultNodes.size());
	//profiler.CustomMax(CTile::TilesRendered);

	m_defaultNodes.clear();

	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// Alpha blended nodes (particles etc..)
	for(i=0;i<m_alphaBlendedNodes.size();++i)
		m_alphaBlendedNodes[i]->Render();

	m_alphaBlendedNodes.clear();

	m_particleEngine.Render();
}

void CSceneManager::UpdateAll(float frameTime)
{
	GetEdgewalkerApp()->UpdateInputDevices();

	ISceneNode::Update(frameTime);

	ISceneNode::OnPostRender(frameTime);

	RemoveMarkedNodes();
}

void CSceneManager::RemoveMarkedNodes()
{
	vector<ISceneNode*>::iterator it = m_removeList.begin();
	for(;it!=m_removeList.end(); it++)
		if(*it != NULL)
			(*it)->Remove();
	m_removeList.clear();
}

void CSceneManager::RemoveNonSpecialObjects()
{
	list<ISceneNode*>::iterator it = m_children.begin();
	for(; it != m_children.end();)
	{
		if((*it) != m_objectManager && (*it) != m_tiledTerrain)
		{
			if(*it != NULL)
				delete (*it);
			it = m_children.erase(it);
			continue;
		}
		++it;
	}
}

/*********************************
 * 
 * Add scene node functions
 *********************************/

CCameraSceneNode *CSceneManager::AddCameraSceneNode(ISceneNode *parent, long id,
		D3DXVECTOR3 position, D3DXVECTOR3 lookat)
{
	if(!parent)
		parent = this;

	CCameraSceneNode *node = new CCameraSceneNode(parent, this, id, position, lookat);

	return node;
}

CCameraFPSSceneNode *CSceneManager::AddCameraFPSSceneNode(ISceneNode *parent, 
		long id, D3DXVECTOR3 position,	float movespeed, float rotatespeedf)
{
	if(!parent)
		parent = this;

	CCameraFPSSceneNode *node = new CCameraFPSSceneNode(parent, this,
		GetEdgewalkerApp()->GetMouse(), id, movespeed, rotatespeedf, position);

	return node;
}

CCameraFollowSceneNode *CSceneManager::AddCameraFollowSceneNode(ISceneNode *parent, long id,
		D3DXVECTOR3 position)
{
	if(!parent)
		parent = this;

	CCameraFollowSceneNode *node = new CCameraFollowSceneNode(parent, this, id, position);

	return node;
}

CLightSceneNode *CSceneManager::AddLightSceneNode(ISceneNode *parent, bool bStatic,
		D3DXVECTOR3 pos, D3DCOLOR color, float radius, long id)
{
	if(!parent)
		parent = this;

	CLightSceneNode *node = new CLightSceneNode(parent, this, bStatic, pos, color, radius, id);

	return node;
}

CMeshSceneNode *CSceneManager::AddMeshSceneNode(CMesh *mesh, ISceneNode *parent, long id,
		D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale)
{
	if(!parent)
		parent = this;

	CMeshSceneNode *node = new CMeshSceneNode(mesh, parent, this, id, position, rotation, scale);

	return node;
}

CSkyBoxSceneNode *CSceneManager::AddSkyBox(ISceneNode *parent, long id,
		CTexture *top, CTexture *bottom, CTexture *front,
		CTexture *back, CTexture *left, CTexture *right)
{
	if(!parent)
		parent = this;

	CSkyBoxSceneNode *node = new CSkyBoxSceneNode(this, parent, id, top, bottom, front, back, left, right);

	return node;
}

CParticleSystemSceneNode *CSceneManager::AddParticleSystemSceneNode(ISceneNode *parent, long id, 
	D3DXVECTOR3 pos, bool autoDestruct, float duration)
{
	bool autocull = false;
	if(!parent)
	{
		autocull = true;
		parent = this;
	}

	CParticleSystemSceneNode *node = new CParticleSystemSceneNode(parent, this, id, pos, autoDestruct, duration);
	node->SetAutomaticCulling(autocull);

	return node;
}

CTiledTerrain *CSceneManager::AddTiledTerrain(long id)
{
	CTiledTerrain *node = new CTiledTerrain(this, this, id);
	m_tiledTerrain = node;

	return node;
}

CObjectManagerSceneNode *CSceneManager::AddObjectManager(long id, DWORD width, DWORD height, int sectorSize)
{
	CObjectManagerSceneNode *node = new CObjectManagerSceneNode(this, this, id, width, height, sectorSize);
	m_objectManager = node;

	return node;
}

