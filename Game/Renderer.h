#pragma once
#include <Corrade/Containers/Array.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData3D.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>

using namespace Magnum;
using namespace Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;


namespace UL {
	class ulRenderer {
		Shaders::Phong m_ColoredShader,
			m_TexturedShader{ Shaders::Phong::Flag::DiffuseTexture };
		Containers::Array<Containers::Optional<GL::Mesh>> m_Meshes;
		Containers::Array<Containers::Optional<GL::Texture2D>> m_Textures;

		Scene3D m_Scene;
		Object3D m_Manipulator, m_CameraObject;
		SceneGraph::Camera3D* m_Camera;
		SceneGraph::DrawableGroup3D m_Drawables;
		
		void addObject(Trade::AbstractImporter& importer, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i);

	public:

		void loadFile(Trade::AbstractImporter* importer, std::string fname);
		void drawEvent();
		void viewportEvent(const Vector2i& frameBufferSize, const Vector2i& windowSize);
		Vector3 positionOnSphere(const Vector2i& position) const;
		void zoom(float ratio);
		void rotateManupulator(Magnum::Rad, const Vector3&);
		ulRenderer();

	};

	class ColoredDrawable : public SceneGraph::Drawable3D {
	public:
		explicit ColoredDrawable(Object3D& object, Shaders::Phong& shader, GL::Mesh& mesh, const Color4& color, SceneGraph::DrawableGroup3D& group) : SceneGraph::Drawable3D{ object, &group }, m_Shader(shader), m_Mesh(mesh), m_Color{ color } {}

	private:
		void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

		Shaders::Phong& m_Shader;
		GL::Mesh& m_Mesh;
		Color4 m_Color;
	};

	class TexturedDrawable : public SceneGraph::Drawable3D {
	public:
		explicit TexturedDrawable(Object3D& object, Shaders::Phong& shader, GL::Mesh& mesh, GL::Texture2D& texture, SceneGraph::DrawableGroup3D& group) : SceneGraph::Drawable3D{ object, &group }, m_Shader(shader), m_Mesh(mesh), m_Texture(texture) {}

	private:
		void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

		Shaders::Phong& m_Shader;
		GL::Mesh& m_Mesh;
		GL::Texture2D& m_Texture;
	};
}