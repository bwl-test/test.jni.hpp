#include <iostream>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tuple>

template <char... chars>
struct StringLiteral {
	const char * operator ()() const {
		static char storage[] = {chars..., 0};
		return storage;
	}
};

template <class T, class... Properties>
struct Builder {
	using DataTuple = std::tuple<typename Properties::DataType...>;
	using NameTuple = std::tuple<typename Properties::NameType...>;

	template <size_t... I>
	DataTuple parseFromObjectHelper(const T&obj, std::index_sequence<I...>) const {
		NameTuple names;
		return std::make_tuple(obj.template Get<std::tuple_element_t<I, DataTuple>>(std::get<I>(names)())...);
	}

	DataTuple parseFromObject(const T&obj) const {
		return parseFromObjectHelper(obj, std::make_index_sequence<sizeof...(Properties)>{});
		
	}
};


class JavaObject {
public:
	template <class T>
	T Get(const char *name) const {
        std::cout << "get property: " << name << "\n";
        return {};
	}
};


struct PeerProperty {
	using DataType = long;
	using NameType = StringLiteral<'p', 'e', 'e', 'r'>;
};

struct NameProperty {
	using DataType = std::string;
	using NameType = StringLiteral<'n', 'a', 'm', 'e'>;
};

TEST_CASE( "通过Builder类获取属性集合", "[single-file]" ) {
	Builder<JavaObject, PeerProperty, NameProperty> builder;

	JavaObject obj;
	auto ret1 = builder.parseFromObject(obj);
}
