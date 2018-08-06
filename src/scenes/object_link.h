#pragma once

#include <vector>
#include <optional>

#include <graphics/object.h>
#include <simulation/universe.h>

class ObjectLinkCollection
{
public:
	struct ObjectLink {
		const std::string& name() const { return simu_->first; }
		const SimObject& simu() const { return simu_->second; }
		const GrObject& visu() const { return *visu_; }
		GrObject& visu() { return *visu_; }

		const SimObject* primary_simu() const { return primary_simu_ ? &primary_simu_.value()->second : nullptr; }

		SimUniverse::Collection::const_iterator simu_;
		std::optional<SimUniverse::Collection::const_iterator> primary_simu_;
		GrObject* visu_;
	};

	auto begin() { return object_links_.begin(); }
	auto end() { return object_links_.end(); }

	auto cbegin() const { return object_links_.cbegin(); }
	auto cend() const { return object_links_.cend(); }

	auto rbegin() { return object_links_.rbegin(); }
	auto rend() { return object_links_.rend(); }

	auto crbegin() const { return object_links_.crbegin(); }
	auto crend() const { return object_links_.crend(); }

	void add(ObjectLink object_link) { object_links_.push_back(object_link); }

	ObjectLink& find(SimUniverse::Collection::const_iterator simu)
	{
		for (auto& link : object_links_) {
			if (link.simu_ == simu)
				return link;
		}
		throw std::runtime_error("Cannot find matching ObjectLink for SimObject");
	}

	ObjectLink& find(std::string name)
	{
		for (auto& link : object_links_) {
			if (link.simu_->first == name)
				return link;
		}
		throw std::runtime_error("Cannot find matching ObjectLink for name " + name);
	}

private:
	std::vector<ObjectLink> object_links_;
};
