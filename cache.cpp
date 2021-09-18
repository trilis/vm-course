#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <set>

size_t a[1024 * 1024 * 4 * 256];

int time(int H, int S) {
    for (int i = (S - 1) * H; i > 0; i -= H) {
        a[i] = (size_t) &a[i - H];
    }
    a[0] = (size_t) &a[(S - 1) * H];
    std::vector<long long> ans;
    for (int k = 0; k < 1000; k++) {
        auto start = std::chrono::high_resolution_clock::now();
        size_t** x = (size_t **)&a[(S - 1) * H];
        for (int i = 0; i < 100000; i++) {
            x = (size_t **)*x;
        }
        auto end = std::chrono::high_resolution_clock::now();
        ans.push_back((end - start).count());
    }
    return std::reduce(ans.begin(), ans.end()) / ans.size() / 1000;
}

int main() {

    std::vector<std::set<int>> jumps;
    int H = 16;

    for (; H < 4 * 1024 * 1024; H *= 2) {
        std::cout << "probing stride " << H << "..." << std::endl;
        int prev_time = -1;
        std::set<int> new_jumps;
    
        for (int S = 1; S <= 16; S++) {
            int curr_time = time(H, S);
            if (prev_time != -1 && curr_time - prev_time > 70) {
                new_jumps.insert(S - 1);
                std::cout << S - 1 << " ";
            }
            prev_time = curr_time;
        }
        std::cout << std::endl;
        
        bool changed = jumps.empty() || new_jumps.empty() || new_jumps.size() == jumps[jumps.size() - 1].size();
        for (int jump : new_jumps) {
            changed |= jumps[jumps.size() - 1].count(jump);
        }

        jumps.push_back(new_jumps);

        if (!changed && H >= 256 * 1024) {
            break;
        }
    }

    std::cout << "done!" << std::endl;

    std::vector<std::pair<int, int>> caches;

    std::set<int> to_process = jumps[jumps.size() - 1];
    std::reverse(jumps.begin(), jumps.end());
    for (auto& jump : jumps) {
        std::set<int> to_delete;
        for (int s : to_process) {
            if (!jump.count(s)) {
                caches.push_back({H * s * sizeof(size_t), s});
                to_delete.insert(s);
            }
        }
        for (int s : to_delete) {
            to_process.erase(s);
        }
        H /= 2;
    }

    std::sort(caches.begin(), caches.end());

    for (int i = 0; i < caches.size(); i++) {
        std::cout << "L" << i + 1 << " cache: " << "size = " << caches[i].first << ", associativity = " << caches[i].second << std::endl;
    }

}