#include <iostream>

int main()
{
    float square_size = 3.88;
    int y;
    float length;

    for(int i=0;i<10;i++)
    {
        y = i;
        length = y*square_size;
        std::cout << length << std::endl;
    }

    return 0;
}
