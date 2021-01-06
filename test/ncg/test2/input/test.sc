int func(int x)
{
    if (x <= 1)
    {
        return (x);
    }
    else
    {
        return (func(x-2) + func(x-1));
    }
}

void main()
{
    int i,x;
    scanf(x);
    for (i=0; i<x; i=i+1)
        printf(func(i));
}