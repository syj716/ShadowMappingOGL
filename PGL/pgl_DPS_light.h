#ifndef DPS_LIGHT
#define DPS_LIGHT

#include "pgl_techniques.h"
#include "pgl_maths.h"
#include "pgl_lighting_technique.h"
#include "pgl_shadowmap_fbo.h"
#include "pgl_shadowmap_technique.h"

class LightingTechnique : public Techniques
{
public:

	static const unsigned int MAX_POINT_LIGHTS = 1;
    static const unsigned int MAX_SPOT_LIGHTS = 1;

    LightingTechnique();

    virtual bool Init();

    void SetWVP(const Matrix4f& WVP);
	void SetTextureUnit(unsigned int TextureUnit);
	void SetDirectionalLight( DirectionalLight& Light);
	void SetWorldMatrixLocation(const Matrix4f& WV);
	void SetEyeWolrdPos(const Vector3f& EyeWorldPos);
	void SetPointLights(unsigned int NumLights, const PointLight* pLights);
	void SetSpotLights(unsigned int NumLights, const SpotLight* pLights);
	void SetMatSpecularIntensity(float Intensity);
	void SetMatSpecularPower(float Power);
	void SetShadowMapTextureUnit(uint TextureUnit);
	void SetLightWVP(const Matrix4f& LightWVP);

private:

    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
	GLuint m_eyeWorldPosLocation;
	GLuint m_numPointLightsLocation;
	GLuint m_numSpotLightsLocation;
	GLuint m_samplerLocation;
	GLuint m_matSpecularIntensityLocation;
	GLuint m_matSpecularPowerLocation;
	GLuint m_shadowMapLocation;
	GLuint m_LightWVPLocation;

	struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } m_dirLightLocation;

	struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        struct
        {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_pointLightsLocation[MAX_POINT_LIGHTS];
	 
    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint DiffuseIntensity;
        GLuint Position;
        GLuint Direction;
        GLuint Cutoff;
        struct {
            GLuint Constant;
            GLuint Linear;
            GLuint Exp;
        } Atten;
    } m_spotLightsLocation[MAX_SPOT_LIGHTS];
};

#endif