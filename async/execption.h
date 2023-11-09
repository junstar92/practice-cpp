#pragma once
#include <iostream>
#include <future>
#include <exception>
#include <thread>
#include <utility>

namespace exception {

/**
 * If the callable used by `std::async` or by `std::packaged_task` throws an error,
 * the execption is store in the shared state. When the future then calls `get()`,
 * the exception is rethrown, and the future has to handle it.
 * 
 * `std::promise` provides the same facility but has the mothod `set_value(std::current_exception())`
 * to set the exception as shared state.
 */

struct Div {
    void operator()(std::promise<int>&& int_promise, int a, int b) {
        try {
            if (b == 0) {
                std::string msg = std::string("Illegal division by zero: ") 
                    + std::to_string(a) + "/" + std::to_string(b);
                throw std::runtime_error(msg);
            }
            int_promise.set_value(a / b);
        }
        catch (...) {
            int_promise.set_exception(std::current_exception());
        }
    }
};

void except_promise()
{
    std::cout << std::endl;

    // Dividing a number by 0 is undefined behavior.
    auto execute_division = [](int nom, int denom) {
        std::promise<int> div_promise;
        auto div_result = div_promise.get_future();

        Div div;
        std::thread div_thread(div, std::move(div_promise), nom, denom);

        // get the result or the exception
        try {
            std::cout << nom << "/" << denom << " = " << div_result.get() << std::endl;
        }
        catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
        }

        div_thread.join();
    };

    execute_division(20, 0);
    execute_division(20, 10);
    
    std::cout << "-------------------------------------------------------" << std::endl << std::endl;
}

}