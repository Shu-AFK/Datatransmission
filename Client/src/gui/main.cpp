// TODO: Next to form display a list of all connected clients. With a double click switch the chat to that client
// TODO: Save and load states(All previous shells and connections)

#define IMGUI_API

#if defined (_WIN32) && (_WIN32_WINNT < 0x0600)
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include "imgui.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#include <filesystem>
#include <utility>
#include <vector>
#include <format>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <d3d11.h>
#include <tchar.h>

#define DATATRANSMISSION_CLIENT_GUI
#include "../client.h"

#include "style.h"

static ID3D11Device             *device = nullptr;
static ID3D11DeviceContext      *context = nullptr;
static IDXGISwapChain           *SwapChain = nullptr;
static bool                     SwapChainOccluded = false;
static UINT                     resizeWidth = 0, resizeHeight = 0;
static ID3D11RenderTargetView   *mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hwnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);

void renderGUI(bool *done);

std::string getButtonSelectionName(int connPos);

bool checkInputs(char *ipv4, char *port, const char *username, const char *password);
void connectToServer(char *ipv4, char *port, char *username, char *password);
void disconnectFromServer();
bool saveLogs();

enum SmartButtonState {
    None = 0,
    Hovered,
    Pressed,
    Released,
};

static SmartButtonState SmartButton(const char* label);

struct Connection {
    std::shared_ptr<Client> client;
    std::string buttonName;
    ImVec4 col;

    explicit Connection(std::shared_ptr<Client> iclient) : client(std::move(iclient)), col(buttonNotActiveCol){}
};

std::vector<Connection> connections;

