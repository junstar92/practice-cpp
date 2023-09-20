#include <iostream>
#include <string>
#include <string_utils.hpp>

using namespace std::literals;
using namespace strutils;

int main(int argc, char** argv)
{
    auto ut{to_upper("this is not UPPERCASE"s)};
    auto lt{to_lower("THIS IS NOT lowercase"s)};
    auto rt{reverse("cookbook"s)};
    std::cout << "to_upper(text): " << ut << std::endl;
    std::cout << "to_lower(text): " << std::endl;
    std::cout << "reverse(text): " << rt << std::endl;

    auto text1{"      this is an example    "s};
    auto t1{trim(text1)};
    auto t2{trimleft(text1)};
    auto t3{trimright(text1)};
    std::cout << "\n\ntext1: " << text1 << std::endl;
    std::cout << "trim(text1): " << t1 << std::endl;
    std::cout << "trimleft(text1): " << t2 << std::endl;
    std::cout << "trimright(text1): " << t3 << std::endl;

    auto chars1{" !%\n"s};
    auto text3{"!!  this % needs a lot of trimming!\n"s};
    auto t7{trim(text3, chars1)};
    auto t8{trimleft(text3, chars1)};
    auto t9{trimright(text3, chars1)};
    std::cout << "\n\ntext3: " << text3 << " / chars1: " << chars1 << std::endl;
    std::cout << "trim(text3, chars1): " << t7 << std::endl;
    std::cout << "trimleft(text3, chars1): " << t8 << std::endl;
    std::cout << "trimright(text3, chars1): " << t9 << std::endl;

    auto text4{"must remove all * from text**"s};
    auto t10{remove(text4, '*')};
    auto t11{remove(text4, '!')};
    std::cout << "\n\ntext4: " << text4 << std::endl;
    std::cout << "remove(text4, '*'): " << t10 << std::endl;
    std::cout << "remove(text4, '!'): " << t11 << std::endl;

    auto text5{"this text will be split   "s};
    auto tokens1{split(text5, ' ')};
    auto tokens2{split(""s, ' ')};
    std::cout << "\n\ntext5: " << text5 << std::endl;
    std::cout << "split(text5, ' '): {";
    std::copy(tokens1.begin(), tokens1.end(), std::ostream_iterator<std::string>{std::cout, " "});
    std::cout <<"}\n";
    std::cout << "split(\"\"s, ' '): { ";
    std::copy(tokens2.begin(), tokens2.end(), std::ostream_iterator<std::string>{std::cout, " "});
    std::cout << "}\n";

    return 0;
}