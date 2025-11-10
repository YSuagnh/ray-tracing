// 材质模板实现文件
// 提供材质模板的静态成员变量定义和初始化

#include "templates/MaterialTemplates.hpp"

using namespace NRenderer;
using namespace std;

// 材质模板映射表，键为材质类型，值为对应的模板
unordered_map<int, MaterialTemplates::Template> MaterialTemplates::templates{};

// 材质模板初始化器，用于在程序启动时初始化所有预定义的材质模板
MaterialTemplates::InitTemplates MaterialTemplates::initTemplates = MaterialTemplates::InitTemplates{MaterialTemplates::templates};