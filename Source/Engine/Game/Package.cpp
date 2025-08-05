#include "Game/Package.h"
#include "Core/VectorHelp.h"
#include "Asset/AssetManager.h"
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/detail/archive_serializer_map.hpp>

namespace volucris
{
	Package::Package()
		: GameObject()
		, m_assetData()
	{
		m_assetData.guid = GUID::generate();
	}

	Package::Package(const std::string& path)
		: Package()
	{
		m_assetData.path = path;
	}

	Package::~Package()
	{
	}

	void Package::updateDependecies()
	{
		//DependentCollectorArchive collection;
		//collection << m_object;
	}

} // namespace volucris


BOOST_CLASS_EXPORT_IMPLEMENT(volucris::Package);