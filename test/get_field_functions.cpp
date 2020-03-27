#include <iostream>
#include <type_traits>
#include <catch2/catch.hpp>
#include <tuple>

template <class T>
struct Field {
	using Type = T;
};

template <class T>
struct Property {
	using Type = T;

	Property(const char *name_) : name(name_) {}
	const char *name;
};

template <class ObjType, class PropertyTuple, class FieldTuple, size_t... I>
auto parseFromObjectWithFieldHelper(const ObjType& obj, PropertyTuple &properties, FieldTuple &fields, std::index_sequence<I...>) {
	return std::make_tuple(obj.template Get<std::tuple_element_t<I, FieldTuple>>(std::get<I>(properties).name)...);
}

template <class KlassType, class ObjType, class... Ts>
auto parseFromObjectWithField(const KlassType &klass, const ObjType& obj, Property<Ts>... properties) {
	auto fields = std::make_tuple(klass.template GetField<Ts>(properties.name)...);
	auto propertyTuple = std::make_tuple(properties...);
	return parseFromObjectWithFieldHelper(obj, propertyTuple, fields, std::make_index_sequence<sizeof...(properties)>{});
}

template <class T>
struct JavaObjectExtractor {
	template <class ObjType>
	static T Get(const ObjType& obj, const char *name) {
		std::cout << "Get Nromal property: " << name << "\n";
		return {};
	}
};

template <class T>
struct JavaObjectExtractor<Field<T>> {
	template <class ObjType>
	static T Get(const ObjType& obj, const char *name) {
		std::cout << "Get Field property: " << name << "\n";
		return {};
	}
};

class JavaObject {
public:
	template <class T>
	auto Get(const char *name) const {
		return JavaObjectExtractor<T>::Get(*this, name);
	}
};

class JavaClass {
public:
	template <class T>
	auto GetField(const char *name) const {
		return Field<T>{};
	}
};

TEST_CASE( "通过field获取属性集合", "[single-file]" ) {
	JavaClass klass;
	JavaObject obj;
    parseFromObjectWithField(klass, obj, Property<int>{"gretting"}, Property<int>{"hello"}, Property<int>{"you"});
}
