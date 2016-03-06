// Copyright (c) 2016 Juan Delgado (JuDelCo)
// License: GPLv3 License
// GPLv3 License web page: http://www.gnu.org/licenses/gpl.txt

#include "WindowManager.hpp"
#include "InputManager.hpp"
#include "../OpenGL.hpp"
#include "../Resources/Renderer.hpp"
#include "../Resources/DebugLog.hpp"
#include "../ImGui/imgui.hpp"
#include "../ImGui/impl/imgui_impl_glfw.h"
#include <thread>

namespace JuEngine
{
void ErrorCallbackGLFW(int error, const char* description)
{
	JuEngine::DebugLog::Write("GLFW Error (#%i): %s", error, description);
}

WindowManager* WindowManager::mInstance = nullptr;

WindowManager::WindowManager() : IObject("windowManager")
{
	if(WindowManager::mInstance != nullptr)
	{
		ThrowRuntimeError("Error, there are a WindowManager instance created already");
	}

	WindowManager::mInstance = this;
}

WindowManager::~WindowManager()
{
	ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(mWindow);
	glfwTerminate();

	WindowManager::mInstance = nullptr;
}

bool WindowManager::HasFocus()
{
	// TODO
	// glfwGetWindowAttrib(WindowManager::mInstance->mWindow, GLFW_FOCUSED);

	return WindowManager::mInstance->mWindowHasFocus;
}

void WindowManager::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(WindowManager::mInstance->mWindow, title.c_str());
}

vec2 WindowManager::GetSize()
{
	// TODO
	// int width, height; glfwGetFramebufferSize(WindowManager::mInstance->mWindow, &width, &height);

	return WindowManager::mInstance->mWindowFramebufferSize;
}

void WindowManager::SetSize(const unsigned int width, const unsigned int height)
{
	glfwSetWindowSize(WindowManager::mInstance->mWindow, width, height);
}

vec2 WindowManager::GetPosition()
{
	// TODO
	// int xPos, yPos; glfwGetWindowPos(WindowManager::mInstance->mWindow, &xPos, &yPos);

	return WindowManager::mInstance->mWindowPosition;
}

void WindowManager::SetPosition(const int left, const int top)
{
	glfwSetWindowPos(WindowManager::mInstance->mWindow, left, top);
}

bool WindowManager::GetCloseState()
{
	// TODO
	//return glfwWindowShouldClose(WindowManager::mInstance->mWindow);

	return WindowManager::mInstance->mWindowCloseRequested;
}

void WindowManager::SetCloseState(const bool close)
{
	WindowManager::mInstance->mWindowCloseRequested = close;

	// TODO
	// if(close) glfwSetWindowShouldClose(WindowManager::mInstance->mWindow, GL_TRUE);
	// else glfwSetWindowShouldClose(WindowManager::mInstance->mWindow, GL_FALSE);
}

auto WindowManager::GetClipboardString() -> const std::string&
{
	std::string* clipboardStr = new std::string();

	clipboardStr->assign(glfwGetClipboardString(WindowManager::mInstance->mWindow));

	return std::move(*clipboardStr);
}

void WindowManager::SetClipboardString(const std::string& text)
{
	glfwSetClipboardString(WindowManager::mInstance->mWindow, text.c_str());
}

auto WindowManager::GetRenderer() -> std::shared_ptr<Renderer>
{
	if(mInstance->mRenderer.get() != nullptr)
	{
		return mInstance->mRenderer;
	}

	DebugLog::Write("Warning: WindowManager.GetRenderer: No renderer set (in %s)", mInstance->GetId().GetStringRef().c_str());

	return nullptr;
}

void WindowManager::SetRenderer(std::shared_ptr<Renderer> renderer)
{
	mInstance->mRenderer = renderer;
}

