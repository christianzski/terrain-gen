#include "window.h"

int main(int, char*[]) {
    window main_window(1280, 729);

    if(main_window.initialized()) {
        while(!main_window.should_close()) {
            main_window.draw();
        }
    } else return 1;

    return 0;
}
