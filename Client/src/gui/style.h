#ifndef DATATRANSMISSION_STYLE_H
#define DATATRANSMISSION_STYLE_H

#include "imgui.h"

#include <string>
#include <optional>
#include <filesystem>

struct fonts {
    ImFont *default_font;
    ImFont *heading_font;
};

extern fonts ifonts;

std::optional<std::filesystem::path> find_path(const std::string& target_directory);
void imgui_theme();
void set_font(ImGuiIO &io);
void displayTextHeading(const std::string &str);
void displayInputLine(const std::string &text, char *buffer, const std::string &id, size_t size, ImGuiInputTextFlags flags);
void verticalSpacing(size_t n);

#endif
