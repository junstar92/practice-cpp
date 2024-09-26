/**
 * Hacker Rank - Short Palindrome
 */
#include <iostream>
#include <string>
#include <vector>

constexpr int MOD = 1e9 + 7;

// 1. Encoding a character to an integer
//      a -> 0
//      b -> 1
//      ...
//      z -> 25
//      aa -> 0 * 26 + 0
//      ab -> 0 * 26 = 1
//      ...
//      zy -> 25 * 26 + 24
//      zz -> 25 * 26 + 25
//
// 2. Declare for count
//  -   freq[26] = {0} contains count of characters seen so far.
//  -   pair_freq[26 * 26] contains count of character pairs seen so far.
//      For example, pair_freq[0 * 26 + 3] is the number of "ad" seen so far.
//  -   triple_freq[26] contains count of triplets which needs character ch to complete anagram.
//      For example, triple_freq[0] contains count of triplets “aaa”, “abb”, “acc” ... “azz” seen so far.
//      So, It can complete the short palindrome by adding another "a".
//      Similarly triple_freq[1] contains count of triplets "baa", "bbb", ..., "bzz" for "b"
//  -   ans contains the count of short palindromes (of length 4 seen so far).
// 
// 3. Scan the string character by character.
//  a.  When you see a character c, encoding it to an integer (i = c - 'a')
//  b.  triple_freq[i] is the number of triplets senn so far that require the last character i to complete short palindroms,
//      so we add triple_freq[v] to ans.
//  c.  And, update triple_freq[i] by adding pair_freq[j]. Here the value of j corresponds to the encoded value of "a"c, "b"c, ..., "z"c.
//  d.  Then, update pair_freq[j] by counting all pairs ending in i (e.g., if i is "a", then all pairs "aa", "ba", "ca", ..., "za")
//  e.  Increment freq[v] by 1.
//
// 4. At end ans would contain the total count of short palindromes.

int shortPalindrome(std::string s)
{
    std::vector<long long> freq(26, 0);
    std::vector<long long> pair_freq(26*26, 0);
    std::vector<long long> triple_freq(26, 0);

    int ans = 0;
    for (auto const& c : s) {
        ans = (ans + triple_freq[c - 'a']) % MOD;
        for (int i = 0, j = c - 'a'; i < 26; i++, j+=26) {
            triple_freq[i] += pair_freq[j];
            pair_freq[j] += freq[i];
        }
        freq[c - 'a']++;
    }

    return ans;
}

int main(int, char**)
{
    std::string test_cases[] {
        "kkkkkkz",  // 15
        "abbaab",   // 4
        "akakak",   // 2
    };

    for (auto const& test_case : test_cases) {
        std::cout << test_case << " : " << shortPalindrome(test_case) << "\n";
    }

    return 0;
}