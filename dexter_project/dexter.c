#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// Função para pressionar teclas
void pressKey(WORD vk) {
    keybd_event(vk, 0, 0, 0);
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
}

// Função para digitar texto
void typeText(const char *text) {
    for (int i = 0; i < strlen(text); i++) {
        WORD vk = VkKeyScan(text[i]);
        if ((vk & 0x0100) != 0) {
            keybd_event(VK_SHIFT, 0, 0, 0);
        }
        pressKey(vk & 0xFF);
        if ((vk & 0x0100) != 0) {
            keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
        }
        Sleep(50);
    }
}

// Função para executar comandos no CMD
void executar_comando(const char *comando) {
    char cmd[256];
    sprintf(cmd, "cmd /c %s", comando);
    system(cmd);
}

// Monitoramento de processos ativos
void monitorar_processos(char *log_resultados) {
    PROCESSENTRY32 processo;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    processo.dwSize = sizeof(PROCESSENTRY32);
    strcat(log_resultados, "Monitoramento de processos ativos:\n");
    if (Process32First(snapshot, &processo)) {
        do {
            strcat(log_resultados, processo.szExeFile);
            strcat(log_resultados, "\n");
        } while (Process32Next(snapshot, &processo));
    } else {
        strcat(log_resultados, "Erro ao acessar os processos ativos.\n");
    }
    CloseHandle(snapshot);
}

// Função para bloquear IPs da blacklist
void bloquear_ips() {
    FILE *file = fopen("blacklist.txt", "r");
    char ip[100];
    if (file) {
        while (fgets(ip, sizeof(ip), file) != NULL) {
            ip[strcspn(ip, "\n")] = 0;
            char command[200];
            sprintf(command, "netsh advfirewall firewall add rule name=\"Dexter Block\" dir=in action=block remoteip=%s", ip);
            system(command);
            printf("Bloqueado: %s\n", ip);
        }
        fclose(file);
        printf("Bloqueio de IPs concluido.\n");
    } else {
        printf("Erro ao abrir blacklist.txt\n");
    }
}

// Função para desbloquear os IPs
void desbloquear_ips() {
    system("netsh advfirewall firewall delete rule name=\"Dexter Block\"");
    printf("Desbloqueio realizado.\n");
}

// Monitoramento de conexões de rede
void monitorar_conexoes(char *log_resultados) {
    FILE *fp = _popen("netstat -ano", "r"); // Use _popen to execute netstat and capture its output
    if (fp) {
        char buffer[512];
        strcat(log_resultados, "Monitoramento de conexoes de rede:\n");
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            strcat(log_resultados, buffer); // Append each line of netstat output to log_resultados
        }
        _pclose(fp); // Close the pipe
    } else {
        strcat(log_resultados, "Erro ao executar netstat.\n");
    }
}

// Verificação de arquivos maliciosos
void verificar_malware() {
    printf("Iniciando verificacao de malwares...\n");
    system("dir /s /b C:\\ > file_list.txt");
    printf("Verificacao concluida. Verifique o arquivo file_list.txt\n");
}

void monitorar_arp(char *log_resultados) {
    FILE *fp = _popen("arp -a", "r");
    if (fp) {
        char buffer[512];
        strcat(log_resultados, "Verificando tabela ARP:\n");
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            strcat(log_resultados, buffer);
        }
        _pclose(fp);
        strcat(log_resultados, "Verificacao de tabela ARP concluida.\n");
    } else {
        strcat(log_resultados, "Erro ao executar comando arp -a.\n");
    }
}

// Manipulação de listas (Whitelist/Blacklist)
int atualizar_lista(const char *arquivo, const char *entrada) { // Change return type to int
    FILE *file = fopen(arquivo, "a");
    if (file) {
        fprintf(file, "%s\n", entrada);
        fclose(file);
        printf("%s adicionado a lista %s\n", entrada, arquivo);
        return 1; // Success
    } else {
        printf("Erro ao abrir a lista %s\n", arquivo);
        return 0; // Failure
    }
}

