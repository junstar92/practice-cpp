# C++ Practice

## Overview of C++

### C++11

- The C++11 is called standard **Modern C++**.
- C++ introduced many features that fundamentally changed the way we program in C++.
- Move Sementics, Perfect Forwarding, Variadic Templates, and `constexpr`.
- Automatic Type Deduction (`auto`).

### C++14

- A small C++ standard.
- Read-Write Locks, Generalized Lambdas, and extended `constexpr` functions.
- `auto` for functions.
- Introduces `std::make_unique`.

### C++17

- It has two outstanding features: the parallel STL and standarized filesystem API(`std::filesystem`).
- About 80 algorithms of the STL can be excuted in parallel or vectorized.
- New data types: `std::string_view`, `std::optional`, `std::variant`, and `std::any`.
- Structured Bindings: `auto [var1, var2, var3] = expr;`

### C++20

- 4 outstanding features: Concepts, Ranges, Coroutines, and Modules.
- Three-Way Comparision Operator `<=>`
- Designated Initialization

```c++
struct Point2D {
    int x;
    int y;
};
struct Point3D {
    int x;
    int y;
    int z;
};

int main(int argc, char** argv)
{
    Point2D point2D { .x=1, .y=2 };
    // Point2D point2d { .y=2, .x=1 }; // error
    Point3D point3D { .x=1, .y=2, .z=2 };
    // Point3D point3D { .x=1, .z=2 }; // {1, 0, 2}
}
```

- `consteval` and `constinit`
- Various improvements to programming with templates.
- Lambda improvements (lambda templates, ...).
- `std::span` : represents an object that can refer to a contiguous sequence of objects.

```c++
void copy_n(int const* src, int* des, int n) {}
void copy(std::span<int const> src, std::span<int> des) {} // std::span automatically deduces the size of an array, a std::array, or a std::vector.
int main()
{
    int arr1[] = { 1, 2, 3 };
    int arr2[] = { 3, 4, 5 };

    copy_n(arr1, arr2, 3);
    copy(arr1, arr2);
}
```

- Foramtting library (`std::format`).
- `std::jthread` : joinable `std::thread`.
- Synchronized Outputstreams (`std::osyncstream()`).
- Mathematical Constants (`<numbers>` header).