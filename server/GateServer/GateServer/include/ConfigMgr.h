#pragma once

#include "Singleton.h"
#include "const.h"
#include <map>

struct SectionInfo {
public:
	SectionInfo() = default;

	~SectionInfo(){
		_section_datas.clear();
	}

	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this) {
			return *this;
		}
		this->_section_datas = src._section_datas;
		return *this;
	}

	std::string  operator[](const std::string& key) {
		if (_section_datas.count(key) == 0) {
			return "";
		}
		return _section_datas[key];
	}

public:
	std::map<std::string, std::string> _section_datas;
};

class ConfigMgr:public Singleton<ConfigMgr>
{
	friend class Singleton<ConfigMgr>;
public:
	~ConfigMgr() {
		_config_map.clear();
	}

	SectionInfo operator[](const std::string& section) {
		if (_config_map.find(section) == _config_map.end()) {
			return SectionInfo();
		}
		return _config_map[section];
	}

	ConfigMgr& operator=(const ConfigMgr& src) {
		if (&src == this)
			return *this;
		this->_config_map = src._config_map;
	}

	ConfigMgr(const ConfigMgr& src) {
		this->_config_map = src._config_map;
	}

private:
	ConfigMgr();

private:
	// 各项的具体配置
	std::map<std::string, SectionInfo> _config_map;
};

