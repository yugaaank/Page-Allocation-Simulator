/*
 * ============================================================
 *  PAGE REPLACEMENT ALGORITHM SIMULATOR  (Interactive)
 *  Algorithms : FIFO | LRU | Optimal
 *  Compile    : g++ -std=c++17 -Wall -o simulator simulator.cpp
 *  Run        : ./simulator
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <climits>
#include <limits>

// ─── ANSI COLOUR HELPERS ─────────────────────────────────────
std::string red    (const std::string& s){ return "\033[1;31m" + s + "\033[0m"; }
std::string green  (const std::string& s){ return "\033[1;32m" + s + "\033[0m"; }
std::string yellow (const std::string& s){ return "\033[1;33m" + s + "\033[0m"; }
std::string cyan   (const std::string& s){ return "\033[1;36m" + s + "\033[0m"; }
std::string magenta(const std::string& s){ return "\033[1;35m" + s + "\033[0m"; }
std::string bold   (const std::string& s){ return "\033[1m"    + s + "\033[0m"; }
std::string dim    (const std::string& s){ return "\033[2m"    + s + "\033[0m"; }

// ─── DATA TYPE ───────────────────────────────────────────────
struct SimResult {
    std::string                   name;
    int                           faults = 0;
    int                           hits   = 0;
    std::vector<std::vector<int>> frame_states;
    std::vector<bool>             is_fault;
};

// ─── CLEAR SCREEN ────────────────────────────────────────────
void clearScreen() { std::cout << "\033[2J\033[H"; }

// ─── BANNER ──────────────────────────────────────────────────
void printBanner() {
    std::cout << magenta(
        "\n  +================================================+\n"
        "  |     PAGE REPLACEMENT ALGORITHM SIMULATOR      |\n"
        "  |       FIFO  |  LRU  |  Optimal               |\n"
        "  +================================================+\n"
    );
}

// ─── DISPLAY HELPERS ─────────────────────────────────────────
std::string formatFrames(const std::vector<int>& frames, int capacity) {
    std::ostringstream os;
    os << "[ ";
    for (int i = 0; i < capacity; ++i) {
        if (i < (int)frames.size()) os << frames[i];
        else                        os << "-";
        if (i < capacity - 1) os << " | ";
    }
    os << " ]";
    return os.str();
}

void printTable(const SimResult& res, const std::vector<int>& refs, int capacity) {
    const int W_PAGE   = 6;
    const int W_FRAMES = 4 + capacity * 4;
    const int W_STATUS = 10;

    auto hline = [&](const std::string& l, const std::string& sep,
                     const std::string& r, const std::string& f) {
        std::cout << l;
        for (int i = 0; i < W_PAGE   + 2; i++) std::cout << f;
        std::cout << sep;
        for (int i = 0; i < W_FRAMES + 2; i++) std::cout << f;
        std::cout << sep;
        for (int i = 0; i < W_STATUS + 2; i++) std::cout << f;
        std::cout << r << "\n";
    };

    std::cout << "\n  " << cyan(bold(res.name)) << "\n";
    hline("+", "+", "+", "-");
    std::cout << "| " << std::setw(W_PAGE)   << std::left << "Page"
              << " | " << std::setw(W_FRAMES) << std::left << "Frames"
              << " | " << std::setw(W_STATUS) << std::left << "Status"
              << " |\n";
    hline("+", "+", "+", "-");

    for (int i = 0; i < (int)refs.size(); ++i) {
        std::string fr = formatFrames(res.frame_states[i], capacity);
        bool fault     = res.is_fault[i];
        std::string st = fault ? red("FAULT *") : green("HIT    ");

        std::cout << "| " << std::setw(W_PAGE) << std::left << refs[i]
                  << " | " << std::left << fr;

        int pad = W_FRAMES - (int)fr.size();
        for (int p = 0; p < pad; p++) std::cout << " ";
        std::cout << " | " << st << " |\n";
    }

    hline("+", "+", "+", "-");
    double ratio = (double)res.hits * 100.0 / refs.size();
    std::ostringstream rs;
    rs << std::fixed << std::setprecision(2) << ratio << "%";
    std::cout << "  Page Faults : " << red(std::to_string(res.faults))  << "\n";
    std::cout << "  Page Hits   : " << green(std::to_string(res.hits))   << "\n";
    std::cout << "  Hit Ratio   : " << bold(rs.str())                    << "\n";
}

void printSummary(const std::vector<SimResult>& results, int refLen) {
    const int W1=12, W2=8, W3=8, W4=10;

    auto hline = [&](const std::string& l, const std::string& sep,
                     const std::string& r, const std::string& f) {
        std::cout << l;
        for(int i=0;i<W1+2;i++) std::cout << f;
        std::cout << sep;
        for(int i=0;i<W2+2;i++) std::cout << f;
        std::cout << sep;
        for(int i=0;i<W3+2;i++) std::cout << f;
        std::cout << sep;
        for(int i=0;i<W4+2;i++) std::cout << f;
        std::cout << r << "\n";
    };

    std::cout << "\n  " << yellow(bold("== COMPARISON SUMMARY ==")) << "\n";
    hline("+", "+", "+", "=");
    std::cout << "| " << std::setw(W1) << std::left << "Algorithm"
              << " | " << std::setw(W2) << std::left << "Faults"
              << " | " << std::setw(W3) << std::left << "Hits"
              << " | " << std::setw(W4) << std::left << "Hit Ratio"
              << " |\n";
    hline("+", "+", "+", "=");

    int bestFaults = INT_MAX;
    for (auto& r : results) bestFaults = std::min(bestFaults, r.faults);

    for (auto& r : results) {
        double ratio = (double)r.hits * 100.0 / refLen;
        std::ostringstream rs;
        rs << std::fixed << std::setprecision(2) << ratio << "%";
        bool isBest = (r.faults == bestFaults);

        if (isBest) std::cout << "\033[1;32m";
        std::cout << "| " << std::setw(W1) << std::left << r.name
                  << " | " << std::setw(W2) << std::left << r.faults
                  << " | " << std::setw(W3) << std::left << r.hits
                  << " | " << std::setw(W4) << std::left << rs.str()
                  << " |" << (isBest ? " <- BEST" : "        ") << "\n";
        if (isBest) std::cout << "\033[0m";
    }
    hline("+", "+", "+", "=");
}

// ─── ALGORITHMS ──────────────────────────────────────────────
SimResult simulateFIFO(const std::vector<int>& refs, int cap) {
    SimResult r; r.name = "FIFO";
    std::deque<int> queue;
    std::unordered_set<int> inFrames;
    for (int page : refs) {
        if (inFrames.count(page)) {
            r.hits++; r.is_fault.push_back(false);
        } else {
            r.faults++; r.is_fault.push_back(true);
            if ((int)queue.size() == cap) { inFrames.erase(queue.front()); queue.pop_front(); }
            queue.push_back(page); inFrames.insert(page);
        }
        r.frame_states.push_back({queue.begin(), queue.end()});
    }
    return r;
}

SimResult simulateLRU(const std::vector<int>& refs, int cap) {
    SimResult r; r.name = "LRU";
    std::deque<int> order;
    std::unordered_set<int> inFrames;
    for (int page : refs) {
        if (inFrames.count(page)) {
            r.hits++; r.is_fault.push_back(false);
            order.erase(std::find(order.begin(), order.end(), page));
            order.push_back(page);
        } else {
            r.faults++; r.is_fault.push_back(true);
            if ((int)order.size() == cap) { inFrames.erase(order.front()); order.pop_front(); }
            order.push_back(page); inFrames.insert(page);
        }
        r.frame_states.push_back({order.begin(), order.end()});
    }
    return r;
}

SimResult simulateOptimal(const std::vector<int>& refs, int cap) {
    SimResult r; r.name = "Optimal";
    std::vector<int> frames;
    for (int i = 0; i < (int)refs.size(); ++i) {
        int page = refs[i];
        if (std::find(frames.begin(), frames.end(), page) != frames.end()) {
            r.hits++; r.is_fault.push_back(false);
        } else {
            r.faults++; r.is_fault.push_back(true);
            if ((int)frames.size() < cap) {
                frames.push_back(page);
            } else {
                int evict_idx = 0, farthest = -1;
                for (int j = 0; j < (int)frames.size(); ++j) {
                    int nextUse = INT_MAX;
                    for (int k = i+1; k < (int)refs.size(); ++k)
                        if (refs[k] == frames[j]) { nextUse = k; break; }
                    if (nextUse > farthest) { farthest = nextUse; evict_idx = j; }
                }
                frames[evict_idx] = page;
            }
        }
        r.frame_states.push_back(frames);
    }
    return r;
}


// ─── INPUT FUNCTIONS ─────────────────────────────────────────
int readInt(const std::string& prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= minVal && val <= maxVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << red("  Invalid input.") << " Enter a number between "
                  << minVal << " and " << maxVal << ".\n";
    }
}

std::vector<int> readRefString() {
    std::vector<int> refs;
    while (refs.empty()) {
        std::cout << "  Enter reference string (space-separated numbers, e.g. 7 0 1 2 0 3):\n  > ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        int n; bool valid = true;
        while (iss >> n) {
            if (n < 0) { valid = false; break; }
            refs.push_back(n);
        }
        if (!valid || refs.empty()) {
            refs.clear();
            std::cout << red("  Invalid.") << " Enter at least one non-negative number.\n";
        }
    }
    return refs;
}

// Returns bitmask: bit0=FIFO, bit1=LRU, bit2=OPT, bit3=LFU
int selectAlgorithms() {
    std::cout << "\n  " << bold("Select algorithms to run:") << "\n";
    std::cout << "  " << cyan("[1]") << " FIFO\n";
    std::cout << "  " << cyan("[2]") << " LRU\n";
    std::cout << "  " << cyan("[3]") << " Optimal\n";

    std::cout << "  " << cyan("[4]") << " All (FIFO + LRU + Optimal)\n";

    int choice = readInt("  Your choice [1-4]: ", 1, 4);
    if (choice == 4) return 0b0111;

    int mask = 1 << (choice - 1);
    std::cout << "  Add more? Enter another number " << cyan("[1-3]")
              << " to add, " << cyan("[0]") << " when done.\n";
    while (true) {
        int extra = readInt("  > ", 0, 3);
        if (extra == 0) break;
        mask |= (1 << (extra - 1));
    }
    return mask;
}

// ─── BELADY'S ANOMALY DEMO ───────────────────────────────────
void beladyDemo(void) {
    std::vector<int> refs = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    std::cout << "\n  " << yellow(bold("== BELADY'S ANOMALY DEMO ==")) << "\n";
    std::cout << dim("  Reference string: 1 2 3 4 1 2 5 1 2 3 4 5\n");
    std::cout << dim("  With FIFO, MORE frames can sometimes cause MORE faults!\n\n");
    for (int f : {3, 4}) {
        SimResult res = simulateFIFO(refs, f);
        std::cout << "  FIFO with " << bold(std::to_string(f)) << " frames → Faults: "
                  << (f == 4 ? red(std::to_string(res.faults)) : green(std::to_string(res.faults)))
                  << "\n";
        printTable(res, refs, f);
    }
    std::cout << "\n  " << yellow("3 frames = 9 faults, but 4 frames = 10 faults with FIFO!")
              << "\n  " << dim("This is why Optimal and LRU are preferred in theory.\n");
}

// ─── MAIN MENU ───────────────────────────────────────────────
void printMainMenu() {
    std::cout << "\n  " << bold("What would you like to do?") << "\n";
    std::cout << "  " << cyan("[1]") << " Run new simulation\n";
    std::cout << "  " << cyan("[2]") << " Same reference string, different frame count\n";
    std::cout << "  " << cyan("[3]") << " Same reference string, different algorithms\n";
    std::cout << "  " << cyan("[4]") << " Belady's Anomaly demo\n";
    std::cout << "  " << cyan("[0]") << " Exit\n";
}

// ─── MAIN ────────────────────────────────────────────────────
int main() {
    clearScreen();
    printBanner();

    std::vector<int> lastRefs;
    int  lastFrames   = 0;
    int  lastAlgoMask = 0b1111;
    bool hasLastRun   = false;

    while (true) {
        printMainMenu();
        int choice = readInt("  Your choice [0-4]: ", 0, 4);

        if (choice == 0) {
            std::cout << "\n  " << green("Goodbye! Good luck with your project.") << "\n\n";
            break;
        }

        if (choice == 4) { beladyDemo(); continue; }

        // Need a previous run for options 2 and 3
        if ((choice == 2 || choice == 3) && !hasLastRun) {
            std::cout << yellow("  No previous run yet. Starting fresh.\n");
            choice = 1;
        }

        std::vector<int> refs;
        int frames;
        int algoMask;

        if (choice == 1) {
            std::cout << "\n";
            frames   = readInt("  Number of frames [1-20]: ", 1, 20);
            refs     = readRefString();
            algoMask = selectAlgorithms();
        } else if (choice == 2) {
            refs     = lastRefs;
            frames   = readInt("\n  New frame count [1-20]: ", 1, 20);
            algoMask = lastAlgoMask;
        } else { // choice == 3
            refs     = lastRefs;
            frames   = lastFrames;
            algoMask = selectAlgorithms();
        }

        lastRefs     = refs;
        lastFrames   = frames;
        lastAlgoMask = algoMask;
        hasLastRun   = true;

        // Print run header
        std::cout << "\n  " << bold("Frames        : ") << frames << "\n";
        std::cout << "  " << bold("Reference str : ");
        for (int p : refs) std::cout << p << " ";
        std::cout << "\n  " << bold("Length        : ") << refs.size() << "\n";

        // Run selected algorithms
        std::vector<SimResult> results;
        if (algoMask & 0b0001) results.push_back(simulateFIFO   (refs, frames));
        if (algoMask & 0b0010) results.push_back(simulateLRU    (refs, frames));
        if (algoMask & 0b0100) results.push_back(simulateOptimal(refs, frames));


        for (auto& res : results)
            printTable(res, refs, frames);

        if (results.size() > 1)
            printSummary(results, (int)refs.size());
    }

    return 0;
}
