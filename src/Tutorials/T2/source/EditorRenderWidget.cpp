#include "EditorRenderWidget.h"

#include <iostream>
#include <Core/Base/macro.h>
#include <Engine.h>
#include <Function/Render/Interface/Renderer.h>
#include <Function/Render/Interface/Shader.h>
#include <Function/Event/EventSystem.h>
#include <Function/Scene/EditCamera.h>
#include <Function/Scene/Scene.h>
#include <qelapsedtimer.h>
#include <qevent.h>
#include <QtImGui.h>
#include <imgui.h>
#include <ImGuizmo.h>
#include <Function/Scene/Light.h>
#include <Function/Scene/Material.h>
#include <glm/gtc/type_ptr.hpp>

#include <Resource/Components/Transform.h>
#include <GdalScene/TiffMesh.h>
//#include "test.h"
namespace Stone
{
    TiffMesh* testmesh;
    TransformComponent* transformcomponent;
	EditorRendererWidget::EditorRendererWidget(QWidget* parent)
		: QOpenGLWidget(parent), m_MousePos(std::make_shared<MousePos>(0.0f, 0.0f)), m_MouseAngle(std::make_shared<MouseAngle>(0.0f, 0.0f))
	{}
	EditorRendererWidget::~EditorRendererWidget()
	{
    }

	void EditorRendererWidget::initializeGL()
	{
        PublicSingleton<Engine>::getInstance().renderInitialize();
        PublicSingleton<Engine>::getInstance().logicalInitialize();
        QtImGui::initialize(this);

        std::string filename = "../../../../data/drycreek.tif";
        testmesh = new TiffMesh(filename);
        transformcomponent = new TransformComponent();
        transformcomponent->Scale = { 0.001, 0.001, 0.001 };
	}

	void EditorRendererWidget::resizeGL(int w, int h)
	{
        PublicSingleton<Renderer>::getInstance().screenFrameBuffer()->resize(w, h);
        PublicSingleton<EditorCamera>::getInstance().setViewportSize(w, h);
	}

	void EditorRendererWidget::paintGL()
	{
        QElapsedTimer timer;
        timer.start();
        PublicSingleton<Engine>::getInstance().logicalTick();
        PublicSingleton<Renderer>::getInstance().begin();
        
        PublicSingleton<EditorCamera>::getInstance().bind();
        PublicSingletonInstance(GLobalLight).bind(1);
        PublicSingletonInstance(MaterialPool).getMaterial("BasicMaterial")->bind(2);
        //PublicSingleton<ShaderPool>::getInstance().get("MeshShader")->bind();
        PublicSingleton<ShaderPool>::getInstance().get(m_Shader)->bind();
        //PublicSingleton<ShaderPool>::getInstance().get(m_Shader)->bind();
        //_texture->bind(0);
        transformcomponent->bind(2);

        QtImGui::newFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        int x, y, width, height;
        const QRect& geom = this->geometry();
        x = geom.x();
        y = geom.y();
        width = geom.width();
        height = geom.height();
        ImGuizmo::SetRect(x, y, width, height);
        auto& view = PublicSingletonInstance(EditorCamera).getViewMatrix();
        auto& proj = PublicSingletonInstance(EditorCamera).getProjMatrix();
        glm::mat4 testMatrix = glm::mat4(1);
        ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(proj), glm::value_ptr(testMatrix), 100.f);
        ImGui::Render();
        QtImGui::render();

        PublicSingleton<Renderer>::getInstance().render(testmesh);

        QtImGui::newFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::ViewManipulate(glm::value_ptr(view), 8.f, ImVec2(x + width - width * 0.1, 0), ImVec2(width * 0.1, width * 0.1), 0x10101010);
        PublicSingletonInstance(EditorCamera).updateBuffer();
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(testMatrix));
        ImGui::Text("Hello");
        float aaa = 0.0f;
        ImGui::DragFloat3("Light Pos", (float*)&(PublicSingletonInstance(GLobalLight).m_BlockData.Position));
        PublicSingletonInstance(GLobalLight).updateBuffer();
        ImGui::Render();
        QtImGui::render();

        PublicSingleton<Renderer>::getInstance().end(defaultFramebufferObject());
        update();
        PublicSingleton<Engine>::getInstance().DeltaTime = timer.nsecsElapsed()* 1.0e-9f;
	}


    void EditorRendererWidget::mousePressEvent(QMouseEvent* event)
    {
        QKeyEvent* e = (QKeyEvent*)event;
        if (e->modifiers() == Qt::ShiftModifier)
        {
            m_MousePos->x = event->pos().x();
            m_MousePos->y = -event->pos().y();
            PublicSingletonInstance(EventSystem).sendEvent("EditCamera_Begin", (void*)m_MousePos.get());
        }
    }

    void EditorRendererWidget::mouseDoubleClickEvent(QMouseEvent* event)
    {
    }

    void EditorRendererWidget::mouseMoveEvent(QMouseEvent* event)
    {
        QKeyEvent* e = (QKeyEvent*)event;
        if (e->modifiers() == Qt::ShiftModifier)
        {
            m_MousePos->x = -event->pos().x();
            m_MousePos->y = event->pos().y();
            if (event->buttons() & Qt::LeftButton)
            {

                PublicSingletonInstance(EventSystem).sendEvent("EditCamera_Rotate", (void*)m_MousePos.get());
            }
            else if (event->buttons() & Qt::MiddleButton)
            {

                PublicSingletonInstance(EventSystem).sendEvent("EditCamera_Pan", (void*)m_MousePos.get());
            }
        }
        
    }

    void EditorRendererWidget::mouseReleaseEvent(QMouseEvent* event)
    {
        QKeyEvent* e = (QKeyEvent*)event;
        if (e->modifiers() == Qt::ShiftModifier)
        {
            PublicSingletonInstance(EventSystem).sendEvent("EditCamera_End", (void*)m_MousePos.get());
        }
    }

    void EditorRendererWidget::wheelEvent(QWheelEvent* event)
    {
        m_MouseAngle->x = event->angleDelta().x();
        m_MouseAngle->y = event->angleDelta().y();
        PublicSingletonInstance(EventSystem).sendEvent("EditCamera_Zoom", (void*)m_MouseAngle.get());
    }

    void EditorRendererWidget::renderImGui()
    {
        QtImGui::newFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        int x, y, width, height;
        const QRect& geom = this->geometry();
        x = geom.x();
        y = geom.y();
        width = geom.width();
        height = geom.height();
        ImGuizmo::SetRect(x, y, width, height);
        auto& view = PublicSingletonInstance(EditorCamera).getViewMatrix();
        auto& proj = PublicSingletonInstance(EditorCamera).getProjMatrix();
        glm::mat4 testMatrix = glm::mat4(1);
        ImGuizmo::ViewManipulate(glm::value_ptr(view), 8.f, ImVec2(x + width - width * 0.1, 0), ImVec2(width * 0.1, width * 0.1), 0x10101010);
        PublicSingletonInstance(EditorCamera).updateBuffer();
        ImGuizmo::DrawGrid(glm::value_ptr(view), glm::value_ptr(proj), glm::value_ptr(testMatrix), 100.f);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(testMatrix));
        ImGui::Text("Hello");
        float aaa = 0.0f;
        ImGui::DragFloat3("Light Pos", (float*)&(PublicSingletonInstance(GLobalLight).m_BlockData.Position));
        PublicSingletonInstance(GLobalLight).updateBuffer();
        ImGui::Render();
        QtImGui::render();
    }
    
}