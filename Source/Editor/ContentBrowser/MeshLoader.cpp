#include "MeshLoader.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <EditorCore/editor.h>
#include <Engine/Core/TypesHelp.h>
#include <Engine/Game/MeshResourceData.h>
#include <Engine/Game/StaticMesh.h>

namespace volucris
{
	inline glm::vec3 assimpVec3ToGlmVec3(const aiVector3D& pos)
	{
		return { pos.x, pos.y, pos.z };
	}

	MeshLoader::MeshLoader(const std::string& filepath)
		: m_filepath(filepath)
	{
	}

	bool MeshLoader::load()
	{
		//Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
		Assimp::Importer importer;
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
			aiPrimitiveType_LINE | aiPrimitiveType_POINT);
		importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
			aiComponent_LIGHTS | aiComponent_CAMERAS);
		importer.SetPropertyBool(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, true);
		importer.ApplyPostProcessing(aiProcess_PreTransformVertices);
		//importer.SetPropertyBool(AI_CONFIG_PP_PTV_KEEP_HIERARCHY, true);
		aiScene const* scene = importer.ReadFile(m_filepath,
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_GlobalScale |
			aiProcess_GenUVCoords |
			aiProcess_Triangulate |
			aiProcess_RemoveComponent |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType
		);

		if (!scene)
		{
			V_LOG_WARN(Editor, "load mesh from file: {} failed.", m_filepath);
			return false;
		}

		std::vector<aiNode*> nodes = { scene->mRootNode };
		while (!nodes.empty())
		{
			auto node = *nodes.rbegin();
			nodes.pop_back();
			if (node->mNumMeshes > 0)
			{
				auto res = loadMeshFromNode(scene, node);
				m_meshes.push_back(std::move(res));
			}

			for (auto idx = 0; idx < node->mNumChildren; ++idx)
			{
				nodes.push_back(node->mChildren[idx]);
			}
		}
		
		return true;
	}

	MeshResource MeshLoader::loadMeshFromNode(const aiScene* scene, aiNode* node)
	{
		MeshResource resource;
		auto mesh = std::make_shared<StaticMesh>();
		resource.mesh = mesh;
		resource.name = node->mName.C_Str();
		size_t vertexCount = 0;
		bool buildUV0 = false;
		bool buildUV1 = false;

		aiMatrix4x4 transform = node->mTransformation;

		// 2. 从矩阵中分解出平移分量（即pivot位置）
		aiVector3D scaling;
		aiQuaternion rotation;
		aiVector3D location; // 这就是pivot点坐标（父空间坐标系）
		transform.Decompose(scaling, rotation, location);

		MeshData data;

		for (auto idx = 0; idx < node->mNumMeshes; ++idx)
		{
			auto mesh = scene->mMeshes[node->mMeshes[idx]];
			vertexCount += mesh->mNumVertices;

			if (idx == 0)
			{
				buildUV0 = mesh->mTextureCoords[0] != nullptr;
				buildUV1 = mesh->mTextureCoords[1] != nullptr;
			}
		}
		resource.slots.reserve(node->mNumMeshes);

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> uv0;
		std::vector<glm::vec3> uv1;

		vertices.reserve(vertexCount);
		normals.reserve(vertexCount);
		if (buildUV0) { uv0.reserve(vertexCount); }
		if (buildUV1) { uv1.reserve(vertexCount); }


		glm::vec3 minPos, maxPos;
		minPos = maxPos = assimpVec3ToGlmVec3(scene->mMeshes[node->mMeshes[0]]->mVertices[0]);
		
		for (auto idx = 0; idx < node->mNumMeshes; ++idx)
		{
			auto mesh = scene->mMeshes[node->mMeshes[idx]];
			auto indexOffset = vertices.size();

			for (auto idx = 0; idx < mesh->mNumVertices; ++idx)
			{
				auto pos = assimpVec3ToGlmVec3(mesh->mVertices[idx]);
				minPos.x = minPos.x < pos.x ? minPos.x : pos.x;
				minPos.y = minPos.y < pos.y ? minPos.y : pos.y;
				minPos.z = minPos.z < pos.z ? minPos.z : pos.z;

				maxPos.x = maxPos.x > pos.x ? maxPos.x : pos.x;
				maxPos.y = maxPos.y > pos.y ? maxPos.y : pos.y;
				maxPos.z = maxPos.z > pos.z ? maxPos.z : pos.z;
				vertices.push_back(pos);
			}

			for (auto idx = 0; idx < mesh->mNumVertices; ++idx)
			{
				auto normal = glm::normalize(assimpVec3ToGlmVec3(mesh->mNormals[idx]));
				normals.push_back(normal);
			}

			if (buildUV0)
			{
				for (auto idx = 0; idx < mesh->mNumVertices; ++idx)
				{
					glm::vec3 uv = { 0,0,0 };
					if (auto texcoords = mesh->mTextureCoords[0])
					{
						uv = assimpVec3ToGlmVec3(texcoords[idx]);
					}
					uv0.push_back(uv);
				}
			}

			if (buildUV1)
			{
				for (auto idx = 0; idx < mesh->mNumVertices; ++idx)
				{
					glm::vec3 uv = { 0,0,0 };
					if (auto texcoords = mesh->mTextureCoords[1])
					{
						uv = assimpVec3ToGlmVec3(texcoords[idx]);
					}
					uv1.push_back(uv);
				}
			}

			{
				std::unique_ptr<LargeMeshElements> elements = std::make_unique<LargeMeshElements>();
				auto count = mesh->mNumFaces * 3;
				elements->reserve(count);
				for (auto k = 0; k < mesh->mNumFaces; ++k)
				{
					auto& face = mesh->mFaces[k];
					elements->create() = indexOffset + face.mIndices[0];
					elements->create() = indexOffset + face.mIndices[1];
					elements->create() = indexOffset + face.mIndices[2];
				}
				resource.mesh->addSubMesh(std::move(elements));
			}

			// 材质
			MeshResource::Slot slot;
			slot.index = idx;
			slot.assimpMatIndex = mesh->mMaterialIndex;
			resource.slots.emplace_back(slot);
		}
		data.setVertices(std::move(vertices));
		data.setNormals(std::move(normals));
		data.setUV0(std::move(uv0));
		data.setUV1(std::move(uv1));
		
		resource.mesh->setMeshData(std::move(data));
		return resource;
	}

}
