#include "minimalButton.h"

using namespace ftxui;

Component minimalButton(const std::string& label, std::function<void()> action) {
	ButtonOption option;
	option.transform = [&](const EntryState& entry_state) {
		Element element = text(label);
		element |= entry_state.focused ? color(Color::Green) : color(Color::Default);
		return element;
	};

	return Button(label, action, option);
}
