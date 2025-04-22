#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <tlhelp32.h>

// Execute command
void executar_comando(const char *comando) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cmd /c %s", comando);
    system(cmd);
}

// Monitor processes
void monitorar_processos(char *log_resultados) {
    PROCESSENTRY32 processo;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    processo.dwSize = sizeof(PROCESSENTRY32);
    strcat(log_resultados, "Processos ativos:\n");
    if (Process32First(snapshot, &processo)) {
        do {
            strcat(log_resultados, processo.szExeFile);
            strcat(log_resultados, "\n");
        } while (Process32Next(snapshot, &processo));
    }
    CloseHandle(snapshot);
}

// Monitor network connections
void monitorar_conexoes(char *log_resultados) {
    FILE *fp = _popen("netstat -ano", "r");
    if (fp) {
        char buffer[512];
        strcat(log_resultados, "Conexoes de rede:\n");
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            strcat(log_resultados, buffer);
        }
        _pclose(fp);
    }
}

// Monitor ARP table
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

// Block IP
void bloquear_ip(const char *ip) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "netsh advfirewall firewall add rule name=\"BlockIP\" dir=in action=block remoteip=%s", ip);
    executar_comando(cmd);
}

// Unblock IP
void desbloquear_ip() {
    executar_comando("netsh advfirewall firewall delete rule name=\"BlockIP\"");
}

// Update whitelist/blacklist
int atualizar_lista(const char *arquivo, const char *entrada) {
    FILE *file = fopen(arquivo, "a");
    if (file) {
        fprintf(file, "%s\n", entrada);
        fclose(file);
        return 1;
    }
    return 0;
}

int main() {
    char log_resultados[4096] = "";
    char entrada[100];
    int command;
    while (1) {
        printf("* 8 888888888o.      8 8888888888   `8.`8888.      ,8' 8888888 8888888888 8 8888888888   8 888888888o. *\n* 8 8888    `^888.   8 8888          `8.`8888.    ,8'        8 8888       8 8888         8 8888    `88. *\n* 8 8888        `88. 8 8888           `8.`8888.  ,8'         8 8888       8 8888         8 8888     `88 *\n* 8 8888         `88 8 8888            `8.`8888.,8'          8 8888       8 8888         8 8888     ,88 *\n* 8 8888          88 8 888888888888     `8.`88888'           8 8888       8 888888888888 8 8888.   ,88' *\n* 8 8888          88 8 8888             .88.`8888.           8 8888       8 8888         8 888888888P' *\n* 8 8888         ,88 8 8888            .8'`8.`8888.          8 8888       8 8888         8 8888`8b *\n* 8 8888        ,88' 8 8888           .8'  `8.`8888.         8 8888       8 8888         8 8888 `8b.\n* 8 8888    ,o88P'   8 8888          .8'    `8.`8888.        8 8888       8 8888         8 8888   `8b.\n* 8 888888888P'      8 888888888888 .8'      `8.`8888.       8 8888       8 888888888888 8 8888     `88. *\n\n\n");
        printf("#------------------------------------#\n");
        printf("\t Antivirus Options:\n");
        printf("#------------------------------------#\n");
        printf("1 - Monitorar processos\n2 - Monitorar conexoes de rede\n3 - Monitorar tabela ARP (detectar MITM)\n4 - Bloquear IP\n5 - Desbloquear IP\n6 - Adicionar a whitelist\n7 - Adicionar a blacklist\n8 - Sair\nEscolha: ");
        scanf("%d", &command);

        switch (command) {
            case 1:
                monitorar_processos(log_resultados);
                break;
            case 2:
                monitorar_conexoes(log_resultados);
                break;
            case 3:
                monitorar_arp(log_resultados);
                break;
            case 4:
                printf("Digite o IP para bloquear: ");
                scanf("%s", entrada);
                bloquear_ip(entrada);
                break;
            case 5:
                desbloquear_ip();
                break;
            case 6:
                printf("Digite o IP para whitelist: ");
                scanf("%s", entrada);
                atualizar_lista("whitelist.txt", entrada);
                break;
            case 7:
                printf("Digite o IP para blacklist: ");
                scanf("%s", entrada);
                atualizar_lista("blacklist.txt", entrada);
                break;
            case 8:
                exit(0);
            default:
                printf("Opcao invalida!\n");
                break;
        }
    }
    return 0;
}

