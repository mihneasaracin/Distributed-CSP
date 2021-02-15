#include <unordered_map>
#include <iostream>
#include <cassert>
#include <typeinfo>


// Example to have a map with different types of function


class Handler
{
public:
    template <typename T>
    Handler(T fn)
            : f((void(*)())(fn))
            , info(typeid(T))
    {
    }

    template <typename... Args>
    void operator()(Args&&... args)
    {
        using Fn = void(Args...);
        assert(info.hash_code() == typeid(Fn*).hash_code());
        return ((Fn*)(f))(std::forward<Args>(args)...);
    }
    void (*f)();
    const std::type_info& info;
};


void ab(int x){
    std::cout << x << "\n";
}

int main()
{
    std::unordered_map<int, Handler> cbmap;
    cbmap.emplace(1, +[](int a, double b){std::cout << "1" << a << " " << b << "\n";});
    cbmap.emplace(2, +[](double a){std::cout << "2" << a << "\n";});
    cbmap.emplace(3, +[](double& a){std::cout << "3 " << a << "\n";});
    cbmap.emplace(4, ab);


    double x = 42.0;

    cbmap.at(1)(42,4.2);
    cbmap.at(2)(4.2);
    cbmap.at(3)(x);
    cbmap.at(4)(3);
    std::cout << typeid(cbmap.at(4)).name()<< std::endl;
}