// Menu principal
void menu() {
    int command;
    char entrada[100];
    char ultima_acao[256] = "Nenhuma acao realizada ainda."; // Armazena a última ação realizada
    char log_resultados[4096] = ""; // Aumentei o tamanho para armazenar mais resultados

    while (1) {
        const char *ascii[] = {
            " * 8 888888888o.      8 8888888888   `8.`8888.      ,8' 8888888 8888888888 8 8888888888   8 888888888o. *",
            " * 8 8888    `^888.   8 8888          `8.`8888.    ,8'        8 8888       8 8888         8 8888    `88. *",
            " * 8 8888        `88. 8 8888           `8.`8888.  ,8'         8 8888       8 8888         8 8888     `88 *",
            " * 8 8888         `88 8 8888            `8.`8888.,8'          8 8888       8 8888         8 8888     ,88 *",
            " * 8 8888          88 8 888888888888     `8.`88888'           8 8888       8 888888888888 8 8888.   ,88' *",
            " * 8 8888          88 8 8888             .88.`8888.           8 8888       8 8888         8 888888888P'  *",
            " * 8 8888         ,88 8 8888            .8'`8.`8888.          8 8888       8 8888         8 8888`8b       ",
            " * 8 8888        ,88' 8 8888           .8'  `8.`8888.         8 8888       8 8888         8 8888 `8b.     ",
            " * 8 8888    ,o88P'   8 8888          .8'    `8.`8888.        8 8888       8 8888         8 8888   `8b.   ",
            " * 8 888888888P'      8 888888888888 .8'      `8.`8888.       8 8888       8 888888888888 8 8888     `88. *"
        };

        const char *menu_linhas[] = {
            "#------------------------------------#",
            "           Antivirus Options:",
            "#------------------------------------#",
            "1  - Executar Neofetch no CMD",
            "2  - Monitorar processos ativos",
            "3  - Monitorar conexoes de rede",
            "4  - Verificar malware",
            "5  - Adicionar a whitelist",
            "6  - Adicionar a blacklist",
            "7  - Bloquear IPs da blacklist",
            "8  - Desbloquear IPs da blacklist",
            "9  - Monitorar tabela ARP (detectar MITM)"
            "10 - Sair",
        };

        int num_linhas_menu = sizeof(menu_linhas) / sizeof(menu_linhas[0]);
        int num_linhas_ascii = sizeof(ascii) / sizeof(ascii[0]);
        int max_linhas = (num_linhas_menu > num_linhas_ascii) ? num_linhas_menu : num_linhas_ascii;

        printf("\n");

        for (int i = 0; i < max_linhas; i++) {
            if (i < num_linhas_menu)
                printf("%-45s", menu_linhas[i]); // menu com alinhamento
            else
                printf("%-45s", ""); // linha vazia se o menu acabar

            if (i < num_linhas_ascii)
                printf("%s", ascii[i]); // imprime arte ao lado
            printf("\n");
        }

        printf("\nUltima acao: %s\n", ultima_acao);
        printf("\nLog de resultados:\n%s\n", log_resultados);
        printf("\nEscolha uma opcao: ");
        scanf("%d", &command);

        switch (command) {
            case 1:
                executar_comando("neofetch");
                strcpy(ultima_acao, "Executado Neofetch no CMD.");
                strcat(log_resultados, "Neofetch executado com sucesso.\n");
                break;
            case 2:
                monitorar_processos(log_resultados);
                strcpy(ultima_acao, "Monitoramento de processos ativos realizado.");
                break;
            case 3:
                monitorar_conexoes(log_resultados);
                strcpy(ultima_acao, "Monitoramento de conexoes de rede realizado.");
                break;
            case 4:
                verificar_malware();
                strcpy(ultima_acao, "Verificacao de malware realizada.");
                strcat(log_resultados, "Verificacao de malware concluida. Verifique o arquivo file_list.txt.\n");
                break;
            case 5:
                printf("Digite o IP para whitelist: ");
                scanf("%s", entrada);
                if (atualizar_lista("whitelist.txt", entrada)) {
                    sprintf(ultima_acao, "IP %s adicionado a whitelist.", entrada);
                    sprintf(log_resultados + strlen(log_resultados), "IP %s adicionado com sucesso a whitelist.\n", entrada);
                } else {
                    sprintf(ultima_acao, "Falha ao adicionar IP %s a whitelist.", entrada);
                    sprintf(log_resultados + strlen(log_resultados), "Erro ao adicionar IP %s a whitelist.\n", entrada);
                }
                break;
            case 6:
                printf("Digite o IP para blacklist: ");
                scanf("%s", entrada);
                if (atualizar_lista("blacklist.txt", entrada)) {
                    sprintf(ultima_acao, "IP %s adicionado a blacklist.", entrada);
                    sprintf(log_resultados + strlen(log_resultados), "IP %s adicionado com sucesso a blacklist.\n", entrada);
                } else {
                    sprintf(ultima_acao, "Falha ao adicionar IP %s a blacklist.", entrada);
                    sprintf(log_resultados + strlen(log_resultados), "Erro ao adicionar IP %s a blacklist.\n", entrada);
                }
                break;
            case 7:
                bloquear_ips();
                strcpy(ultima_acao, "Bloqueio de IPs da blacklist realizado.");
                strcat(log_resultados, "Bloqueio de IPs da blacklist concluido.\n");
                break;
            case 8:
                desbloquear_ips();
                strcpy(ultima_acao, "Desbloqueio de IPs da blacklist realizado.");
                strcat(log_resultados, "Desbloqueio de IPs da blacklist concluido.\n");
                break;
            case 9:
                exit(0);
            default:
                strcpy(ultima_acao, "Opcao invalida selecionada.");
                strcat(log_resultados, "Opcao invalida inserida.\n");
                break;
            case 10:
                monitorar_arp(log_resultados);
                strcpy(ultima_acao, "Monitoramento de ARP realizado.");
                break;
        }
    }
}

int main() {
    menu();
    return 0;
}
