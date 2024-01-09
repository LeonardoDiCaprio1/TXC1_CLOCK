bit write = 0;
sbit sda = P2 ^ 0;
sbit scl = P2 ^ 1;
void delay0()
{
    ;
    ;
}
void start()
{
    sda = 1;
    delay0();
    scl = 1;
    delay0();
    sda = 0;
    delay0();
}

void stop()
{
    sda = 0;
    delay0();
    scl = 1;
    delay0();
    sda = 1;
    delay0();
}

void respons()
{
    uchar i;
    scl = 1;
    delay0();
    while ((sda == 1) && (i < 250))
        i++;
    scl = 0;
    delay0();
}
void init_24c02()
{
    sda = 1;
    delay0();
    scl = 1;
    delay0();
}
void write_byte(uchar date)
{
    uchar i, temp;
    temp = date;
    for (i = 0; i < 8; i++)
    {
        temp = temp << 1;
        scl = 0;
        delay0();
        sda = CY;
        delay0();
        scl = 1;
        delay0();
    }
    scl = 0;
    delay0();
    sda = 1;
    delay0();
}
uchar read_byte()
{
    uchar i, k;
    scl = 0;
    delay0();
    sda = 1;
    delay0();
    for (i = 0; i < 8; i++)
    {
        scl = 1;
        delay0();
        k = (k << 1) | sda;
        scl = 0;
        delay0();
    }
    return k;
}
void write_add(uchar address, uchar date)
{
    start();
    write_byte(0xa0);
    respons();
    write_byte(address);
    respons();
    write_byte(date);
    respons();
    stop();
}
char read_add(uchar address)
{
    uchar date;
    start();
    write_byte(0xa0);
    respons();
    write_byte(address);
    respons();
    start();
    write_byte(0xa1);
    respons();
    date = read_byte();
    stop();
    return date;
}
