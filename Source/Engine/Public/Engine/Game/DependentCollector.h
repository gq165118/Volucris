#ifndef __volucris_dependent_collector_h__
#define __volucris_dependent_collector_h__

#include <boost/type_traits/is_enum.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/detail/oserializer.hpp>
#include <Engine/Game/SoftObject.h>
#include <vector>
#include <string>

namespace volucris
{

    class DependentCollectorArchive : public boost::archive::detail::common_oarchive<DependentCollectorArchive>
    {
        std::vector<std::string> dependencies;

        using base_type = boost::archive::detail::common_oarchive<DependentCollectorArchive>;
        friend class boost::archive::save_access;
        friend struct boost::archive::detail::interface_oarchive<DependentCollectorArchive>;

        template <typename T>
        struct is_softobject : public std::false_type {};

        template <typename T>
        struct is_softobject<SoftObject<T>> : public std::true_type {};

        template <typename T>
        static constexpr bool is_softobject_v = is_softobject<T>::value;

        template <typename T>
        typename std::enable_if_t<is_softobject_v<T>, std::string>
            extract_string(const T& obj) {
            return obj.getPath();
        }

        // 序列化对象（递归遍历）
        template<class T>
        void serialize_object(const T& t) {
            const_cast<T&>(t).serialize(*this, 0);
        }

    public:
        DependentCollectorArchive() : base_type(0) {}

        typedef boost::archive::detail::common_oarchive<DependentCollectorArchive> detail_common_oarchive;

        template<class T>
        void save_override(const T& t) {
            // 判断是否为 SoftObject<T>
            if constexpr (is_softobject_v<T>) {
                // 收集字符串数据
                dependencies.push_back(extract_string(t));
            }
            else {
                // 对于非 SoftObject 类型，继续遍历其内部成员
                this->detail_common_oarchive::save_override(t);
            }
        }

        void save_override(const boost::archive::class_id_optional_type& /* t */) {}

        template<typename T>
        void save(const T& t) {}
    };
}

//BOOST_SERIALIZATION_REGISTER_ARCHIVE(volucris::DependentCollectorArchive)

#endif // !__volucris_dependent_collector_h__
