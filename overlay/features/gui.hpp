#pragma once

#include "../engine/overlay.hpp"

class dx_gui
	: public Singleton<dx_gui> {
public:
	virtual	void present();

private:
	bool m_render = true;
private:
	std::unordered_map< std::string, bool* > m_group_map = { };
};