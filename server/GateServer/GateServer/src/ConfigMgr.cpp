#include "ConfigMgr.h"

ConfigMgr::ConfigMgr()
{
	// 读取config.ini配置
    // 获取当前工作目录  
    boost::filesystem::path current_path = boost::filesystem::current_path();
    // 构建config.ini文件的完整路径  
    boost::filesystem::path config_path = current_path / "config.ini";
    std::cout << "open config file: " << config_path << std::endl;

    // boost树形读取ini文件
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);

    // 遍历ini中所有section
    for (const auto& section_pair : pt) {
        const std::string& section_name = section_pair.first;
        const boost::property_tree::ptree& section_tree = section_pair.second;

        // 对于每个section，遍历其所有key-value对
        SectionInfo sectionInfo;
        std::map<std::string, std::string>& section_config = sectionInfo._section_datas;
        for (const auto& key_value : section_tree) {
            const std::string& key = key_value.first;
            const std::string& value = key_value.second.get_value<std::string>();
            section_config[key] = value;
        }
        // 将section的key-value对保存到config_map中  
        _config_map[section_name] = sectionInfo;
    }

    // 输出所有的section和key-value对  
    for (const auto& section_entry : _config_map) {
        const std::string& section_name = section_entry.first;
        SectionInfo section_config = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto& key_value : section_config._section_datas) {
            std::cout << key_value.first << "=" << key_value.second << std::endl;
        }
    }
}
