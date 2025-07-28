#ifndef __volucris_material_loader_h__
#define __volucris_material_loader_h__

#include <string>
#include <MaterialEditor/MaterialTemplate.h>

namespace volucris
{
	class MaterialLoader
	{
	public:
		MaterialLoader();

		bool setVertexSource(const std::string& filepath);

		bool setFragmentSource(const std::string& filepath);

		bool load();

		std::shared_ptr<MaterialTemplate> getMaterial() const { return m_material; }

		const std::string& getAssetName() const { return m_name; }

		static std::string getSource(const std::string& filepath);

	private:
		void init(const std::string& filepath);

		std::string getKey(const std::string& filepath);

	private:
		std::string m_name;
		std::string m_vsp;
		std::string m_fsp;
		std::string m_key;
		std::shared_ptr<MaterialTemplate> m_material;
	};
}

#endif // !__volucris_material_loader_h__
