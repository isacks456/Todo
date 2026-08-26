// Dear ImGui: standalone example application for Windows API + DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// Data
static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Task
{
	int id;
	std::string title;
	std::string description;
	bool completed;

public:
	Task(int p_id, std::string p_title, std::string p_description, bool p_completed) : id{ p_id }, title{ std::move(p_title) }, description{std::move(p_description) }, completed{ p_completed }
	{

	}

	int GetId() const
	{
		return id;
	}

	const std::string& GetTitle() const
	{
		return title;
	}

	const std::string& GetDescription() const
	{
		return description;
	}

	bool GetCompleted() const
	{
		return completed;
	}

	void SetTitle(const std::string& newTitle)
	{
		title = newTitle;
	}

	void SetDescription(const std::string& newDescription)
	{
		description = newDescription;
	}

	void SetCompeted(const bool& newCompleted)
	{
		completed = newCompleted;
	}

};

class Todo
{
	int newId = 1;

public:

std::vector<Task> tasks;

	void addTask(std::string newTitle, std::string newDescription)
	{


		tasks.push_back(Task(newId, std::move(newTitle), std::move(newDescription), false));
		newId++;

	};


	void changeTask(int targetId, std::string newTitle, std::string newDescription)
	{

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == targetId)
			{
				tasks[i].SetTitle(newTitle);
				tasks[i].SetDescription(newDescription);
				break;
			}
		}
	}

	void taskCompleted(int targetId)
	{

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == targetId)
			{
				tasks[i].SetCompeted(true);
				break;
			}
		}

	}

	void dropTask(int targetId)
	{

		for (size_t i = 0; i < tasks.size(); ++i)
		{
			if (tasks[i].GetId() == targetId)
			{

				tasks.erase(tasks.begin() + i);
				break;
			}
		}

        if(tasks.empty())
        {
            newId = 1;
        }

	}

	void addToFile()
	{
		std::ofstream out("Todo.txt");

		if (!out.is_open())
		{
			return;
		}

		for (size_t i = 0; i < tasks.size(); ++i)
		{
				out << tasks[i].GetId() << "\n"
					<< tasks[i].GetTitle() << "\n"
					<< tasks[i].GetDescription() << "\n"
					<< (tasks[i].GetCompleted() ? " Выполнено " : " В процессе... ") << "\n"
					<< std::endl;
		}

		out.close();

	}

    void loadFromFile()
    {
        std::ifstream in("Todo.txt");

        if (!in.is_open()) return; 

        tasks.clear(); 
        int max_id = 0; 

        std::string line_id, title, desc, status_str, empty_line;

        
        while (std::getline(in, line_id))
        {
            std::getline(in, title);
            std::getline(in, desc);
            std::getline(in, status_str);
            std::getline(in, empty_line); 

            
            int id = std::stoi(line_id); 
            
            bool completed = (status_str.find("Выполнено") != std::string::npos);

            tasks.push_back(Task(id, title, desc, completed));

            if (id > max_id) {
                max_id = id;
            }
        }

        in.close(); 

        newId = max_id + 1;
    }




};

// Main code

Todo list;

int main(int, char**)
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Todo By IsmaFx", WS_OVERLAPPEDWINDOW, 100, 100, (int)(1280 * main_scale), (int)(800 * main_scale), nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    
    HICON hIcon = ::LoadIconW(wc.hInstance, MAKEINTRESOURCEW(1)); 
    
    if (!hIcon) {
        hIcon = (HICON)::LoadImageW(nullptr, L"TodoFx.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    }

    ::SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    ::SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    // style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    style.WindowRounding = 8.0f;     
    style.FrameRounding = 6.0f;      
    style.PopupRounding = 6.0f;      
    style.FramePadding = ImVec2(6, 6); 
    style.ItemSpacing = ImVec2(8, 8);

    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.45f, 0.45f, 0.45f, 0.45f); 
    style.Colors[ImGuiCol_Header]           = ImVec4(0.35f, 0.20f, 0.50f, 1.00f); 
    style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.45f, 0.25f, 0.65f, 1.00f); 
// Кнопки
    style.Colors[ImGuiCol_Button]           = ImVec4(0.20f, 0.60f, 0.90f, 1.00f); 
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.20f, 0.60f, 0.90f, 0.70f); 
    style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.15f, 0.50f, 0.80f, 1.00f); 

