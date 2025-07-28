#ifndef __volucris_vector_help_h__
#define __volucris_vector_help_h__

#include <vector>
#include <memory>
#include <functional>
#include <fmt/format.h>

namespace volucris
{
	namespace VectorHelp
	{
		template<typename T>
		inline bool quickRemove(std::vector<std::shared_ptr<T>>& vec, T* value)
		{
			auto it = std::find_if(vec.begin(), vec.end(), [value](const std::shared_ptr<T>& ptr)->bool {
				if (ptr.get() == value)
				{
					return true;
				}
				return false;
				});
			if (it != vec.end())
			{
				std::swap(*it, *(vec.rbegin()));
				vec.pop_back();
				return true;
			}
			return false;
		}

		template<typename T>
		inline bool quickRemove(std::vector<T>& vec, const T& value)
		{
			auto it = std::find(vec.begin(), vec.end(), value);
			if (it != vec.end())
			{
				std::swap(*it, *(vec.rbegin()));
				vec.pop_back();
				return true;
			}
			return false;
		}

		template<typename T>
		inline bool quickRemoveIf(std::vector<T>& vec, const std::function<bool(const T&)>& pred)
		{
			auto it = std::find_if(vec.begin(), vec.end(), pred);
			if (it != vec.end())
			{
				std::swap(*it, *(vec.rbegin()));
				vec.pop_back();
				return true;
			}
			return false;
		}

		template<typename T>
		inline bool quickRemoveAll(std::vector<T>& vec, const T& value)
		{
			bool find = false;
			size_t idx = 0;
			while (idx < vec.size())
			{
				auto it = vec.begin() + idx;
				if (*it == value)
				{
					std::swap(*it, *(vec.rbegin()));
					vec.pop_back();
					find = true;
				}
				else
				{
					++idx;
				}
			}
			return find;
		}

		template<typename T>
		inline void quickRemoveAllIf(std::vector<T>& vec, const std::function<bool(const T&)>& pred)
		{
			vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
		}
	}

}

// 格式化vector
template <typename T>
struct fmt::formatter<std::vector<T>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const std::vector<T>& vec, format_context& ctx) const {
		auto out = ctx.out();
		fmt::format_to(out, "[");
		for (size_t i = 0; i < vec.size(); ++i) {
			if (i != 0) fmt::format_to(out, " ");
			fmt::format_to(out, "{}", vec[i]);
		}
		return fmt::format_to(out, "]");
	}
};

#endif // !__volucris_vector_help_h__
