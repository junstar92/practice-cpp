// Longest Common Subsequence (LCS) 
// - Dynamic Programming (DP)
#include <iostream>
#include <string>
#include <vector>

int commonChild(std::string s1, std::string s2)
{
    auto m = s1.size(); auto n = s2.size();
    if (m == 0 || n == 0) return 0;

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            }
            else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }

    return dp[m][n];
}

int main(int, char**)
{
    std::string s1{"OUDFRMYMAW"};
    std::string s2{"AWHYFCCMQX"};
    std::cout << "s1: " << s1 << "\n"
            << "s2: " << s2 << "\n";
    std::cout << "results: " << commonChild(s1, s2) << std::endl;

    return 0;
}