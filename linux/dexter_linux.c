#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Função para executar comandos no terminal
void executar_comando(const char *comando) {
    system(comando);
}

// Função para monitorar processos ativos
void monitorar_processos() {
    executar_comando("ps -e");
}

// Função para monitorar conexões de rede
void monitorar_conexoes() {
    executar_comando("ss -tulnp");
}

// Função para verificar malwares
void verificar_malware() {
    printf("Iniciando verificação de malwares...\n");
    executar_comando("find / -type f > file_list.txt");
    printf("Verificação concluída. Verifique o arquivo file_list.txt\n");
}

// Função para bloquear IPs da blacklist
void bloquear_ips() {
    FILE *file = fopen("blacklist.txt", "r");
    char ip[100];
    if (file) {
        while (fgets(ip, sizeof(ip), file) != NULL) {
            ip[strcspn(ip, "\n")] = 0;
            char command[200];
            snprintf(command, sizeof(command), "sudo iptables -A INPUT -s %s -j DROP", ip);
            executar_comando(command);
            printf("Bloqueado: %s\n", ip);
        }
        fclose(file);
        printf("Bloqueio de IPs concluído.\n");
    } else {
        printf("Erro ao abrir blacklist.txt\n");
    }
}

// Função para desbloquear os IPs
void desbloquear_ips() {
    executar_comando("sudo iptables -F");
    printf("Desbloqueio realizado.\n");
}

// Função para atualizar a whitelist/blacklist
int atualizar_lista(const char *arquivo, const char *entrada) {
    FILE *file = fopen(arquivo, "a");
    if (file) {
        fprintf(file, "%s\n", entrada);
        fclose(file);
        printf("%s adicionado à lista %s\n", entrada, arquivo);
        return 1;
    } else {
        printf("Erro ao abrir a lista %s\n", arquivo);
        return 0;
    }
}

// Menu principal
void menu() {
    int opcao;
    char entrada[100];
    char ultima_acao[256] = "Nenhuma ação realizada ainda.";

    while (1) {
        printf("* 8 888888888o.      8 8888888888   `8.`8888.      ,8' 8888888 8888888888 8 8888888888   8 888888888o. *\n* 8 8888    `^888.   8 8888          `8.`8888.    ,8'        8 8888       8 8888         8 8888    `88. *\n* 8 8888        `88. 8 8888           `8.`8888.  ,8'         8 8888       8 8888         8 8888     `88 *\n* 8 8888         `88 8 8888            `8.`8888.,8'          8 8888       8 8888         8 8888     ,88 *\n* 8 8888          88 8 888888888888     `8.`88888'           8 8888       8 888888888888 8 8888.   ,88' *\n* 8 8888          88 8 8888             .88.`8888.           8 8888       8 8888         8 888888888P' *\n* 8 8888         ,88 8 8888            .8'`8.`8888.          8 8888       8 8888         8 8888`8b *\n* 8 8888        ,88' 8 8888           .8'  `8.`8888.         8 8888       8 8888         8 8888 `8b.\n* 8 8888    ,o88P'   8 8888          .8'    `8.`8888.        8 8888       8 8888         8 8888   `8b.\n* 8 888888888P'      8 888888888888 .8'      `8.`8888.       8 8888       8 888888888888 8 8888     `88. *\n\n\n");
        printf("#------------------------------------#\n");
        printf("\t Antivirus Options:\n");
        printf("#------------------------------------#\n");
        printf("1 - Monitorar processos ativos\n");
        printf("2 - Monitorar conexões de rede\n");
        printf("3 - Verificar malware\n");
        printf("4 - Adicionar IP à whitelist\n");
        printf("5 - Adicionar IP à blacklist\n");
        printf("6 - Bloquear IPs da blacklist\n");
        printf("7 - Desbloquear IPs\n");
        printf("8 - Sair\n");
        printf("\nÚltima ação: %s\n", ultima_acao);
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                monitorar_processos();
                strcpy(ultima_acao, "Monitoramento de processos realizado.");
                break;
            case 2:
                monitorar_conexoes();
                strcpy(ultima_acao, "Monitoramento de conexões realizado.");
                break;
            case 3:
                verificar_malware();
                strcpy(ultima_acao, "Verificação de malware realizada.");
                break;
            case 4:
                printf("Digite o IP para a whitelist: ");
                scanf("%s", entrada);
                atualizar_lista("whitelist.txt", entrada);
                strcpy(ultima_acao, "IP adicionado à whitelist.");
                break;
            case 5:
                printf("Digite o IP para a blacklist: ");
                scanf("%s", entrada);
                atualizar_lista("blacklist.txt", entrada);
                strcpy(ultima_acao, "IP adicionado à blacklist.");
                break;
            case 6:
                bloquear_ips();
                strcpy(ultima_acao, "Bloqueio de IPs realizado.");
                break;
            case 7:
                desbloquear_ips();
                strcpy(ultima_acao, "Desbloqueio de IPs realizado.");
                break;
            case 8:
                exit(0);
            default:
                printf("Opção inválida.\n");
                strcpy(ultima_acao, "Opção inválida selecionada.");
                break;
        }
    }
}

int main() {
    menu();
    return 0;
}
