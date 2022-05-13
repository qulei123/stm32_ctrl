
#include "mod_shell.h"
#include "nr_micro_shell.h"
#include "util_log.h"
#include <string.h>


#if _NR_MICRO_SHELL_ENABLE

static void shell_ls_cmd(char argc, char *argv)
{
    unsigned int i = 0;
    if (argc > 1)
    {
        if (!strcmp("cmd", &argv[argv[1]]))
        {
            for (i = 0; nr_shell.static_cmd[i].fp != NULL; i++)
            {
                sh_printf("%s",nr_shell.static_cmd[i].cmd);
                sh_printf("\r\n");
            }
        }
        else if (!strcmp("-v", &argv[argv[1]]))
        {
            sh_printf("ls version 1.0.\r\n");
        }
        else if (!strcmp("-h", &argv[argv[1]]))
        {
            sh_printf("useage: ls [options]\r\n");
            sh_printf("options: \r\n");
            sh_printf("\t -h \t: show help\r\n");
            sh_printf("\t -v \t: show version\r\n");
            sh_printf("\t cmd \t: show all commands\r\n");
        }
    }
    else
    {
        sh_printf("ls need more arguments!\r\n");
    }
}

void Mod_Shell_Init(void)
{
    shell_init();
}

int Mod_Shell_Parse(uint8_t *pbuf, uint16_t len)
{
    if (pbuf == NULL || len == 0)
    {
        return -1;
    }
    while (len--)
    {
        shell(*pbuf);
        pbuf++;
    }
    return 0;
}

extern void shell_motor_cmd(char argc, char *argv);
extern void Shell_Temp_Cmd(char argc, char *argv);
extern void Shell_Baty_Cmd(char argc, char *argv);
extern void Shell_Key_Cmd(char argc, char *argv);
extern void Shell_Link_Cmd(char argc, char *argv);

/* 注册命令使用链表改写, 减小模块直接的耦合 */
const static_cmd_st static_cmd[] =
{
    {"ls", shell_ls_cmd},
    {"motor", shell_motor_cmd},
    {"temp", Shell_Temp_Cmd},
    {"baty", Shell_Baty_Cmd},
    {"key", Shell_Key_Cmd},
    {"link", Shell_Link_Cmd},
    {"\0", NULL}
};

#endif  // _NR_MICRO_SHELL_ENABLE

