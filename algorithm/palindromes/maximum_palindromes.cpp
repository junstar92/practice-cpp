/**
 * HackerRank - Maximum Palindromes
 */

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

constexpr int MOD = 1e9 + 7;

std::vector<std::vector<int>> Cnt;
std::vector<long long> Factorial;
std::vector<long long> PowMminus2;

// from Fermat's little theorem
long long pow(long long num, int p)
{
    if (p == 0) return 1;
    long long ret = pow(num, p / 2) % MOD;
    ret = (ret * ret) % MOD;
    if (p % 2) ret = (num * ret) % MOD;
    return ret;
}

void initialize(std::string s)
{
    auto const length = s.length();
    Cnt = std::vector<std::vector<int>>(length + 1, std::vector<int>(26, 0));
    Factorial.resize(length + 1, 1);
    PowMminus2.resize(length + 1, 1);

    for (int i = 1; i <= length; i++) {
        ++Cnt[i][s[i-1] - 'a'];
    }
    for (int i = 2; i <= length; i++) {
        for (int j = 0; j < 26; j++) {
            Cnt[i][j] += Cnt[i - 1][j];
        }
    }

    for (int i = 1; i <= length; i++) {
        Factorial[i] = (Factorial[i - 1] * i) % MOD;
        PowMminus2[i] = pow(Factorial[i], MOD - 2);
    }
}

int answerQuery(int l, int r)
{
    long long ans{1};
    int total_odd{}, total_even{};
    
    for (int i = 0; i < 26; i++) {
        int num_char = Cnt[r][i] - Cnt[l-1][i];
        int num_even = num_char / 2;
        
        total_even += num_even;
        ans = (1L * ans * PowMminus2[num_even]) % MOD;
        if (num_char % 2 == 1) ++total_odd;
    }

    ans = (1L * ans * Factorial[total_even]) % MOD;
    if (total_odd > 1) ans = (1L * ans * total_odd) % MOD;

    return ans;
}

int main(int, char**)
{
    std::string s{"madamimadam"};

    initialize(s);

    std::pair<int, int> queries[]{
        {4, 7}, // -> amim ==> mam, mim => 2 
        {1, 5}, // -> madam ==> madam, amdma => 2 
    };

    for (auto const& [l, r] : queries) {
        std::cout << "[" << l << ", " << r << "] " << answerQuery(l ,r) << std::endl;
    }

    return 0;
}