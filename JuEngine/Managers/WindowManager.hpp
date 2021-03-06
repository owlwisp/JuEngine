// Copyright (c) 2016 Juan Delgado (JuDelCo)
// License: GPLv3 License
// GPLv3 License web page: http://www.gnu.org/licenses/gpl.txt

#pragma once

#include "../Services/IWindowService.hpp"

namespace JuEngine
{
class JUENGINEAPI WindowManager : public IWindowService
{
	public:
		WindowManager();
		~WindowManager();

		bool HasFocus();
		void SetTitle(const std::string& title);
		vec2 GetSize();
		void SetSize(const unsigned int width, const unsigned int height);
		vec2 GetPosition();
		void SetPosition(const int left, const int top);
		bool GetCloseState();
		void SetCloseState(const bool close);
		auto GetClipboardString() -> const std::string&;
		void SetClipboardString(const std::string& text);
		auto GetRenderer() -> std::shared_ptr<Renderer>;
		void SetRenderer(std::shared_ptr<Renderer> renderer);
		void SetCursorMode(WindowCursorMode mode);
		auto GetKeyState(int key) -> WindowInputState;
		auto GetMouseButtonState(int button) -> WindowInputState;
		void GetCursorPosition(double* xPos, double* yPos);
		void SetCursorPosition(double xPos, double yPos);

		void CallbackWindowSize(int width, int height);
		void CallbackFramebufferSize(int width, int height);
		void CallbackWindowPosition(int xPos, int yPos);
		void CallbackWindowFocus(int focused);
		void CallbackDrop(int count, const char** paths);
		void CallbackClose();

	protected:
		void Load();
		void Render();
		void SwapBuffers();
		void PollEvents();
		void SetActiveInThisThread(const bool active = true);
		auto GetWindow() -> GLFWwindow*;

	private:
		GLFWwindow* mWindow{nullptr};
		bool mWindowCloseRequested{false};
		bool mWindowHasFocus{true};
		vec2 mWindowPosition{0,0};
		vec2 mWindowSize{0,0};
		vec2 mWindowFramebufferSize{0,0};
		std::shared_ptr<Renderer> mRenderer;
};
}
