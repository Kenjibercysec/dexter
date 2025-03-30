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
void monitorar_processos() {
    PROCESSENTRY32 processo;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    processo.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snapshot, &processo)) {
        do {
            printf("[Processo]: %s\n", processo.szExeFile);
        } while (Process32Next(snapshot, &processo));
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
void monitorar_conexoes() {
    system("netstat -ano");
}

// Verificação de arquivos maliciosos
void verificar_malware() {
    printf("Iniciando verificacao de malwares...\n");
    system("dir /s /b C:\\ > file_list.txt");
    printf("Verificacao concluida. Verifique o arquivo file_list.txt\n");
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
    char log_resultados[1024] = ""; // Armazena os resultados das ações

    while (1) {
        system("cls"); // Limpa a tela para atualizar o menu
        printf("* 8 888888888o.      8 8888888888   `8.`8888.      ,8' 8888888 8888888888 8 8888888888   8 888888888o. *\n* 8 8888    `^888.   8 8888          `8.`8888.    ,8'        8 8888       8 8888         8 8888    `88. *\n* 8 8888        `88. 8 8888           `8.`8888.  ,8'         8 8888       8 8888         8 8888     `88 *\n* 8 8888         `88 8 8888            `8.`8888.,8'          8 8888       8 8888         8 8888     ,88 *\n* 8 8888          88 8 888888888888     `8.`88888'           8 8888       8 888888888888 8 8888.   ,88' *\n* 8 8888          88 8 8888             .88.`8888.           8 8888       8 8888         8 888888888P' *\n* 8 8888         ,88 8 8888            .8'`8.`8888.          8 8888       8 8888         8 8888`8b *\n* 8 8888        ,88' 8 8888           .8'  `8.`8888.         8 8888       8 8888         8 8888 `8b.\n* 8 8888    ,o88P'   8 8888          .8'    `8.`8888.        8 8888       8 8888         8 8888   `8b.\n* 8 888888888P'      8 888888888888 .8'      `8.`8888.       8 8888       8 888888888888 8 8888     `88. *\n\n\n");
        printf("#------------------------------------#\n");
        printf("\t Antivirus Options:\n");
        printf("#------------------------------------#\n");
        printf("1 - Executar Neofetch no CMD\n");
        printf("2 - Monitorar processos ativos\n");
        printf("3 - Monitorar conexoes de rede\n");
        printf("4 - Verificar malware\n");
        printf("5 - Adicionar a whitelist\n");
        printf("6 - Adicionar a blacklist\n");
        printf("7 - Bloquear IPs da blacklist\n");
        printf("8 - Desbloquear IPs da blacklist\n");
        printf("9 - Sair\n");
        printf("\nUltima acao: %s\n", ultima_acao); // Exibe a última ação realizada
        printf("\nLog de resultados:\n%s\n", log_resultados); // Exibe os resultados das ações anteriores
        printf("\nEscolha uma opcao: ");
        scanf("%d", &command);

        switch (command) {
            case 1:
                executar_comando("neofetch");
                strcpy(ultima_acao, "Executado Neofetch no CMD.");
                strcat(log_resultados, "Neofetch executado com sucesso.\n");
                break;
            case 2: {
                char buffer[512] = "Monitoramento de processos ativos:\n";
                PROCESSENTRY32 processo;
                HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                processo.dwSize = sizeof(PROCESSENTRY32);
                if (Process32First(snapshot, &processo)) {
                    do {
                        strcat(buffer, processo.szExeFile);
                        strcat(buffer, "\n");
                    } while (Process32Next(snapshot, &processo));
                }
                CloseHandle(snapshot);
                strcpy(ultima_acao, "Monitoramento de processos ativos realizado.");
                strcpy(log_resultados, buffer); // Atualiza o log com os processos ativos
                break;
            }
            case 3: {
                FILE *fp = _popen("netstat -ano", "r");
                if (fp) {
                    char buffer[512];
                    strcpy(log_resultados, "Monitoramento de conexoes de rede:\n");
                    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                        strcat(log_resultados, buffer);
                    }
                    _pclose(fp);
                    strcpy(ultima_acao, "Monitoramento de conexoes de rede realizado.");
                } else {
                    strcpy(ultima_acao, "Falha ao monitorar conexoes de rede.");
                    strcat(log_resultados, "Erro ao executar netstat.\n");
                }
                break;
            }
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
                    sprintf(log_resultados, "IP %s adicionado com sucesso a whitelist.\n", entrada);
                } else {
                    sprintf(ultima_acao, "Falha ao adicionar IP %s a whitelist.", entrada);
                    sprintf(log_resultados, "Erro ao adicionar IP %s a whitelist.\n", entrada);
                }
                break;
            case 6:
                printf("Digite o IP para blacklist: ");
                scanf("%s", entrada);
                if (atualizar_lista("blacklist.txt", entrada)) {
                    sprintf(ultima_acao, "IP %s adicionado a blacklist.", entrada);
                    sprintf(log_resultados, "IP %s adicionado com sucesso a blacklist.\n", entrada);
                } else {
                    sprintf(ultima_acao, "Falha ao adicionar IP %s a blacklist.", entrada);
                    sprintf(log_resultados, "Erro ao adicionar IP %s a blacklist.\n", entrada);
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
        }
    }
}

int main() {
    menu();
    return 0;
}
