#include "Socket.h"
#include "SocketFactory.h"
#include "SocketType.h"


#include <iostream>


int main()
{
    
    while (true) {
        int option;
        std::shared_ptr<Socket> socket=nullptr;
        std::cout << "1.Server\n"
            << "2.Client\n"
            << "Enter the choise : ";
        std::cin >> option;
        std::cin.ignore(1, '\n');


        switch (option) {
            case 1:
                socket = SocketFactory::getSocket(SocketType::SERVER);
                break;
            case 2:
                socket = SocketFactory::getSocket(SocketType::CLIENT);
                break;
            default:
                std::cout << "Invalid option.";
                continue;
        }
        
        socket->setup();
        
    }
    


}
