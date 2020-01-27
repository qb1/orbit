#include "utils.h"

#include <imgui.h>

bool imgui_select_object(const std::string name, std::optional<SimUniverse::Collection::const_iterator>& selected,
                         const ObjectLinkCollection& object_links,
                         std::optional<SimUniverse::Collection::const_iterator> ignore)
{
	bool selection_changed = false;
	const char* current = selected ? selected.value()->first.c_str() : nullptr;
	if (ImGui::BeginCombo(name.c_str(), current)) {
		for (const auto& obj : object_links) {
			if (obj.simu_ == ignore)
				continue;

			bool checked = false;
			if (selected and selected.value() == obj.simu_) {
				checked = true;
			}
			bool was_checked = checked;
			ImGui::Selectable(obj.name().c_str(), &checked);

			if (was_checked != checked) {
				selection_changed = true;
				if (checked) {
					selected = obj.simu_;
				} else {
					selected = std::nullopt;
				}
			}
		}
		ImGui::EndCombo();
	}

	return selection_changed;
}