#include <math.h>
#include "pgl_main.h"

class BlueRenderer : public ICallbacks
{

private:
	void CalculateNormals(const unsigned int* pIndices,unsigned int indexCount, Vertex *pVertex, unsigned int vertexCount)
	{
		for( unsigned int i = 0; i < indexCount; i +=3)
		{
			unsigned int Index0 = pIndices[i];
			unsigned int Index1 = pIndices[i+1];
			unsigned int Index2 = pIndices[i+2];

			Vector3f v1 = pVertex[Index1].m_pos - pVertex[Index0].m_pos;
			Vector3f v2 = pVertex[Index2].m_pos - pVertex[Index0].m_pos;

			Vector3f Normal = v1.Cross(v2);
			Normal.Normalize();

			pVertex[Index0].m_normal += Normal;
			pVertex[Index1].m_normal += Normal;
			pVertex[Index2].m_normal += Normal;
		}

		// Normalize all the vertex normals
		for (unsigned int i = 0 ; i < vertexCount ; i++) 
		{
			pVertex[i].m_normal.Normalize();
		}
	}

	LightingTechnique* m_pLightingEffect;
    ShadowMapTechnique* m_pShadowMapEffect;
    Camera* m_pGameCamera;
    float m_scale;
    SpotLight m_spotLight;
    Mesh* m_pMesh;
    Mesh* m_pQuad;
    Texture* m_pGroundTex;
    ShadowMapFBO m_shadowMapFBO;
    PersProjInfo m_persProjInfo;


public:

    BlueRenderer()
    {
		m_pLightingEffect = NULL;
        m_pShadowMapEffect = NULL;
        m_pGameCamera = NULL;
        m_pMesh = NULL;
        m_pQuad = NULL;
        m_scale = 0.0f;
        m_pGroundTex = NULL;

        m_spotLight.AmbientIntensity = 0.1f;
        m_spotLight.DiffuseIntensity = 0.9f;
        m_spotLight.Color = Vector3f(1.0f, 1.f, 1.0f);
        m_spotLight.Attenuation.Linear = 0.01f;
        m_spotLight.Position  = Vector3f(-20.0, 20.0, 1.0f);
        m_spotLight.Direction = Vector3f(1.0f, -1.0f, 0.0f);
        m_spotLight.Cutoff =  20.0f;

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 500.0f;        
	}

	~BlueRenderer()
	{
		SAFE_DELETE(m_pLightingEffect);
		SAFE_DELETE(m_pShadowMapEffect);
		SAFE_DELETE(m_pGameCamera);
		SAFE_DELETE(m_pMesh);
		SAFE_DELETE(m_pQuad);
		SAFE_DELETE(m_pGroundTex);
	}

    bool Init()
    {
		if(!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT))
			return false;

		m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

		m_pLightingEffect = new LightingTechnique();
		if(!m_pLightingEffect->Init())
		{
			printf("Error while initializing lighting technique\n");
			return false;
		}
		m_pLightingEffect->Enable();
		m_pLightingEffect->SetSpotLights(1, &m_spotLight);
		m_pLightingEffect->SetTextureUnit(0);
		m_pLightingEffect->SetShadowMapTextureUnit(1);

		m_pShadowMapEffect = new ShadowMapTechnique();
		if(!m_pShadowMapEffect->Init())
		{
			printf("Error Initializing shadow map technique\n");
			return false;
		}

		m_pQuad = new Mesh();
		if(!m_pQuad->LoadMesh("../Content/quad.obj"))
			return false;

		m_pGroundTex = new Texture(GL_TEXTURE_2D, "../Content/test1.jpg");
		if(!m_pGroundTex->Load())
			return false;

		m_pMesh = new Mesh();
		if(!m_pMesh->LoadMesh("../Content/phoenix_ugv.md2"))
			return false;

		return true;
        
    }

	virtual void SpecialKeyboardCB(int Key, int x, int y)
	{
		OGLDEV_KEY OgldevKey = GLUTKeyToOGLDEVKey(Key);
		m_pGameCamera->OnKeyboard(OgldevKey);
	}

    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
       
        m_scale += 0.10f;

        ShadowMapPass();
		RenderPass();

        glutSwapBuffers();
    }

	 virtual void ShadowMapPass()
    {
        m_shadowMapFBO.BindForWriting();

        glClear(GL_DEPTH_BUFFER_BIT);

        m_pShadowMapEffect->Enable();

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(0.0f, m_scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 3.0f);
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.SetPerspectiveProj(m_persProjInfo);
        m_pShadowMapEffect->SetWVP(p.GetWVPTrans());
        m_pMesh->Render();
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    }
    
    virtual void RenderPass()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
		m_pLightingEffect->Enable();
		m_pLightingEffect->SetEyeWolrdPos(m_pGameCamera->GetPos());

		m_shadowMapFBO.BindForReading(GL_TEXTURE1);
		
		Pipeline p;
		
		p.SetPerspectiveProj(m_persProjInfo);

		p.Scale(10.0f, 10.0f, 10.0f);
		p.WorldPos(0.0f,0.0f,1.0f);
		p.Rotate(90.0f,0.0f,0.0f);
		p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());

		m_pLightingEffect->SetWVP(p.GetWVPTrans());
		m_pLightingEffect->SetWorldMatrixLocation(p.GetWorldTrans());

		p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
		m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
		
		m_pGroundTex->Bind(GL_TEXTURE0);
		m_pQuad->Render();
		

		p.Scale(0.1f, 0.1f, 0.1f);
		p.Rotate(0.0f, m_scale, 0.0f);
		p.WorldPos(0.0f, 0.0f, 3.0f);
		p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
		
		m_pLightingEffect->SetWVP(p.GetWVPTrans());
		m_pLightingEffect->SetWorldMatrixLocation(p.GetWorldTrans());
		
		p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
		m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
		m_pMesh->Render(); 
		
    }


    virtual void KeyboardCB(OGLDEV_KEY OgldevKey)
    {
        switch (OgldevKey) {
            case OGLDEV_KEY_ESCAPE:
            case OGLDEV_KEY_q:
                    GLUTBackendLeaveMainLoop();
                    break;

            case OGLDEV_KEY_a:
				
                break;

            case OGLDEV_KEY_s:
				
                break;
        }
    }
	    


};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv, true, false);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, false, "PGL 1.0")) {
        return 1;
    }

    BlueRenderer* pApp = new BlueRenderer();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;
 
    return 0;
}
