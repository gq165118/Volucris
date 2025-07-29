#ifndef __volucris_delegate_h__
#define __volucris_delegate_h__

#include <functional>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <Engine/Core/VectorHelp.h>
#include <Engine/Core/Object.h>

namespace volucris
{
	template <typename ReturnType, typename ...Args>
	class DelegateCallable
	{
	public:
		virtual ~DelegateCallable() = default;

		virtual ReturnType invoke(Args&&... args) = 0;
	};

	template <typename Callable, typename ReturnType, typename ...Args>
	class DelegateCallableWrapper : public DelegateCallable<ReturnType, Args...>
	{
	public:
		DelegateCallableWrapper(const Callable& callable)
			: m_callable(callable) {}

		DelegateCallableWrapper(Callable&& callable)
			: m_callable(std::move(callable)) 
		{
		}

		ReturnType invoke(Args&&... args)
		{
			return m_callable(std::forward<Args>(args)...);
		}

	private:
		Callable m_callable;
	};

	template <typename Callable, typename ReturnType, typename ...Args>
	class FunctionCallableWrapper : public DelegateCallable<ReturnType, Args...>
	{
	public:
		FunctionCallableWrapper(Callable callable)
			: m_callable(callable) 
		{
		}

		ReturnType invoke(Args&&... args)
		{
			return m_callable(std::forward<Args>(args)...);
		}

	private:
		Callable m_callable;
	};

	template <typename ReturnType, typename ...Args>
	class SingleDelegate
	{
		using DelegateCallablePtr = DelegateCallable<ReturnType, Args...>*;

		DelegateCallablePtr m_callable = nullptr;

	public:
		SingleDelegate() = default;

		template<typename Callable>
		SingleDelegate(Callable&& callable)
		{
			bind(callable);
		}

		SingleDelegate(const SingleDelegate& other)
			: m_callable(other.m_callable)
		{
		}

		SingleDelegate(SingleDelegate&& other)
			: m_callable(other.m_callable)
		{
			other.m_callable = nullptr;
		}

		~SingleDelegate()
		{
			unbind();
		}

		template<typename Callable>
		bool bind(Callable&& callable)
		{
			if (m_callable)
			{
				return false;
			}

			if constexpr (std::is_function_v<std::decay_t<Callable>>) {
				m_callable = new FunctionCallableWrapper<std::decay_t<Callable>, ReturnType, Args...>(callable);
			}
			else {
				m_callable = new DelegateCallableWrapper<std::decay_t<Callable>, ReturnType, Args...>(std::forward<Callable>(callable));
			}
			return true;
		}

		void unbind()
		{
			delete m_callable;
			m_callable = nullptr;
		}

		void clear()
		{
			unbind();
		}

		ReturnType invoke(Args... args)
		{
			if (m_callable)
			{
				return m_callable->invoke(std::forward<Args>(args)...);
			}
			return ReturnType();
		}

		bool isValid() const { return m_callable != nullptr; }
	};

	template <typename ReturnType, typename ...Args>
	class MutiDelegate
	{
		using EventHandle = DelegateCallable<ReturnType, Args...>*;
		std::vector<EventHandle> m_callables;

	public:
		MutiDelegate() = default;

		MutiDelegate(const MutiDelegate& other)
			: m_callables(other.m_callables)
		{
		}

		MutiDelegate(MutiDelegate&& other)
			: m_callables(std::move(other.m_callables))
		{
			other.m_callables = {};
		}

		~MutiDelegate()
		{
			for (auto & m_callable : m_callables)
			{
				delete m_callable;
			}
		}

		template<typename Callable>
		EventHandle bind(Callable&& callable)
		{
			EventHandle handle = createHandle(callable);
			m_callables.push_back(handle);
			return handle;
		}

		template<typename Callable>
		EventHandle bindLamda(Callable&& callable)
		{
			EventHandle handle = createHandle(callable);
			m_callables.push_back(handle);
			return handle;
		}

		template<typename T, typename Callable>
		EventHandle bindObject(T* object, Callable&& callable)
		{
			ObjectCallable objectCallable;
			objectCallable.object = object;
			objectCallable.handle = createHandle([object, callable = callable](auto&& ...args)->ReturnType {
				return std::invoke(callable, object, std::forward<Args>(args)...);
				});
			m_objectCallables.push_back(objectCallable);
			return objectCallable.handle;
		}

		void invoke(Args... args)
		{
			for (auto & m_callable : m_callables)
			{
				if (m_callable)
				{
					m_callable->invoke(std::forward<Args>(args)...);
				}
			}

			for (auto& callable : m_objectCallables)
			{
				if (callable.object)
				{
					callable.call(std::forward<Args>(args)...);
				}
			}
		}

		void unbind(EventHandle handle)
		{
			VectorHelp::quickRemove<EventHandle>(m_callables, handle);
			delete handle;
		}

		void unbind(Object* object)
		{
			VectorHelp::quickRemoveAllIf<ObjectCallable>(m_objectCallables, [object](const ObjectCallable& callable) {
				return callable.object == object;
				});
		}

		void clear()
		{
			for (auto& callable : m_callables)
			{
				delete callable;
			}
			m_callables.clear();
		}

	private:
		template<typename Callable>
		EventHandle createHandle(Callable&& callable)
		{
			if constexpr (std::is_function_v<std::decay_t<Callable>> ||
				std::is_member_function_pointer_v<std::decay_t<Callable>>)
			{
				return new FunctionCallableWrapper<std::decay_t<Callable>, ReturnType, Args...>(callable);
			}
			else 
			{
				return new DelegateCallableWrapper<std::decay_t<Callable>, ReturnType, Args...>(std::forward<Callable>(callable));
			}
		}

	private:

		struct ObjectCallable
		{
			Object* object;
			EventHandle handle;
			void call(Args&&... args)
			{
				handle->invoke(std::forward<Args>(args)...);
			}
		};
		std::vector<ObjectCallable> m_objectCallables;
	};

	

	enum class Key
	{
		KEY_INVALID = 0,
		KEY_A = 'A', KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,
		KEY_0, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_F1 = 0x70, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12
	};

	enum class Modifier
	{
		SHIFT = 0x01,
		CTRL = 0x02,
		ALT = 0x04
	};

}

#define DECLARE_EVENT_DELEGATE(EVENT_TYPE, ...) using EVENT_TYPE = volucris::SingleDelegate<##__VA_ARGS__>;
#define DECLARE_EVENT_MUTI_DELEGATE(EVENT_TYPE, ...) using EVENT_TYPE = volucris::MutiDelegate<##__VA_ARGS__>;

#endif // !__volucris_delegate_h__