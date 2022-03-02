void waitticks(unsigned char ticks)
{
    *((unsigned char*)0x00ff) = 0;  // reset counter
    while((*(unsigned char*)0x00ff) < ticks);
    return;
}

void main(void)
{
    while(1)
    {
        *((unsigned char*)0xf800) = '*';
        waitticks(25);
        *((unsigned char*)0xf800) = ' ';
        waitticks(25);
    } 
    return;
}
