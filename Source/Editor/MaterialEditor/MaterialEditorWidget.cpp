#include "MaterialEditorWidget.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <Engine/Application/Window.h>
#include <Engine/FileSystem/FileSystem.h>
#include <EditorCore/editor.h>
#include <Viewport/ViewportWidget.h>
#include <Engine/Game/GameWorld.h>
#include <Engine/Application/Application.h>
#include <Engine/Application/Event.h>
#include <MaterialEditor/MaterialParameterWidget.h>
#include <MaterialEditor/MaterialTemplate.h>

namespace volucris
{
    MaterialEditorWidget::MaterialEditorWidget()
        : m_viewport(std::make_shared<ViewportWidget>())
        , m_world(std::make_shared<GameWorld>())
        , m_material(nullptr)
        , m_parameterWidget(std::make_shared<MaterialParameterWidget>())
    {
        addChild(m_viewport);
        addChild(m_parameterWidget);
        m_viewport->setWorld(m_world);
    }

    void MaterialEditorWidget::setMaterial(const std::shared_ptr<MaterialTemplate>& material)
    {
        m_material = material;
        m_viewport->setTestMaterial(material);
        m_parameterWidget->setMaterial(material);
    }

    void MaterialEditorWidget::onBuild(bool)
	{
        // 创建主窗口（包含DockSpace和菜单栏）
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // 窗口样式设置（无边框、无标题栏）
        static ImGuiWindowFlags window_flags =
            ImGuiDockNodeFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        // 开始主窗口
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MaterialEditorWindow", nullptr, window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace = ImGui::GetID("MaterialEditorWindow");
        ImGui::DockSpace(dockspace);

        bool init = ImGui::GetFrameCount() == 1 && !hasCustomLayout("MaterialEditorWindow");
        if (init)
		{
            ImGui::DockBuilderRemoveNode(dockspace); // 清除现有布局（如果有）
            ImGui::DockBuilderAddNode(dockspace, ImGuiDockNodeFlags_DockSpace); // 添加新的 DockSpace

            // 将 DockSpace 拆分为左右两部分
            ImGuiID left_id, right_id;
            ImGui::DockBuilderSplitNode(
                dockspace,
                ImGuiDir_Right,    // 方向：左侧
                .8f,             // 左侧占 50%
                &right_id,         // 左侧 DockNode ID
                &left_id         // 右侧 DockNode ID
            );

            ImGui::DockBuilderSetNodeSize(left_id, {600,100});

            // 将窗口绑定到 DockNode
            ImGui::DockBuilderDockWindow("Preview", left_id);
            ImGui::DockBuilderDockWindow("ParameterWidget", right_id);

            // 完成布局设置
            ImGui::DockBuilderFinish(dockspace);
		}

        static ImGuiWindowClass no_title_class;
        no_title_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton |
            ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_HiddenTabBar |
            ImGuiDockNodeFlags_NoWindowMenuButton;

        // 对每个窗口
        ImGui::SetNextWindowClass(&no_title_class);

        m_viewport->build();

        ImGui::SetNextWindowClass(&no_title_class);
        m_parameterWidget->build();

		ImGui::End();
	}

    void MaterialEditorWidget::onWindowFocusChanged(FocusEvent* event)
    {
        if (event->focused)
        {
            gApp->addGame(m_world);
        }
        else
        {
            gApp->removeGame(m_world);
        }
    }

} // namespace volucris