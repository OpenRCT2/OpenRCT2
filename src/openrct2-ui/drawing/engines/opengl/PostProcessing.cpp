#include "PostProcessing.h"

#include <openrct2/Context.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/FileStream.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <set>

namespace OpenRCT2::Ui
{

    static std::vector<std::string> GetAvailableShaders()
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto shadersPath = env->GetDirectoryPath(DIRBASE::OPENRCT2, DIRID::SHADER);
        auto path = Path::Combine(shadersPath, "pp");

        // Traverse folder, just iterate .frag, always assume it also has .vert
        std::vector<std::string> shaders;
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                auto extension = entry.path().extension();
                if (extension == ".frag")
                {
                    shaders.push_back("pp/" + entry.path().stem().string());
                }
            }
        }

        return shaders;
    }

    PostProcessing::PostProcessing()
    {
        _available = GetAvailableShaders();
    }

    void PostProcessing::Initialize()
    {
        for (const auto& shader : _available)
        {
            _postProcessShaders.push_back(std::make_unique<PostProcessShader>(shader));
        }
    }

    void PostProcessing::Resize(int32_t width, int32_t height)
    {
        _postProcessFramebuffer = std::make_unique<OpenGLFramebuffer>(width, height, false, false);
        _postProcessFramebuffer->SetName("PostProcessFrameBuffer");
    }

    void PostProcessing::Update()
    {
        for (auto& shader : _postProcessShaders)
        {
            shader->Update();
        }
    }

    void PostProcessing::Apply(OpenGLFramebuffer& screenFrameBuffer)
    {
        _postProcessFramebuffer->Bind();
        _postProcessFramebuffer->Copy(screenFrameBuffer, GL_NEAREST);

        for (auto& shader : _postProcessShaders)
        {
            shader->Use();
            shader->SetTexture(_postProcessFramebuffer->GetTexture());
            shader->Draw();
        }

        screenFrameBuffer.Copy(*_postProcessFramebuffer, GL_NEAREST);
    }

} // namespace OpenRCT2::Ui
