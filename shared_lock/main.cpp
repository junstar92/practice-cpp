#include <iostream>
#include <map>
#include <shared_mutex>
#include <string>
#include <thread>

void shared_lock_exam()
{
    // std::shared_lock has the same interface as a std::unique_lock
    // but behaves differently when used with a std::shared_timed_mutex
    // or a std::shared_mutex.
    // Many threads can shared one std::shared_timed_mutex(std::shared_mutex)
    // and, therefore, implement a reader-writer lock.
    {
        // typical example using a reader-writer lock
        // : An arbitrary number of threads executing read operations can access 
        //   the critical region at the same time, but only one thread is allowed to write.
        std::map<std::string, int> tele_book{
            {"Dijkstra", 1972}, {"Scott", 1976}, {"Ritchie", 1983}
        };
        std::shared_timed_mutex tele_book_mutex;

        std::cout << "The old telephone book" << std::endl;
        for (auto tel : tele_book) {
            std::cout << tel.first << ": " << tel.second << std::endl;
        }
        std::cout << std::endl;

        // To access the telephone book at the same time,
        // the reading threads use the `std::shared_lock` in print_number function below
        auto print_number = [&](std::string const& name) {
            std::shared_lock reader_lock(tele_book_mutex);
            std::cout << name << ": " << tele_book[name];
        };
        // The writing threads need exclusive access to the critical section.
        // The exclusivity is given by the `std::lock_guard<std::shared_timed_mutex>>.
        auto add_to_telebook = [&](std::string const& name, int tele) {
            std::lock_guard write_lock(tele_book_mutex);
            std::cout << "\nSTARTING UPDATE " << name;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            tele_book[name] = tele;
            std::cout << " ... ENDING UPDATE " << name << std::endl;
        };

        std::thread reader1(print_number, "Scott");
        std::thread reader2(print_number, "Ritchie");
        std::thread writer1(add_to_telebook, "Scott", 1968);
        std::thread reader3(print_number, "Dijkstra");
        std::thread reader4(print_number, "Scott");
        std::thread writer2(add_to_telebook, "Bjarne", 1965);
        std::thread reader5(print_number, "Scott");
        std::thread reader6(print_number, "Ritchie");
        std::thread reader7(print_number, "Scott");
        std::thread reader8(print_number, "Bjarne");

        reader1.join(); reader2.join(); reader3.join(); reader4.join(); reader5.join();
        reader6.join(); reader7.join(), reader8.join();
        writer1.join(); writer2.join();

        std::cout << "\n\nThe new telephone book" << std::endl;
        for (auto tel : tele_book) {
            std::cout << tel.first << ": " << tel.second << std::endl;
        }

        std::cout << "-------------------------------------------------------" << std::endl << std::endl;

        // This program has undefined behavior. It has a data race between a reader and writer.
        // If "bjarne" is not found in the telephone book, a pair ("bjarne", 0) is created from the read access.
        // To fix this issue, revise the function `print_number` like just another example below
    }
    {
        std::map<std::string, int> tele_book{
            {"Dijkstra", 1972}, {"Scott", 1976}, {"Ritchie", 1983}
        };
        std::shared_timed_mutex tele_book_mutex;

        // Fix version for resolving data race between a reader and a writer
        auto print_number = [&](std::string const& name) {
            std::shared_lock reader_lock(tele_book_mutex);
            if (tele_book.find(name) != tele_book.end()) {
                std::cout << name << ": " << tele_book[name];
            }
            else {
                std::cout << name << " not found!\n";
            }
        };
        auto add_to_telebook = [&](std::string const& name, int tele) {
            std::lock_guard write_lock(tele_book_mutex);
            std::cout << "\nSTARTING UPDATE " << name;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            tele_book[name] = tele;
            std::cout << " ... ENDING UPDATE " << name << std::endl;
        };

        std::thread reader1(print_number, "Scott");
        std::thread reader2(print_number, "Ritchie");
        std::thread writer1(add_to_telebook, "Scott", 1968);
        std::thread reader3(print_number, "Dijkstra");
        std::thread reader4(print_number, "Scott");
        std::thread writer2(add_to_telebook, "Bjarne", 1965);
        std::thread reader5(print_number, "Scott");
        std::thread reader6(print_number, "Ritchie");
        std::thread reader7(print_number, "Scott");
        std::thread reader8(print_number, "Bjarne");

        reader1.join(); reader2.join(); reader3.join(); reader4.join(); reader5.join();
        reader6.join(); reader7.join(), reader8.join();
        writer1.join(); writer2.join();

        std::cout << "\n\nThe new telephone book" << std::endl;
        for (auto tel : tele_book) {
            std::cout << tel.first << ": " << tel.second << std::endl;
        }

        std::cout << "-------------------------------------------------------" << std::endl << std::endl;
    }
}

int main()
{
    shared_lock_exam();
}