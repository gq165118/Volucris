#ifndef __volucris_resource_h__
#define __volucris_resource_h__

#include <Engine/Core/Object.h>
#include <Engine/Core/TypesHelp.h>

namespace volucris
{
	class Resource : public Object
	{
	public:
		Resource() : Object(), m_dirty(false){}

		~Resource() {}

		void markDirty(bool dirty=true) { m_dirty = dirty; }

		bool isDirty() const { return m_dirty; }

	private:
		bool m_dirty;
	};
}

#endif // !__volucris_resource_h__
