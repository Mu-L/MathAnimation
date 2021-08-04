#include "core.h"
#include "core/Window.h"
#include "core/Input.h"
#include "renderer/Renderer.h"
#include "renderer/OrthoCamera.h"
#include "renderer/Shader.h"
#include "renderer/Framebuffer.h"
#include "renderer/Texture.h"
#include "renderer/VideoWriter.h"
#include "renderer/Fonts.h"
#include "animation/Animation.h"
#include "animation/GridLines.h"
#include "animation/Settings.h"
#include "animation/Sandbox.h"
#include "animation/Styles.h"

namespace MathAnim
{
	namespace Application
	{
		static bool playAnim = false;
		static bool outputVideoFile = false;

		static int framerate = 60;
		static int outputWidth = 3840;
		static int outputHeight = 2160;

		void run()
		{
			// Initiaize GLFW/Glad
			bool isRunning = true;

			Window window = Window(1920, 1080, "Math Animations");
			window.setVSync(true);

			OrthoCamera camera;
			camera.position = glm::vec2(0, 0);
			camera.projectionSize = glm::vec2(6.0f * (1920.0f / 1080.0f), 6.0f);

			Fonts::init();
			Renderer::init(camera);
			Sandbox::init();

			Texture mainTexture = TextureBuilder()
				.setFormat(ByteFormat::RGBA8_UI)
				.setWidth(outputWidth)
				.setHeight(outputHeight)
				.build();

			Framebuffer mainFramebuffer = FramebufferBuilder(outputWidth, outputHeight)
				.addColorAttachment(mainTexture)
				.generate();

			// Run game loop
			// Start with a 60 fps frame rate
			float previousTime = glfwGetTime() - 0.16f;
			while (isRunning && !window.shouldClose())
			{
				float deltaTime = glfwGetTime() - previousTime;
				previousTime = glfwGetTime();

				// Bind main framebuffer
				mainFramebuffer.bind();
				glViewport(0, 0, mainFramebuffer.width, mainFramebuffer.height);
				Renderer::clearColor(Colors::greenBrown);

				// Update components
				if (Settings::displayGrid)
				{
					GridLines::update(camera);
				}

				if (playAnim)
				{
					AnimationManager::update(deltaTime);
					Sandbox::update(deltaTime);
				}

				if (outputVideoFile)
				{
					AnimationManager::update(1.0f / (float)framerate);
					Sandbox::update(1.0f / (float)framerate);
				}

				// Render to main framebuffer
				Renderer::render();

				// Render to screen
				mainFramebuffer.unbind();
				glViewport(0, 0, window.width, window.height);
				Renderer::renderFramebuffer(mainFramebuffer);

				if (outputVideoFile)
				{
					Pixel* pixels = mainFramebuffer.readAllPixelsRgb8(0);
					VideoWriter::pushFrame(pixels, outputHeight * outputWidth);
					mainFramebuffer.freePixels(pixels);
				}

				window.swapBuffers();

				if (Input::isKeyPressed(GLFW_KEY_ESCAPE))
				{
					isRunning = false;
				}
				else if (Input::isKeyPressed(GLFW_KEY_F5) && !playAnim)
				{
					playAnim = true;
				}
				else if (Input::isKeyPressed(GLFW_KEY_F6) && !outputVideoFile)
				{
					outputVideoFile = true;
					AnimationManager::reset();
					VideoWriter::startEncodingFile("output.mp4", outputWidth, outputHeight, framerate);
				}
				else if (Input::isKeyPressed(GLFW_KEY_F7) && outputVideoFile)
				{
					outputVideoFile = false;
					VideoWriter::finishEncodingFile();
				}

				window.pollInput();
			}

			Window::cleanup();
		}
	}
}