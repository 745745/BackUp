#include "server.h"

int main()
{
    server a;
    a.sockInit();

    while (1)
    {
        while (!a.listenSocket())
            ;

        a.connectClient();
        while (1)
        {
            a.chooseFunction();
            if(a.disconnect())
            break;
        }
        
    }
}