#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include "Controller.h"

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

void outputThread(char *argv[])
{
    char last_printed = '\0';
    Controller controller(argv[1]);

    // controller.zero_torque_state();
    // controller.move_to_default_pos();
    // controller.run();
    // controller.damp();

    const std::chrono::milliseconds cycle_time(20);
    auto next_cycle = std::chrono::steady_clock::now();

    float period = .8;
    float time = 0;

    while (running)
    {
        char current;

        {
            std::lock_guard<std::mutex> lock(mtx);
            current = shared_key;
        }

        // if (current != last_printed && current != '\0')
        // {
        std::cout << "[KEYBOARD]: " << current << std::endl;
        // CONTROL LOOP --------
        switch (current)
        {
        case '1':
            std::cout << "[STATE]: zero_torque_state" << std::endl;
            controller.zero_torque_state();
            break;
        case '2':
            std::cout << "[STATE]: move_to_default_pos" << std::endl;
            controller.move_to_default_pos();
            current = '\0';
            break;
        case 'w':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 'a':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 's':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 'd':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 'e':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 'q':
            std::cout << "[STATE]: run" << std::endl;
            controller.run(period, time, current);
            break;
        case 'k':
            std::cout << "[STATE]: damp" << std::endl;
            break;
        default:
            break;
        }
        next_cycle += cycle_time;
        std::this_thread::sleep_until(next_cycle);
        time += .02;
        // ---------------------
        last_printed = current;
        // }

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "walking-g1 [net_interface]" << std::endl;
        exit(1);
    }
    std::cout << "Press keys (single key capture). Press Ctrl+C to quit.\n";

    std::thread t1(inputThread);
    std::thread t2(outputThread, argv);
    t1.join();
    running = false;
    t2.join();

    return 0;
}
