#include "ui/views/SceneView.hpp"
#include "asset/SceneBuilder.hpp"

// 场景视图实现文件
// 实现了场景设置界面的功能

namespace NRenderer
{
    // 构造函数
    // 初始化场景视图
    SceneView::SceneView(const Vec2& position, const Vec2& size, UIContext& uiContext, Manager& manager)
        : View                  (position, size, uiContext, manager)  // 调用基类构造函数
        , currComponentSelected (-1)                                  // 初始化当前选中组件为-1
    {
    }

    // 开始绘制场景设置窗口
    void SceneView::drawBeginWindow() {
        ImGui::Begin("Render Settings", nullptr, windowFlag);  // 创建渲染设置窗口
    }

    // 绘制场景设置界面
    void SceneView::draw() {
        cameraSetting();     // 相机设置
        ImGui::Separator();  // 分隔线
        renderSetting();     // 渲染设置
        ImGui::Separator();  // 分隔线
        ambientSetting();    // 环境光设置
        ImGui::Separator();  // 分隔线
        componentSetting();  // 组件设置
    }

    // 相机设置界面
    void SceneView::cameraSetting() {
        auto& camera = manager.renderSettingsManager.camera;
        ImGui::TextUnformatted("Camera:");
        float floatStep = 0.1;  // 浮点数调整步长

        // 相机参数输入
        ImGui::InputFloat3("Position", &camera.position.x);        // 相机位置
        ImGui::InputFloat3("Up", &camera.up.x);                   // 上方向
        ImGui::InputFloat3("LookAt", &camera.lookAt.x);           // 观察点
        ImGui::InputScalar("Fov", ImGuiDataType_Float, &camera.fov, &floatStep, NULL);                  // 视场角
        ImGui::InputScalar("Aspect", ImGuiDataType_Float, &camera.aspect, &floatStep, NULL);            // 宽高比
        ImGui::InputScalar("Aperture", ImGuiDataType_Float, &camera.aperture, &floatStep, NULL);        // 光圈大小
        ImGui::InputScalar("FocusDistance", ImGuiDataType_Float, &camera.focusDistance, &floatStep, NULL);  // 焦距
    }

    // 渲染设置界面
    void SceneView::renderSetting() {
        int intStep = 1;  // 整数调整步长
        auto& rs = manager.renderSettingsManager.renderSettings;
        ImGui::TextUnformatted("Render Settings:");

        // 渲染参数输入
        ImGui::InputScalar("Width", ImGuiDataType_U32, &rs.width, &intStep, NULL, "%u");            // 渲染宽度
        ImGui::InputScalar("Height", ImGuiDataType_U32, &rs.height, &intStep, NULL, "%u");          // 渲染高度
        ImGui::InputScalar("Depth", ImGuiDataType_U32, &rs.depth, &intStep, NULL, "%u");            // 光线追踪深度
        ImGui::InputScalar("Sample Nums", ImGuiDataType_U32, &rs.samplesPerPixel, &intStep, NULL, "%u");  // 采样数量
    }

    // 环境光设置界面
    void SceneView::ambientSetting() {
        auto& as = manager.renderSettingsManager.ambientSettings;
        ImGui::TextUnformatted("Ambient:");

        // 环境光类型选择
        const string typeStr[2] = {"Constant", "Environment Map"};
        int curr = as.type == AmbientSettings::Type::CONSTANT ? 0 : 1;
        if(ImGui::BeginCombo("Type##AmbientSettings", typeStr[curr].c_str())) {
            for (int i=0; i<2; i++) {
                bool selected = curr == i;
                if (ImGui::Selectable((typeStr[i]+"##AmbientTypeItem").c_str(), &selected)) {
                    as.type = i == 0 ? AmbientSettings::Type::CONSTANT : AmbientSettings::Type::ENVIROMENT_MAP;
                    curr = i;
                }    
            }
            ImGui::EndCombo();
        }

        // 根据环境光类型显示不同设置
        if (curr == 0) {
            // 常量环境光设置
            ImGui::ColorEdit3("Ambient", &as.ambient.r, ImGuiColorEditFlags_Float);
        }
        else if (curr == 1) {
            // 环境贴图设置
            auto& v = as.mapTexture;
            if (ImGui::BeginCombo("Map Texture##AmbientEMAPTex", v.valid()? manager.assetManager.asset.textureItems[v.index()].name.c_str() : "")) {
                for (int i=0; i < manager.assetManager.asset.textureItems.size(); i++) {
                    bool selected = v.valid() ? (i == v.index()) : false;
                    if (ImGui::Selectable(manager.assetManager.asset.textureItems[i].name.c_str(), &selected)) {
                        v.setIndex(i);
                    }
                }
                ImGui::EndCombo();
            }
        }
    }

    // 渲染组件设置界面
    void SceneView::componentSetting() {
        ImGui::TextUnformatted("Render Component:");

        // 获取可用的渲染组件列表
        auto&& components = getServer().componentFactory.getComponentsInfo("Render");
        auto& componentManager = manager.componentManager;
        auto& state = uiContext.state;

        // 渲染组件选择下拉框
        string comboPre = currComponentSelected != -1 && currComponentSelected < components.size() ?
            components[currComponentSelected].name : "";
        if (ImGui::BeginCombo("Render Component##SceneView", comboPre.c_str())) {
            for(int i=0; i<components.size(); i++) {
                bool selected = i == currComponentSelected;
                if (ImGui::Selectable((to_string(i+1) + ". " + components[i].name + "##SceneView").c_str(), &selected)) {
                    currComponentSelected = i;
                }
            }
            ImGui::EndCombo();
        }

        // 显示选中组件的描述
        if (currComponentSelected != -1 && currComponentSelected < components.size()) {
            ImGui::TextWrapped(components[currComponentSelected].description.c_str());
        }

        // 渲染按钮
        if (ImGui::Button("Render")) {
            if (currComponentSelected != -1 && currComponentSelected < components.size()) {
                // 创建场景并执行渲染
                auto& rs = manager.renderSettingsManager;
                SceneBuilder sceneBuilder{manager.assetManager.asset, rs.renderSettings, rs.ambientSettings, rs.camera};
                componentManager.exec<RenderComponent>(components[currComponentSelected], sceneBuilder.build());
            }
            else {
                getServer().logger.error("No render component is selected!");  // 未选择渲染组件时显示错误
            }
        }
    }

} // namespace NRenderer
