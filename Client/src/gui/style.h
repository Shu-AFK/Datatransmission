#ifndef DATATRANSMISSION_STYLE_H
#define DATATRANSMISSION_STYLE_H

#include "../../../include/imgui/imgui.h"

#include <string>
#include <optional>
#include <filesystem>

struct fonts {
    ImFont *default_font;
    ImFont *heading_font;
};

extern ImVec4 buttonActiveCol;
extern ImVec4 buttonNotActiveCol;

extern ImVec4 textColor;
extern ImVec4 errorTextColor;

extern fonts ifonts;

std::optional<std::filesystem::path> find_path(const std::string& target_directory);
void imgui_theme();
void set_font(ImGuiIO &io);
void displayTextHeading(const std::string &str);
bool displayInputLine(const std::string &text, char *buffer, const std::string &id, size_t size, ImGuiInputTextFlags flags);
void verticalSpacing(size_t n);
void displayText(const std::string &buffer);

#endif
