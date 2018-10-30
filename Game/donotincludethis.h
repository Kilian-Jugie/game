/*
	This file is part of Magnum.
	Original authors — credit is appreciated but not required:
		2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 —
			Vladimír Vondruš <mosra@centrum.cz>
	This is free and unencumbered software released into the public domain.
	Anyone is free to copy, modify, publish, use, compile, sell, or distribute
	this software, either in source code form or as a compiled binary, for any
	purpose, commercial or non-commercial, and by any means.
	In jurisdictions that recognize copyright laws, the author or authors of
	this software dedicate any and all copyright interest in the software to
	the public domain. We make this dedication for the benefit of the public
	at large and to the detriment of our heirs and successors. We intend this
	dedication to be an overt act of relinquishment in perpetuity of all
	present and future rights to this software under copyright law.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

namespace Magnum {
	namespace Examples {

		using namespace Math::Literals;

		typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
		typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

		class UnknownLegacy : public Platform::Application {
		public:
			explicit UnknownLegacy(const Arguments& arguments);

		private:
			void drawEvent() override;
			void viewportEvent(ViewportEvent& event) override;
			void mousePressEvent(MouseEvent& event) override;
			void mouseReleaseEvent(MouseEvent& event) override;
			void mouseMoveEvent(MouseMoveEvent& event) override;
			void mouseScrollEvent(MouseScrollEvent& event) override;

			Vector3 positionOnSphere(const Vector2i& position) const;

			void addObject(Trade::AbstractImporter& importer, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i);

			Shaders::Phong m_ColoredShader,
				m_TexturedShader{ Shaders::Phong::Flag::DiffuseTexture };
			Containers::Array<Containers::Optional<GL::Mesh>> m_Meshes;
			Containers::Array<Containers::Optional<GL::Texture2D>> m_Textures;

			Scene3D m_Scene;
			Object3D m_Manipulator, m_CameraObject;
			SceneGraph::Camera3D* m_Camera;
			SceneGraph::DrawableGroup3D m_Drawables;
			Vector3 _previousPosition;
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

		UnknownLegacy::UnknownLegacy(const Arguments& arguments) :
			Platform::Application{ arguments, Configuration{}
				.setTitle("Magnum Viewer Example")
				.setWindowFlags(Configuration::WindowFlag::Resizable) }
		{
			Utility::Arguments args;
			args.addArgument("file").setHelp("file", "file to load")
				.addOption("importer", "AnySceneImporter").setHelp("importer", "importer plugin to use")
				.addSkippedPrefix("magnum").setHelp("engine-specific options")
				.setHelp("Displays a 3D scene file provided on command line.")
				.parse(arguments.argc, arguments.argv);

			/* Every scene needs a camera */
			m_CameraObject
				.setParent(&m_Scene)
				.translate(Vector3::zAxis(5.0f));
			(*(m_Camera = new SceneGraph::Camera3D{ m_CameraObject }))
				.setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
				.setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
				.setViewport(GL::defaultFramebuffer.viewport().size());

			/* Base object, parent of all (for easy manipulation) */
			m_Manipulator.setParent(&m_Scene);

			/* Setup renderer and shader defaults */
			GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
			GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
			m_ColoredShader
				.setAmbientColor(0x111111_rgbf)
				.setSpecularColor(0xffffff_rgbf)
				.setShininess(80.0f);
			m_TexturedShader
				.setAmbientColor(0x111111_rgbf)
				.setSpecularColor(0x111111_rgbf)
				.setShininess(80.0f);

			/* Load a scene importer plugin */
			PluginManager::Manager<Trade::AbstractImporter> manager;
			std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate(args.value("importer"));
			if (!importer) std::exit(1);

			Debug{} << "Opening file" << args.value("file");

			/* Load file */
			if (!importer->openFile(args.value("file")))
				std::exit(4);

			/* Load all textures. Textures that fail to load will be NullOpt. */
			m_Textures = Containers::Array<Containers::Optional<GL::Texture2D>>{ importer->textureCount() };
			for (UnsignedInt i = 0; i != importer->textureCount(); ++i) {
				Debug{} << "Importing texture" << i << importer->textureName(i);

				Containers::Optional<Trade::TextureData> textureData = importer->texture(i);
				if (!textureData || textureData->type() != Trade::TextureData::Type::Texture2D) {
					Warning{} << "Cannot load texture properties, skipping";
					continue;
				}

				Debug{} << "Importing image" << textureData->image() << importer->image2DName(textureData->image());

				Containers::Optional<Trade::ImageData2D> imageData = importer->image2D(textureData->image());
				GL::TextureFormat format;
				if (imageData && imageData->format() == PixelFormat::RGB8Unorm)
					format = GL::TextureFormat::RGB8;
				else if (imageData && imageData->format() == PixelFormat::RGBA8Unorm)
					format = GL::TextureFormat::RGBA8;
				else {
					Warning{} << "Cannot load texture image, skipping";
					continue;
				}

				/* Configure the texture */
				GL::Texture2D texture;
				texture
					.setMagnificationFilter(textureData->magnificationFilter())
					.setMinificationFilter(textureData->minificationFilter(), textureData->mipmapFilter())
					.setWrapping(textureData->wrapping().xy())
					.setStorage(Math::log2(imageData->size().max()) + 1, format, imageData->size())
					.setSubImage(0, {}, *imageData)
					.generateMipmap();

				m_Textures[i] = std::move(texture);
			}

			/* Load all materials. Materials that fail to load will be NullOpt. The
			   data will be stored directly in objects later, so save them only
			   temporarily. */
			Containers::Array<Containers::Optional<Trade::PhongMaterialData>> materials{ importer->materialCount() };
			for (UnsignedInt i = 0; i != importer->materialCount(); ++i) {
				Debug{} << "Importing material" << i << importer->materialName(i);

				std::unique_ptr<Trade::AbstractMaterialData> materialData = importer->material(i);
				if (!materialData || materialData->type() != Trade::MaterialType::Phong) {
					Warning{} << "Cannot load material, skipping";
					continue;
				}

				materials[i] = std::move(static_cast<Trade::PhongMaterialData&>(*materialData));
			}

			/* Load all meshes. Meshes that fail to load will be NullOpt. */
			m_Meshes = Containers::Array<Containers::Optional<GL::Mesh>>{ importer->mesh3DCount() };
			for (UnsignedInt i = 0; i != importer->mesh3DCount(); ++i) {
				Debug{} << "Importing mesh" << i << importer->mesh3DName(i);

				Containers::Optional<Trade::MeshData3D> meshData = importer->mesh3D(i);
				if (!meshData || !meshData->hasNormals() || meshData->primitive() != MeshPrimitive::Triangles) {
					Warning{} << "Cannot load the mesh, skipping";
					continue;
				}

				/* Compile the mesh */
				m_Meshes[i] = MeshTools::compile(*meshData);
			}

			/* Load the scene */
			if (importer->defaultScene() != -1) {
				Debug{} << "Adding default scene" << importer->sceneName(importer->defaultScene());

				Containers::Optional<Trade::SceneData> sceneData = importer->scene(importer->defaultScene());
				if (!sceneData) {
					Error{} << "Cannot load scene, exiting";
					return;
				}

				/* Recursively add all children */
				for (UnsignedInt objectId : sceneData->children3D())
					addObject(*importer, materials, m_Manipulator, objectId);

				/* The format has no scene support, display just the first loaded mesh with
				   a default material and be done with it */
			}
			else if (!m_Meshes.empty() && m_Meshes[0])
				new ColoredDrawable{ m_Manipulator, m_ColoredShader, *m_Meshes[0], 0xffffff_rgbf, m_Drawables };
		}

		void UnknownLegacy::addObject(Trade::AbstractImporter& importer, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i) {
			Debug{} << "Importing object" << i << importer.object3DName(i);
			std::unique_ptr<Trade::ObjectData3D> objectData = importer.object3D(i);
			if (!objectData) {
				Error{} << "Cannot import object, skipping";
				return;
			}

			/* Add the object to the scene and set its transformation */
			auto* object = new Object3D{ &parent };
			object->setTransformation(objectData->transformation());

			/* Add a drawable if the object has a mesh and the mesh is loaded */
			if (objectData->instanceType() == Trade::ObjectInstanceType3D::Mesh && objectData->instance() != -1 && m_Meshes[objectData->instance()]) {
				const Int materialId = static_cast<Trade::MeshObjectData3D*>(objectData.get())->material();

				/* Material not available / not loaded, use a default material */
				if (materialId == -1 || !materials[materialId]) {
					new ColoredDrawable{ *object, m_ColoredShader, *m_Meshes[objectData->instance()], 0xffffff_rgbf, m_Drawables };

					/* Textured material. If the texture failed to load, again just use a
					   default colored material. */
				}
				else if (materials[materialId]->flags() & Trade::PhongMaterialData::Flag::DiffuseTexture) {
					Containers::Optional<GL::Texture2D>& texture = m_Textures[materials[materialId]->diffuseTexture()];
					if (texture)
						new TexturedDrawable{ *object, m_TexturedShader, *m_Meshes[objectData->instance()], *texture, m_Drawables };
					else
						new ColoredDrawable{ *object, m_ColoredShader, *m_Meshes[objectData->instance()], 0xffffff_rgbf, m_Drawables };

					/* Color-only material */
				}
				else {
					new ColoredDrawable{ *object, m_ColoredShader, *m_Meshes[objectData->instance()], materials[materialId]->diffuseColor(), m_Drawables };
				}
			}

			/* Recursively add children */
			for (std::size_t id : objectData->children())
				addObject(importer, materials, *object, id);
		}

		void ColoredDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
			m_Shader
				.setDiffuseColor(m_Color)
				.setLightPosition(camera.cameraMatrix().transformPoint({ -3.0f, 10.0f, 10.0f }))
				.setTransformationMatrix(transformationMatrix)
				.setNormalMatrix(transformationMatrix.rotationScaling())
				.setProjectionMatrix(camera.projectionMatrix());

			m_Mesh.draw(m_Shader);
		}

		void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) {
			m_Shader
				.setLightPosition(camera.cameraMatrix().transformPoint({ -3.0f, 10.0f, 10.0f }))
				.setTransformationMatrix(transformationMatrix)
				.setNormalMatrix(transformationMatrix.rotationScaling())
				.setProjectionMatrix(camera.projectionMatrix())
				.bindDiffuseTexture(m_Texture);

			m_Mesh.draw(m_Shader);
		}

		void UnknownLegacy::drawEvent() {
			GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

			m_Camera->draw(m_Drawables);

			swapBuffers();
		}

		void UnknownLegacy::viewportEvent(ViewportEvent& event) {
			GL::defaultFramebuffer.setViewport({ {}, event.framebufferSize() });
			m_Camera->setViewport(event.windowSize());
		}

		void UnknownLegacy::mousePressEvent(MouseEvent& event) {
			if (event.button() == MouseEvent::Button::Left)
				_previousPosition = positionOnSphere(event.position());
		}

		void UnknownLegacy::mouseReleaseEvent(MouseEvent& event) {
			if (event.button() == MouseEvent::Button::Left)
				_previousPosition = Vector3();
		}

		void UnknownLegacy::mouseScrollEvent(MouseScrollEvent& event) {
			if (!event.offset().y()) return;

			/* Distance to origin */
			const Float distance = m_CameraObject.transformation().translation().z();

			/* Move 15% of the distance back or forward */
			m_CameraObject.translate(Vector3::zAxis(
				distance*(1.0f - (event.offset().y() > 0 ? 1 / 0.85f : 0.85f))));

			redraw();
		}

		Vector3 UnknownLegacy::positionOnSphere(const Vector2i& position) const {
			const Vector2 positionNormalized = Vector2{ position } / Vector2{ m_Camera->viewport() } -Vector2{ 0.5f };
			const Float length = positionNormalized.length();
			const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
			return (result*Vector3::yScale(-1.0f)).normalized();
		}

		void UnknownLegacy::mouseMoveEvent(MouseMoveEvent& event) {
			if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

			const Vector3 currentPosition = positionOnSphere(event.position());
			const Vector3 axis = Math::cross(_previousPosition, currentPosition);

			if (_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

			m_Manipulator.rotate(Math::angle(_previousPosition, currentPosition), axis.normalized());
			_previousPosition = currentPosition;

			redraw();
		}

	}
}

//MAGNUM_APPLICATION_MAIN(Magnum::Examples::UnknownLegacy)