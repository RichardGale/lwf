#include "SDL.h"

#include "lwf_libsdl2_particle.h"
#include "lwf_core.h"
#include "lwf_data.h"
#include "lwf_text.h"
#include "lwf_particle.h"

namespace LWF {
    
    class LWFParticle
    {
    public:
        string path;
    protected:
        Matrix m_matrix;
        
    public:
        static LWFParticle *create(const char *path)
        {
            LWFParticle *particle = new LWFParticle();
            if (particle && particle->initWithFile(path)) {
                return particle;
            }
            delete particle;
            return NULL;
        }
        
        bool initWithFile(const char *filePath)
        {
            return false;
        }
        
        void setMatrixAndColorTransform(const Matrix *m, const ColorTransform *)
        {
        }
    };
    
    LWFParticleRenderer::LWFParticleRenderer(LWF *l,
                                             Particle *particle,
                                             SDL_Renderer *node)
	: Renderer(l), m_particle(0)
    {
        const Format::Particle &p = l->data->particles[particle->objectId];
        const Format::ParticleData &d = l->data->particleDatas[p.particleDataId];
        string filename = l->data->strings[d.stringId];
        filename += ".plist";
        m_factory = (LWFRendererFactory *)l->rendererFactory.get();
        string path = m_factory->m_basePath + filename;

        m_particle = LWFParticle::create(path.c_str());
        if (!m_particle) {
            m_particle = LWFParticle::create(path.c_str());
            if (!m_particle)
                return;
        }
    }
    
    LWFParticleRenderer::~LWFParticleRenderer()
    {
    }
    
    void LWFParticleRenderer::Destruct()
    {
        if (!m_particle)
            return;

        delete m_particle;
        m_particle = 0;
    }
    
    void LWFParticleRenderer::Update(const Matrix *matrix,
                                     const ColorTransform *colorTransform)
    {
    }
    
    void LWFParticleRenderer::Render(const Matrix *matrix,
                                     const ColorTransform *colorTransform,
                                     int renderingIndex,
                                     int renderingCount,
                                     bool visible)
    {
        if (!m_particle)
            return;
        
        if (!visible)
            return;

        //m_particle->setZOrder(renderingIndex);
        //m_particle->setMatrixAndColorTransform(matrix, colorTransform);
    }
    
}   // namespace LWF
