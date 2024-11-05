#include "style.h"

#include <iostream>

fonts ifonts = {nullptr, nullptr};

ImVec4 buttonActiveCol;
ImVec4 buttonNotActiveCol;

ImVec4 errorTextColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 textColor = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

std::optional<std::filesystem::path> find_path(std::filesystem::path start_path, const std::string& target_directory) {
    start_path = std::filesystem::absolute(start_path);

    while (start_path != start_path.root_path()) {
        std::filesystem::path append_path = start_path / target_directory;
        if (std::filesystem::exists(append_path)) {
            return append_path;
        }
        start_path = start_path.parent_path();
    }

    try {
        for (const auto& directory_entry : std::filesystem::recursive_directory_iterator(start_path)) {
            if (std::filesystem::exists(directory_entry.path() / target_directory)) {
                return directory_entry.path() / target_directory;
            }
        }
    } catch(const std::filesystem::filesystem_error& ex) {
        std::cerr << "Exception: " << ex.what() << "\n";
    }

    return std::nullopt;
}

void imgui_theme() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = textColor;
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding                     = ImVec2(8.00f, 8.00f);
    style.FramePadding                      = ImVec2(5.00f, 2.00f);
    style.CellPadding                       = ImVec2(6.00f, 6.00f);
    style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
    style.IndentSpacing                     = 25;
    style.ScrollbarSize                     = 15;
    style.GrabMinSize                       = 10;
    style.WindowBorderSize                  = 1;
    style.ChildBorderSize                   = 1;
    style.PopupBorderSize                   = 1;
    style.FrameBorderSize                   = 1;
    style.TabBorderSize                     = 1;
    style.WindowRounding                    = 7;
    style.ChildRounding                     = 4;
    style.FrameRounding                     = 3;
    style.PopupRounding                     = 4;
    style.ScrollbarRounding                 = 9;
    style.GrabRounding                      = 3;
    style.LogSliderDeadzone                 = 4;
    style.TabRounding                       = 4;

    buttonActiveCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    buttonNotActiveCol = ImGui::GetStyle().Colors[ImGuiCol_Button];
}

void set_font(ImGuiIO &io) {
    ImFont *defaultFont;
    ImFont *headingFont;

    if(auto p = find_path(std::filesystem::current_path(), "assets")) {
        std::string font_path = (p.value() / "fonts/JetBrainsMono-Medium.ttf").string();
        defaultFont = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f);
        headingFont = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 24.0f);
    } else {
        std::cout << "[E] Can't find font.. using default font" << std::endl;
        ImFontConfig config16;
        config16.SizePixels = 16;
        defaultFont = io.Fonts->AddFontDefault(&config16);

        ImFontConfig config24;
        config24.SizePixels = 24;
        headingFont = io.Fonts->AddFontDefault(&config24);
    }

    ifonts.default_font = defaultFont;
    ifonts.heading_font = headingFont;
}

void displayTextHeading(const std::string &str) {
    ImGui::PopFont();
    ImGui::PushFont(ifonts.heading_font);
    ImGui::Text(str.c_str());
    ImGui::PopFont();
    ImGui::PushFont(ifonts.default_font);
}

bool displayInputLine(const std::string &text, char *buffer, const std::string &id, size_t size, ImGuiInputTextFlags flags) {
    ImGui::Text(text.c_str());
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3);
    bool returned = ImGui::InputText(id.c_str(), buffer, size, flags);
    ImGui::PopItemWidth();

    return returned;
}

void verticalSpacing(size_t n) {
    for (size_t i = 0; i < n; i++) {
        ImGui::Spacing();
    }
}

// TODO: Make the function work with \t and \n like it did before when error
void displayText(const std::string &buffer) {
    size_t errorPos = buffer.find("Error");
    size_t failedPos = buffer.find("Failed");

    if (errorPos == std::string::npos && failedPos == std::string::npos) {
        ImGui::Text(buffer.c_str());
        return;
    }

    size_t minPos = std::min(
            (errorPos != std::string::npos ? errorPos : buffer.size()),
            (failedPos != std::string::npos ? failedPos : buffer.size())
    );

    std::string noErrorString = buffer.substr(0, minPos);
    ImGui::Text(noErrorString.c_str());

    std::string restOfBuffer = buffer.substr(minPos);
    size_t endOfErrorText = restOfBuffer.find('\n');

    // Ensure endOfErrorText is within bounds
    if (endOfErrorText != std::string::npos) {
        ImGui::PushStyleColor(ImGuiCol_Text, errorTextColor);
        ImGui::Text(restOfBuffer.substr(0, endOfErrorText).c_str());
        ImGui::PopStyleColor();

        displayText(restOfBuffer.substr(endOfErrorText + 1));
    } else {
        // Handle the case where the error text is the last line
        ImGui::PushStyleColor(ImGuiCol_Text, errorTextColor);
        ImGui::Text(restOfBuffer.c_str());
        ImGui::PopStyleColor();
    }
}