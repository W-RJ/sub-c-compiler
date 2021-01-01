int global;

void func()
{
    scanf(global);
}

void main()
{
    int x, y, z, w, t, i;

    x = 2;
    y = x;
    z = y;
    x = z;
    y = x;

    printf("times:");
    func();

    x = y;

    y = 3;
    x = y;
    for (x = 0; x < global; x=x+1)
    {
        printf("Hello World");
    }

    for (z = 3; z > 0; z = z - 1)
    {
        if (z == 2)
        {
            y = 2;
            t = 2;
        }
        if (z >= 2)
        {
            printf(y);
        }
        t = 3;
        printf(t);
    }

    w = 123;
    t = 456;
    i = 16;
    while (i > 0)
    {
        printf(w);
        w = w + 1;
        t = t + 1;
        i = i / 2;
    }

    do
    {
        printf(i);
        i=i+1;
    }while(i<10);

    printf("Hello World");
}
