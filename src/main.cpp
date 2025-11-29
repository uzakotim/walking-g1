#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <termios.h>
#include <unistd.h>

std::mutex mtx;
char shared_key = '\0';
std::atomic<bool> running(true);

// --- Configure terminal to read single characters ---
struct TermiosGuard
{
    termios old_tio;

    TermiosGuard()
    {
        tcgetattr(STDIN_FILENO, &old_tio);
        termios new_tio = old_tio;
        new_tio.c_lflag &= ~(ICANON | ECHO); // disable canonical mode + echo
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    }

    ~TermiosGuard()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    }
};

void inputThread()
{
    TermiosGuard guard; // automatically restores terminal on exit

    while (running)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            std::lock_guard<std::mutex> lock(mtx);
            shared_key = c;

            if (c == '\x03')
            { // Ctrl+C to stop
                running = false;
            }
        }
    }
}

void outputThread()
{
    char last_printed = '\0';

    while (running)
    {
        char current;

        {
            std::lock_guard<std::mutex> lock(mtx);
            current = shared_key;
        }

        if (current != last_printed && current != '\0')
        {
            std::cout << "Key pressed: " << current << std::endl;
            last_printed = current;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

int main()
{
    std::cout << "Press keys (single key capture). Press Ctrl+C to quit.\n";

    std::thread t1(inputThread);
    std::thread t2(outputThread);

    t1.join();
    running = false;
    t2.join();

    return 0;
}
