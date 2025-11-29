#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include "Controller.h"

// Shared key storage
std::mutex mtx;
char shared_key = '\0';
std::atomic<bool> running(true);

// --- Terminal setup for single-character input ---
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

// --- Thread to read keypresses ---
void inputThread()
{
    TermiosGuard guard;

    while (running)
    {
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            {
                std::lock_guard<std::mutex> lock(mtx);
                shared_key = c;
            }

            if (c == '\x03') // Ctrl+C
            {
                running = false;
                break;
            }

            std::cout << "[InputThread] Key pressed: " << c << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// --- Thread to run controller ---
void controllerThread(Controller *controller)
{
    
    while (running)
    {
        char key = '\0';
        {
            std::lock_guard<std::mutex> lock(mtx);
            key = shared_key;
            shared_key = '\0'; // clear after reading
        }

        if (key != '\0')
        {
            // Example: react to key in controller
            std::cout << "[ControllerThread] Got key: " << key << std::endl;
            // You can call your controller methods here
            controller->handleKey(key);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void walkingThread(Controller *controller){
    std::cout<<"walking thread\n";
    controller->run();
}
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: walking-g1 [net_interface]" << std::endl;
        return 1;
    }
    Controller controller(argv[1]);
    std::cout << "Press keys (single key capture). Press Ctrl+C to quit.\n";
    // Launch threads
    std::thread t_input(inputThread);
    std::thread t_controller(controllerThread, &controller);
    std::thread t_walking(walkingThread, &controller);

    // Wait for threads to finish
    t_input.join();
    running = false;
    t_controller.join();
    t_walking.join();

    std::cout << "Exiting program.\n";
    return 0;
}
