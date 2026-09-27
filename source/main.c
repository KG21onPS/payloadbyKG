#include "ps4.h"

int _main(void)
{
    initKernel();
    initLibc();
    initSysUtil();

    sceSysUtilSendSystemNotificationWithText(
        222,
        "payloadbyKGtest lance !"
    );

    return 0;
}