// Поля ввода (InputText, Checkbox)
    style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.20f, 0.22f, 0.27f, 1.00f); 
    style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.25f, 0.28f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.70f, 0.30f, 0.90f, 1.00f);
    

    style.Colors[ImGuiCol_Border]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);


    style.FrameBorderSize = 1.0f;  

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");

	ImFontConfig font_config;
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\tahoma.ttf", 18.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());
    if (font == nullptr) {
        io.Fonts->AddFontDefault(); // Если файл шрифта вдруг не найдет, загрузит стандартный англоязычный
    }

    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    static char title_buf[128] = "";
    static char desc_buf[128] = "";
    static char edit_title_buf[128] = "";
    static char edit_desc_buf[128] = "";
    static int editing_task_id = -1;
    static bool show_empty_warning = false;
    static bool show_success_message = false;

     list.loadFromFile();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
       // if (show_demo_window)
            //ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGuiIO& io = ImGui::GetIO();
            
           
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            
           
            ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

            
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

            ImGui::Begin("Мой Списочек Задач", nullptr, window_flags);

			ImGui::Text("Добавление новой задачи:");

            ImGui::InputText("Название", title_buf, IM_ARRAYSIZE(title_buf));
            ImGui::InputText("Описание", desc_buf, IM_ARRAYSIZE(desc_buf));

			if (ImGui::Button("Добавить задачу")) 
            {
                if(strlen(title_buf) > 0)
                {
                    list.addTask(title_buf, desc_buf);

                    title_buf[0] = '\0';
                    desc_buf[0] = '\0';

                    show_empty_warning = false;
                    show_success_message = false;

                }
			}

            ImGui::SameLine();

            if(ImGui::Button("Сохранить в файл"))
            {
                if(list.tasks.empty())
                {
                    show_empty_warning = true;
                    show_success_message = false;
                }
                else
                {
                    show_empty_warning = false;
                    show_success_message = true;
                    list.addToFile();
                }
            }

            if(show_empty_warning)
            {
                ImGui::SameLine();

                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Ошибка: Добавьте хотя бы одну задачу!");
            }

            if(show_success_message)
            {
                ImGui::SameLine();

                ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Файл успешно создан!");
            }


            ImGui::Separator();
            ImGui::Text("Ваши текущие дела:");

            ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;

            if (ImGui::BeginChild("TaskRegion", ImVec2(0, 300), true, child_flags))
            {

            for(size_t i = 0; i < list.tasks.size(); i++)
            {
                Task& task = list.tasks[i];

                bool is_completed = task.GetCompleted();


                if(ImGui::Checkbox(("##check_" + std::to_string(i)).c_str(), &is_completed))
                {
                    list.taskCompleted(task.GetId());
                }

                ImGui::SameLine();

                if(ImGui::Button(("Удалить##" + std::to_string(i)).c_str()))
                {
                    list.dropTask(task.GetId());

                    i--;
                }

                ImGui::SameLine();

                if(ImGui::Button(("Изменить##" + std::to_string(i)).c_str()))
                {

                    editing_task_id = task.GetId();

                    strcpy_s(edit_title_buf, task.GetTitle().c_str());
                    strcpy_s(edit_desc_buf, task.GetDescription().c_str());

                }

                ImGui::SameLine();

                if(task.GetCompleted())
                {
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%d] %s: %s (Выполнено)", task.GetId(), task.GetTitle().c_str(), task.GetDescription().c_str());
                } else
                {
                    ImGui::Text("[%d] %s: %s", task.GetId(), task.GetTitle().c_str(), task.GetDescription().c_str());
                }
            }

                ImGui::EndChild(); 
        }

            if(editing_task_id != -1)
            {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Редактирование задачи с ID: %d", editing_task_id);

                ImGui::InputText("Новое название", edit_title_buf, IM_ARRAYSIZE(edit_title_buf));
                ImGui::InputText("Новое описание", edit_desc_buf, IM_ARRAYSIZE(edit_desc_buf));

                if(ImGui::Button("Сохранить изменения"))
                {
                    list.changeTask(editing_task_id, edit_title_buf,edit_desc_buf);

                    editing_task_id = -1;
                }

                ImGui::SameLine();

                if(ImGui::Button("Отмена"))
                {
                    editing_task_id = -1;
                }

            }

            ImGui::End();

        }

        // 3. Show another simple window.
        // if (show_another_window)
        // {
        //     ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        //     ImGui::Text("Hello from another window!");
        //     if (ImGui::Button("Close Me"))
        //         show_another_window = false;
        //     ImGui::End();
        // }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    // This is a basic setup. Optimally could use e.g. DXGI_SWAP_EFFECT_FLIP_DISCARD and handle fullscreen mode differently. See #8979 for suggestions.
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