int main(int argc, char **argv) {
    // Creates the application window
    WNDCLASSEXW wc = {
            sizeof(wc),
            CS_CLASSDC,
            WndProc,
            0L,
            0L,
            GetModuleHandle(nullptr),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            L"DT-Client",
            nullptr
    };
    ::RegisterClassExW((&wc));
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"DT-Client", WS_OVERLAPPEDWINDOW, 100, 100,
                                1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Showing the window
    ::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    ::UpdateWindow(hwnd);

    // Setting up ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enables the keyboard

    ImGui::StyleColorsDark();

    // Setting up the platform/renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    imgui_theme();
    set_font(io);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // main loop
    bool done = false;

    for(int i = 0; i < 3; i++) {
        connectToServer("127.0.0.1", "27015", "Floyd", "password");
    }

    while(!done) {
        MSG msg;
        while(::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if(msg.message == WM_QUIT)
                done = true;
        }

        if(done)
            break;

        // Handle window being minimised or screen locked
        if(SwapChainOccluded && SwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
            ::Sleep(10);
            continue;
        }

        SwapChainOccluded = false;

        // Handle resizing
        if(resizeWidth != 0 && resizeHeight != 0) {
            CleanupRenderTarget();
            SwapChain->ResizeBuffers(0, resizeWidth, resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            resizeWidth = resizeHeight = 0;
            CreateRenderTarget();
        }

        // Creating the frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        renderGUI(&done);

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w * clear_color.w};
        context->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
        context->ClearRenderTargetView(mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = SwapChain->Present(1, 0);
        SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
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

bool CreateDeviceD3D(HWND hwnd) {
    // Setup swap chain
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
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            createDeviceFlags,
            featureLevelArray,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &SwapChain,
            &device,
            &featureLevel,
            &context
            );

    if(res == DXGI_ERROR_UNSUPPORTED) { // Trying high-performance WARP driver if hardware is not available
        res = D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_WARP,
                nullptr,
                createDeviceFlags,
                featureLevelArray,
                2,
                D3D11_SDK_VERSION,
                &sd,
                &SwapChain,
                &device,
                &featureLevel,
                &context
        );
    }

    if(res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if(SwapChain) { SwapChain->Release(); SwapChain = nullptr; }
    if(context) { context->Release(); context = nullptr; }
    if(device) { device->Release(); device = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D *pBackBuffer = nullptr;
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if(mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch(msg) {
        case WM_SIZE:
            if(wparam == SIZE_MINIMIZED)
                return 0;
            resizeWidth = (UINT) LOWORD(lparam); // Queue resize
            resizeHeight = (UINT) HIWORD(lparam);
            return 0;

        case WM_SYSCOMMAND:
            if((wparam & 0xfff0) == SC_KEYMENU) // Disable alt application menu
                return 0;
            break;

        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        default:
            break;
    }

    return ::DefWindowProcW(hwnd, msg, wparam, lparam);
}

bool displayErrorText = false;

static int selected = 0;

void renderGUI(bool *done) {
    static bool show_about_window = false;
    static bool show_instruction_window = false;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("DT-Client", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

    ImGui::PushFont(ifonts.default_font);

    // Menu Bar
    if(ImGui::BeginMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Save logs")) { saveLogs(); }
            if(ImGui::MenuItem("Close")) { *done = true; }
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) { show_about_window = true; }
            if (ImGui::MenuItem("Instruction")) { show_instruction_window = true; }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (show_about_window) {
        ImGui::Begin("About", &show_about_window);
        ImGui::Text("A simple remote access tool client.");
        ImGui::End();
    }

    if(show_instruction_window) {
        ImGui::SetNextWindowSize(ImVec2(500, 200));
        ImGui::Begin("Instruction", &show_instruction_window, ImGuiWindowFlags_NoResize);
        ImGui::PushTextWrapPos(450.0f);
        ImGui::TextUnformatted("Type the information of the server(ip, port) into the boxes, as well as your user account name and password. Then click connect. Afterwards the status should say connected and you are free to type in commands into the message box. For more information about the possible commands, please have a look at the 'README.md'.");
        ImGui::PopTextWrapPos();
        ImGui::End();
    }

    // Heading
    verticalSpacing(2);
    displayTextHeading("DT-Client");
    verticalSpacing(2);
    ImGui::Separator();
    verticalSpacing(2);

    static char ipv4[16];
    displayInputLine("Server Address: ", ipv4, "###addr", IM_ARRAYSIZE(ipv4), 0);
    static char port[6];
    displayInputLine("Server Port:    ", port, "###port", IM_ARRAYSIZE(port), 0);
    static char username[32];
    displayInputLine("Username:       ", username, "###username", IM_ARRAYSIZE(username), 0);
    static char password[32];
    displayInputLine("Password:       ", password, "###pw", IM_ARRAYSIZE(password), ImGuiInputTextFlags_Password);

    if(ImGui::Button("Connect")) {
        if(!checkInputs(ipv4, port, username, password)) {
            displayErrorText = true;
        } else {
            displayErrorText = false;
            connectToServer(ipv4, port, username, password);
        }
    }

    if(displayErrorText) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[E] Input error");
    }
    verticalSpacing(2);
    ImGui::Separator();
    verticalSpacing(2);

    for(int i = 0; i < connections.size(); i++) {
        ImGui::PushStyleColor(ImGuiCol_Button, connections[i].col);

        auto state = SmartButton(connections[i].buttonName.c_str());
        if(state == SmartButtonState::Pressed) {
            connections[i].col = buttonActiveCol;
            for(int j = 0; j < connections.size(); j++) {
                if(i != j) {
                    connections[j].col = buttonNotActiveCol;
                }
            }
            selected = i;
        }

        ImGui::PopStyleColor();

        if(i != connections.size() - 1)
            ImGui::SameLine();
    }

    ImGui::PopFont();
    ImGui::End();
}

bool saveLogs() {
    // TODO: Needs to be finished
    return true;
}

bool isValidPort(const char *port) {
    int portNum = atoi(port);
    if (portNum >= 1024 && portNum <= 65535) {
        return true;
    }
    return false;
}

bool isValidIpv4(const char *ipv4) {
    struct sockaddr_in sa = {};
    int result = inet_pton(AF_INET, ipv4, &(sa.sin_addr));
    return result != 0;
}

bool checkInputs(char *ipv4, char *port, const char *username, const char *password) {
    return isValidIpv4(ipv4) && isValidPort(port) && username[0] != '\0' && password[0] != '\0';
}

void connectToServer(char *ipv4, char *port, char *username, char *password) {
    auto newClient = std::make_shared<Client>(ipv4, port, username, password);
    Connection conn(newClient);
    connections.push_back(conn);
    int pos = (int) connections.size() - 1;
    connections[pos].buttonName = getButtonSelectionName(pos);
}

// TODO: Finish and redo all connection names
void disconnectFromServer() {

}

static SmartButtonState SmartButton(const char* label) {
    if(ImGui::Button(label)) return SmartButtonState::Pressed;
    if(ImGui::IsItemActive() && !ImGui::IsItemHovered()) return SmartButtonState::Pressed;
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) return SmartButtonState::Pressed;
    if(ImGui::IsItemHovered()) return SmartButtonState::Hovered;
    if(ImGui::IsItemDeactivated()) return SmartButtonState::Released;

    return SmartButtonState::None;
}

std::string getButtonSelectionName(int connPos) {
    std::string name = std::format("{}##{}", connections[connPos].client->getIP(), connPos);
    //std::string name = std::to_string(connPos);
    return name;
}