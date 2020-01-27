#pragma once

#include <simulation/universe.h>
#include "object_link.h"

bool imgui_select_object(const std::string name, std::optional<SimUniverse::Collection::const_iterator>& selected,
                         const ObjectLinkCollection& object_links,
                         std::optional<SimUniverse::Collection::const_iterator> ignore);