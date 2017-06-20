#ifndef SHADOW_MAP_TECHNIQUE
#define SHADOW_MAP_TECHNIQUE

#include "pgl_techniques.h"
#include "pgl_maths.h"

class ShadowMapTechnique : public Techniques
{
public:

	ShadowMapTechnique();

	virtual bool Init();

	void SetWVP(const Matrix4f& WVP);
	void SetTextureUnit(unsigned int TextureUnit);

private:

	GLuint m_WVPLocation;
	GLuint m_textureLocation;
};

#endif