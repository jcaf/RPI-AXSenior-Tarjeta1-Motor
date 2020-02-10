#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

struct _main_flag
{
    unsigned f20ms:1;
    unsigned alarm:1;
    unsigned __a:6;
};

extern struct _main_flag main_flag;




#endif // MAIN_H_INCLUDED
