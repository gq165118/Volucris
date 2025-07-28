#ifndef __volucris_material_instance_proxy_h__
#define __volucris_material_instance_proxy_h__

#include <Engine/Render/MaterialProxy.h>

namespace volucris
{
	class MaterialInstanceProxy
	{
	public:
		MaterialInstanceProxy();

		void setMaterial(const std::shared_ptr<MaterialProxy>& material);

		void update(const std::vector<MaterialParameterUpdateInfo>& parameters);

		void use(RHICommandList* context);

	private:
		std::vector<MaterialParameterInfo> m_parameters;
		std::shared_ptr<MaterialProxy> m_material;
	};
}

#endif // !__volucris_material_instance_proxy_h__
