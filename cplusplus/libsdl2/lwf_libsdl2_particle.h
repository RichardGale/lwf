#ifndef LWF_COCOS2DX_PARTICLE_H
#define LWF_COCOS2DX_PARTICLE_H

#include "lwf_renderer.h"
#include "lwf_libsdl2_factory.h"

class CCLWFNode;

namespace LWF {

class LWFParticle;

class LWFParticleRenderer : public Renderer
{
protected:
	LWFParticle *m_particle;
    LWFRendererFactory *m_factory;

public:
	LWFParticleRenderer(LWF *l, Particle *particle, SDL_Renderer *node);
	virtual ~LWFParticleRenderer();

	void Destruct();
	void Update(const Matrix *matrix, const ColorTransform *colorTransform);
	void Render(const Matrix *matrix, const ColorTransform *colorTransform,
		int renderingIndex, int renderingCount, bool visible);
};

}   // namespace LWF

#endif
