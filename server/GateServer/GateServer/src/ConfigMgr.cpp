#include "ConfigMgr.h"

ConfigMgr::ConfigMgr()
{
	// ��ȡconfig.ini����
    // ��ȡ��ǰ����Ŀ¼  
    boost::filesystem::path current_path = boost::filesystem::current_path();
    // ����config.ini�ļ�������·��  
    boost::filesystem::path config_path = current_path / "config.ini";
    std::cout << "open config file: " << config_path << std::endl;

    // boost���ζ�ȡini�ļ�
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);

    // ����ini������section
    for (const auto& section_pair : pt) {
        const std::string& section_name = section_pair.first;
        const boost::property_tree::ptree& section_tree = section_pair.second;

        // ����ÿ��section������������key-value��
        SectionInfo sectionInfo;
        std::map<std::string, std::string>& section_config = sectionInfo._section_datas;
        for (const auto& key_value : section_tree) {
            const std::string& key = key_value.first;
            const std::string& value = key_value.second.get_value<std::string>();
            section_config[key] = value;
        }
        // ��section��key-value�Ա��浽config_map��  
        _config_map[section_name] = sectionInfo;
    }

    // ������е�section��key-value��  
    for (const auto& section_entry : _config_map) {
        const std::string& section_name = section_entry.first;
        SectionInfo section_config = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto& key_value : section_config._section_datas) {
            std::cout << key_value.first << "=" << key_value.second << std::endl;
        }
    }
}
