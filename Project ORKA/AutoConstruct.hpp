#pragma once

struct AutoConstruct {
	virtual void create() {};
	virtual void destroy() {};

	AutoConstruct() { create(); };
	AutoConstruct(const AutoConstruct&) = default;
	AutoConstruct(AutoConstruct&&) = default;
	AutoConstruct& operator=(const AutoConstruct&) = default;
	AutoConstruct& operator=(AutoConstruct&&) = default;
	~AutoConstruct() { destroy(); };
};
