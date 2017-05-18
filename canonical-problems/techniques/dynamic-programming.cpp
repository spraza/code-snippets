#include <iostream>
#include <string>

namespace my {
    namespace lowest_common_subsequence {
        std::string max(const std::string& s1, const std::string& s2) {
            if (s2.size() >= s1.size())
                return s2;
            return s1;
        }

        /// Non memoized DP. Can easily be memoized by using a hash map
        /// (std::unordered_map) with custom parameters object as key.
        /// parameters object is something like:
        /// struct { string x; string y; int i; int j }.
        /// Can also be implemented with for-loops as a bottom-up DP.
        std::string dp(const std::string& x, const std::string& y, int i,
                       int j) {
            if (i < 0 || j < 0)
                return "";

            const std::string dpI =
                x[i] == y[j] ? dp(x, y, i - 1, j - 1) + x[i]
                             : max(dp(x, y, i - 1, j), dp(x, y, i, j - 1));
            return dpI;
        }

        std::string lcs(const std::string& x, const std::string& y) {
            return dp(x, y, x.size() - 1, y.size() - 1);
        }
    }

    /// Non memoized DP. Can easily be memoized by using a hash map
    /// Same technique to do that as mentioned above
    namespace min_char_palindrome {
        int min(const int a, const int b) { return a < b ? a : b; }
        int dp(const std::string& x, const int i, const int j) {
            if (i == j)
                return 0;
            return x[i] != x[j] ? 1 + min(dp(x, i + 1, j), dp(x, i, j - 1))
                                : dp(x, i + 1, j - 1);
        }
        int mcp(const std::string& x) {
            if (x.empty())
                return 0;
            return dp(x, 0, x.size() - 1);
        }
    }
}

// Code Output if you run this code:
// $clang++ dynamic-programming.cpp && ./a.out
//    x   : ABCBDAB
//    y   : XYZAYZXBDCABC
//    lcs : ABDAB with length = 5
//    -----------
//    z: Ab3bd
//    mcpCount: 2

int main() {
    using namespace my;
    const std::string x = "ABCBDAB";
    const std::string y = "XYZAYZXBDCABC";

    const std::string lcsStr = lowest_common_subsequence::lcs(x, y);
    std::cout << "x   : " << x << std::endl;
    std::cout << "y   : " << y << std::endl;
    std::cout << "lcs : " << lcsStr << " with length = " << lcsStr.size()
              << std::endl;

    std::cout << "-----------" << std::endl;

    const std::string z = "Ab3bd";
    const int mcpCount = min_char_palindrome::mcp(z);
    std::cout << "z: " << z << std::endl;
    std::cout << "mcpCount: " << mcpCount << std::endl;

    return 0;
}
