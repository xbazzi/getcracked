#include <iostream>

using namespace std;
int n {};

void solve()
{
    int cnt {};

    cin >> n;
    if (n & 0b1) {
        cout << 0 << '\n';
        return;
    }
    int max_cows = n / 4;
    for (size_t i = 0; i <= max_cows; i++) {
        int c = (n - 4 * i) / 2;
        if (c < 0)
            break;
        ++cnt;
    }
    // cout << '\n';
    cout << cnt << '\n';
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t {};
    cin >> t;
    while (t--) {
        solve();
    }

    return 0;
}
