///----------------------------------------------------------------------------------------------------

template <class...>
struct TypeList {};

///jni
struct Env {};
struct jobject {};

using jlong = long;

struct Method {
    const char *name{nullptr};
    const char *signature{nullptr};
    void *ptr{nullptr};
};

template <class... Args>
void jniRegisterNatives(const Args&... args) {
    static_assert(std::conjunction_v<std::is_same<Args, Method>...>, "only support Method Type");
    Method methods[] = {reinterpret_cast<const Method&>(args)...};
    for (auto const &method : methods) {
        if (method.name) {
            std::cout << "register method " << method.name << "\n";
        }
    }
}




///library
template<class T>
struct Field {
    const char *name;
    int id;
};

template <class Tag>
struct Class {
    
};

template <class Tag>
struct Object {
    template <class T>
    T Get(const Field<T> &fileld) const {
        return T{};
    }
    
    template <class T>
    void Set(const Field<T> &fileld, T value) {
        
    }
};

template <class T, class Enable=void>
struct NativeMethodTrait;

template <class R, class... Args>
struct NativeMethodTrait<R(Args...)> {
    using FuntionType = R(Args...);
};

template <class R, class... Args>
struct NativeMethodTrait<R(*)(Args...)> : NativeMethodTrait<R(Args...)> {};

template <class T, class R, class... Args>
struct NativeMethodTrait<R(T::*)(Args...)> : NativeMethodTrait<R(Args...)> {};

template <class T, class R, class... Args>
struct NativeMethodTrait<R(T::*)(Args...) const> : NativeMethodTrait<R(Args...)> {};


template <class M>
struct NativeMethodTrait<M, std::void_t<decltype(&M::operator())>> : NativeMethodTrait<decltype(&M::operator())> {};

template <class T>
using NativeFuntionType = typename NativeMethodTrait<T>::FuntionType;


template <class T, T*...>
struct NativeMethodMaker;

template <class T, class R, class... Args>
struct NativeMethodMaker<R(T::*)(Env&, Args...)> {
    Method operator()(const char *name, const T& m) const {
        static T method{m};
        auto wrapper = [](Env &env, Args... args) {
            return method(env, args...);
        };
        
        NativeFuntionType<decltype(wrapper)> *function = wrapper;
        return Method{name, "signature place holder", reinterpret_cast<void *>(function)};
    };
};

template <class T, class R, class... Args>
struct NativeMethodMaker<R(T::*)(Env&, Args...) const> : NativeMethodMaker<R(T::*)(Env&, Args...)>{};

template <class M>
Method MakeNativeMethod(const char *name, const M &m, std::void_t<decltype(&M::operator())> *dummy = nullptr) {
    auto methodMaker = NativeMethodMaker<decltype(&M::operator())>{};
    return methodMaker(name, m);
}


template <class Tag, class Peer, class>
struct NativePeerHelper;

template <class Tag, class Peer, class... Args>
struct NativePeerHelper<Tag, Peer, std::unique_ptr<Peer>(Env&, Args...)> {
    using UniquePeer = std::unique_ptr<Peer>;
    using Initializer = UniquePeer(Env&, Args...);
    
    auto MakeInitializer(const char *initiaName, const Field<long> &field, Initializer *init) {
        auto lambda = [field, init] (Env& env, Object<Tag> &obj, Args... args) {
            auto unique = init(env, args...);
            auto previous = UniquePeer{reinterpret_cast<Peer *>(obj.Get(field))};
            previous.reset();
            obj.Set(field, reinterpret_cast<long>(unique.release()));
        };
        
        
        return MakeNativeMethod(initiaName, lambda);
    }
};

template <class Tag, class Peer, class Initializer>
void registerNativePeer(const char *peer, const char *initiaName, Initializer *init) {
    auto helper = NativePeerHelper<Tag, Peer, Initializer>{};
    jniRegisterNatives(helper.MakeInitializer(initiaName, Field<long>{}, init));
}

template<class Peer, class... Args>
std::unique_ptr<Peer> makePeer(Env &env, Args... args) {
    return std::make_unique<Peer>(env, args...);
}


///client code
class Employee {
private:
    int id;
    std::string name;
    
public:
    Employee(Env &env, int id_, std::string name_) {
        id = id_;
        name = name_;
    }
    
    std::string getName() const {
        return name;
    }
};

struct EmployeeTag {};

TEST_CASE( "registerNative测试", "[single-file]" ) {
    Method m1, m2;
    m1.name = "methodA";
    m2.name = "methodB";
    jniRegisterNatives(m1, m2);
    
    registerNativePeer<EmployeeTag, Employee>("peer", "nativeInit", makePeer<Employee, int, std::string>);
}
