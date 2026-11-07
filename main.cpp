#include"MachineController.h"
#include"Menu.h"
#include"Screen.h"

int main(){
    MachineController controller;
    Menu menu(controller);
    menu.run();
    return 0;
}