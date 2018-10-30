#include "Renderer.h"

namespace UL {

	void ulRenderer::zoom(float ratio) {
		m_CameraObject.translate(Vector3::zAxis(m_CameraObject.transformation().translation().z()*ratio));
	}

	void ulRenderer::rotateManupulator(Magnum::Rad r,const Vector3& norm) {
		m_Manipulator.rotate(r, norm);
	}

	void ulRenderer::viewportEvent(const Vector2i& frameBufferSize, const Vector2i& windowSize) {
		GL::defaultFramebuffer.setViewport({ {}, frameBufferSize });
		m_Camera->setViewport(windowSize);
	}

	Vector3 ulRenderer::positionOnSphere(const Vector2i& position) const {
		const Vector2 positionNormalized = Vector2{ position } / Vector2{ m_Camera->viewport() } -Vector2{ 0.5f };
		const Float length = positionNormalized.length();
		const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
		return (result*Vector3::yScale(-1.0f)).normalized();
	}


	void ulRenderer::drawEvent() {
		GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

		m_Camera->draw(m_Drawables);
	}

	ulRenderer::ulRenderer() {
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
	}

	void ulRenderer::loadFile(Trade::AbstractImporter* importer, std::string fname) {
		//Debug{} << "Opening file" << args.value("file");

		/* Load file */
		if (!importer->openFile("triangle_v7.obj"))//args.value("file")))
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

	

	void ulRenderer::addObject(Trade::AbstractImporter& importer, Containers::ArrayView<const Containers::Optional<Trade::PhongMaterialData>> materials, Object3D& parent, UnsignedInt i) {
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
			.setLightPosition(camera.cameraMatrix().transformPoint({ 0.0f, 10.0f, 10.0f }))
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
}