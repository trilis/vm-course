#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <set>
#include <algorithm>

char* a;

int time(int H, int S) {
  register char** x;
    for (int i = (S - 1) * H; i >= 0; i -= H) {
      char* next;
      x = (char**) &a[i];
      if (i >= H) {
        next = &a[i - H];
      } else {
        next = &a[(S - 1) * H];
      }
        *x = next;
    }
    std::vector<long long> ans;
    for (int k = 0; k < 20; k++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000000; i++) {
            x = (char **)*x;
        }
        auto end = std::chrono::high_resolution_clock::now();
        ans.push_back((end - start).count());
    }
    return std::accumulate(ans.begin(), ans.end(), 0) / 20;
}

int main() {

    a = (char*) malloc(1024 * 1024 * 1024);
    std::vector<std::set<int>> jumps;
    int H = 16;

    for (; H < 1024 * 1024 * 1024 / 16; H *= 2) {
        std::cout << "probing stride " << H << "..." << std::endl;
        int prev_time = -1;
        std::set<int> new_jumps;
    
        for (int S = 1; S <= 16; S++) {
            int curr_time = time(H, S);
            //std::cout << curr_time << " ";
            if (prev_time != -1 && (double) (curr_time - prev_time) / curr_time > 0.1) {
                new_jumps.insert(S - 1);
                std::cout << S - 1 << " ";
            }
            prev_time = curr_time;
        }
        std::cout << std::endl;
        
        bool same = 1;
        if (jumps.size() > 0) {
            for (int jump : new_jumps) {
                same &= jumps[jumps.size() - 1].count(jump);
            }
            for (int jump : jumps[jumps.size() - 1]) {
                same &= new_jumps.count(jump);
            }
        }

        if (same && H >= 256 * 1024) {
            break;
        }

        jumps.push_back(new_jumps);
    }

    std::cout << "done!" << std::endl;

    std::vector<std::pair<int, int>> caches;

    std::set<int> to_process = jumps[jumps.size() - 1];
    std::reverse(jumps.begin(), jumps.end());
    for (auto& jump : jumps) {
        std::set<int> to_delete;
        for (int s : to_process) {
            if (!jump.count(s)) {
                caches.push_back({H * s, s});
                to_delete.insert(s);
            }
        }
        for (int s : to_delete) {
            to_process.erase(s);
        }
        H /= 2;
    }

    std::sort(caches.begin(), caches.end());

    for (int i = 0; i < std::min((int) caches.size(), 2); i++) {
        int cache_size = caches[i].first;
        int cache_assoc = caches[i].second;
        int cache_line_size = -1;
        int prev_first_jump = 1025;
        for (int L = 1; L <= cache_size; L *= 2) {
            int prev_time = -1;
            int first_jump = -1;
            std::cout << "probing stride " << cache_size / cache_assoc + L << "..." << std::endl;
            for (int S = 1; S <= 1024; S *= 2) {
                int curr_time = time(cache_size / cache_assoc + L, S);
                if (prev_time != -1 && (double) (curr_time - prev_time) / curr_time > 0.3) {
                    if (first_jump <= 0) {
                        first_jump = S;
                    }
                    std::cout << S << " ";
                }
                prev_time = curr_time;
            }
            std::cout << std::endl;
            if (first_jump > prev_first_jump) {
                cache_line_size = L * cache_assoc;
                break;
            }
            prev_first_jump = first_jump;
        }
        std::cout << "L" << i + 1 << " cache: " << "size = " << cache_size <<
             ", associativity = " << cache_assoc <<
             ", line size = " << cache_line_size << std::endl;
    }

}