void WindowManager::Load()
{
	if(! glfwInit())
	{
		ThrowRuntimeError("Error, failed to init GLFW");
	}

	glfwSetErrorCallback(ErrorCallbackGLFW);

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);
	glfwWindowHint(GLFW_REFRESH_RATE, 60); // For fullscreen
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// TODO
	//glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	// TODO: WindowManager: Resolution, Title, etc -> Make an interface (so I can edit them outside this class)
	mWindow = glfwCreateWindow(1280, 720, "JuEngine Test", NULL, NULL);
	mWindowSize = vec2(1280, 720);
	mWindowFramebufferSize = vec2(1280, 720);

	if (! mWindow)
	{
		glfwTerminate();
		ThrowRuntimeError("Error, failed to create a GLFW window");
	}

	SetActiveInThisThread(true);
	glfwSwapInterval(0);

	glewExperimental = GL_TRUE;
	glewInit();
	ImGui_ImplGlfw_Init(mWindow, false);
	ImGui_ImplGlfw_NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;

	DebugLog::Write("-------------------------------------------------");
	DebugLog::Write("OpenGL context settings:");
	DebugLog::Write("-------------------------------------------------");
	DebugLog::Write("CPU Threads: %i", std::thread::hardware_concurrency());
	DebugLog::Write("GL Version: %s", glGetString(GL_VERSION));
	DebugLog::Write("GPU Vendor: %s", glGetString(GL_VENDOR));
	DebugLog::Write("GPU Renderer: %s", glGetString(GL_RENDERER));
	DebugLog::Write("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	GLint contextProfile; glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &contextProfile);
	DebugLog::Write("GL Context profile mask: %i", contextProfile);
	//if (GLEW_ARB_compatibility) DebugLog::Write("Compatibility profile detected"); else DebugLog::Write("Core profile detected");
	GLint maxUBOSize; glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUBOSize);
	DebugLog::Write("UBO max size: %i bytes", maxUBOSize);
	GLint maxUBOVertex; glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxUBOVertex);
	DebugLog::Write("UBO max count: %i", maxUBOVertex);
	GLint offsetUBOAligment; glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &offsetUBOAligment);
	DebugLog::Write("UBO offset alignment: %i bytes", offsetUBOAligment); // alignment for multiple uniform blocks in one UBO - glBindBufferRange()
	DebugLog::Write("GLFW version: %s", glfwGetVersionString());
	GLint forwardCompat = glfwGetWindowAttrib(mWindow, GLFW_OPENGL_FORWARD_COMPAT);
	DebugLog::Write("GLFW GL Forward Compat mode: %s", (forwardCompat == GL_TRUE ? "True" : "False"));
	GLint debugMode = glfwGetWindowAttrib(mWindow, GLFW_OPENGL_DEBUG_CONTEXT);
	DebugLog::Write("GLFW GL Debug mode: %s", (debugMode == GL_TRUE ? "True" : "False"));
	GLint glProfile = glfwGetWindowAttrib(mWindow, GLFW_OPENGL_PROFILE);
	DebugLog::Write("GLFW GL Profile: %s", (glProfile == GLFW_OPENGL_CORE_PROFILE ? "Core" : (glProfile == GLFW_OPENGL_COMPAT_PROFILE ? "Compatibility" : "Unknown")));
	DebugLog::Write("-------------------------------------------------");

	// TODO: WindowManager: Exit if < GL v3.3

	// --------------------------------

	// TODO
	//glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowSizeCallback(mWindow, WindowManager::CallbackWindowSize);
	glfwSetFramebufferSizeCallback(mWindow, WindowManager::CallbackFramebufferSize);
	glfwSetWindowPosCallback(mWindow, WindowManager::CallbackWindowPosition);
	glfwSetWindowFocusCallback(mWindow, WindowManager::CallbackWindowFocus);
	glfwSetWindowCloseCallback(mWindow, WindowManager::CallbackClose);
	glfwSetDropCallback(mWindow, WindowManager::CallbackDrop);
	glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scanCode, int action, int mods) {
		InputManager::CallbackKeyEvent(window, key, scanCode, action, mods);
		ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mods);
	});
	glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int codePoint) {
		InputManager::CallbackTextEvent(window, codePoint);
		ImGui_ImplGlfw_CharCallback(window, codePoint);
	});
	glfwSetCursorPosCallback(mWindow, InputManager::CallbackMouseMoveEvent);
	glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods) {
		InputManager::CallbackMouseButtonEvent(window, button, action, mods);
		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	});
	glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
		InputManager::CallbackMouseScrollEvent(window, xOffset, yOffset);
		ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
	});
}

void WindowManager::Render()
{
	mRenderer->Render();
	ImGui::Render();

	SwapBuffers();
	ImGui_ImplGlfw_NewFrame();
}

void WindowManager::SwapBuffers()
{
	glfwSwapBuffers(mWindow);
}

void WindowManager::PollEvents()
{
	glfwPollEvents();
}

void WindowManager::SetActiveInThisThread(const bool active)
{
	if(mWindow)
	{
		if(active) glfwMakeContextCurrent(mWindow);
		else glfwMakeContextCurrent(NULL);
	}
}

auto WindowManager::GetWindow() -> GLFWwindow*
{
	if(WindowManager::mInstance->mWindow)
	{
		return WindowManager::mInstance->mWindow;
	}

	DebugLog::Write("Warning: WindowManager.GetWindow: No window set");

	return nullptr;
}

void WindowManager::CallbackWindowSize(GLFWwindow* window, int width, int height)
{
	WindowManager::mInstance->mWindowSize = vec2(width, height);
}

void WindowManager::CallbackFramebufferSize(GLFWwindow* window, int width, int height)
{
	WindowManager::mInstance->mWindowFramebufferSize = vec2(width, height);
}

void WindowManager::CallbackWindowPosition(GLFWwindow* window, int xPos, int yPos)
{
	WindowManager::mInstance->mWindowPosition = vec2(xPos, yPos);
}

void WindowManager::CallbackWindowFocus(GLFWwindow* window, int focused)
{
	WindowManager::mInstance->mWindowHasFocus = focused;
}

void WindowManager::CallbackDrop(GLFWwindow* window, int count, const char** paths)
{
	// TODO
	// for(int i = 0;  i < count;  ++i) handle_dropped_file(paths[i]);
}

void WindowManager::CallbackClose(GLFWwindow* window)
{
	WindowManager::mInstance->mWindowCloseRequested = true;
}
}
