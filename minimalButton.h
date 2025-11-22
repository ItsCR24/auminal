#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>

ftxui::Component minimalButton(const std::string& label, std::function<void()> action);
