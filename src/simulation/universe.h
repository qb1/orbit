#pragma once

#include <string>
#include <map>

#include <common/universe_definitions.h>

#include "object.h"

class SimUniverse {
public:
	using Collection = std::map<std::string, SimObject>;

	void add_object(std::string name, SimObject object);
	const Collection& objects() const { return objects_; }

	Collection::iterator find(const std::string& name) { return objects_.find(name); }
	Collection::const_iterator find(const std::string& name) const { return objects_.find(name); }

	void step(double timestep);
	double date() { return date_; }
private:
	Collection objects_;
	double date_;
};

void init_simulation_randomize(SimUniverse& simulation, const UniverseDefinition& definition);