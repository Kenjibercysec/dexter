#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Executa comandos no terminal
void executar_comando(const char *comando) {
    system(comando);
}

// Monitora processos ativos
void monitorar_processos() {
    executar_comando("ps -e");
}

// Monitora conexoes de rede
void monitorar_conexoes() {
    executar_comando("ss -tulnp");
}

// Verifica entradas na tabela ARP para detectar ataques MITM
void monitorar_arp() {
    printf("Verificando tabela ARP (possivel MITM)...\n");
    executar_comando("arp -a");
}

// Verificação de malware com wordlist e extensões
void verificar_malware() {
    printf("Iniciando verificacao de malwares...\n");

    // Recomenda rodar como root para acesso completo
    if (geteuid() != 0) {
        printf("[!] Aviso: Recomendado rodar como root para acesso total aos arquivos.\n");
        printf("[!] Use: sudo ./seu_programa\n");
    }

    // Escaneia o sistema e gera file_list.txt
    printf("Escaneando arquivos do sistema...\n");
    executar_comando("sudo find / -type f 2>/dev/null > file_list.txt");

    // Abre a lista de arquivos encontrados
    FILE *lista_arquivos = fopen("file_list.txt", "r");
    // Abre a wordlist no caminho absoluto que você salvou
    FILE *wordlist      = fopen("/home/kenji/Desktop/dexter/linux/output/malware_wordlist.txt", "r");

    if (!lista_arquivos || !wordlist) {
        printf("Erro ao abrir arquivos de verificação.\n");
        if (lista_arquivos) fclose(lista_arquivos);
        if (wordlist)      fclose(wordlist);
        return;
    }

    char arquivo[1024];
    char malware[256];

    // Extensões suspeitas
    const char *extensoes_suspeitas[] = {
        ".exe", ".bat", ".cmd", ".vbs", ".vbe", ".js", ".jse", ".ps1",
        ".psm1", ".scr", ".com", ".pif", ".inf", ".drv", ".sys",
        ".msi", ".lnk", ".cpl", ".url", ".hta", ".ws",  ".wsf", ".sh"
    };
    int total_extensoes = sizeof(extensoes_suspeitas) / sizeof(extensoes_suspeitas[0]);

    printf("Analisando arquivos encontrados...\n");

    while (fgets(arquivo, sizeof(arquivo), lista_arquivos)) {
        arquivo[strcspn(arquivo, "\n")] = 0;

        // 1) Verificação por nome na wordlist
        rewind(wordlist);
        while (fgets(malware, sizeof(malware), wordlist)) {
            malware[strcspn(malware, "\n")] = 0;
            if (strstr(arquivo, malware)) {
                printf("[!] Nome suspeito encontrado: %s (corresponde a: %s)\n",
                       arquivo, malware);
            }
        }

        // 2) Verificação por extensão
        char *extensao = strrchr(arquivo, '.');
        if (extensao) {
            for (int i = 0; i < total_extensoes; i++) {
                if (strcmp(extensao, extensoes_suspeitas[i]) == 0) {
                    printf("[!] Extensao suspeita encontrada: %s\n", arquivo);
                    break;
                }
            }
        }
    }

    fclose(lista_arquivos);
    fclose(wordlist);
    printf("Verificacao concluida.\n");
}

// Bloqueia IPs listados no arquivo blacklist.txt
void bloquear_ips() {
    FILE *file = fopen("blacklist.txt", "r");
    char ip[100];
    if (file) {
        while (fgets(ip, sizeof(ip), file)) {
            ip[strcspn(ip, "\n")] = 0;
            char command[200];
            snprintf(command, sizeof(command),
                     "sudo iptables -A INPUT -s %s -j DROP", ip);
            executar_comando(command);
            printf("Bloqueado: %s\n", ip);
        }
        fclose(file);
        printf("Bloqueio de IPs concluido.\n");
    } else {
        printf("Erro ao abrir blacklist.txt\n");
    }
}

// Limpa as regras do iptables
void desbloquear_ips() {
    executar_comando("sudo iptables -F");
    printf("Desbloqueio realizado.\n");
}

// Adiciona IP a uma lista (whitelist ou blacklist)
int atualizar_lista(const char *arquivo, const char *entrada) {
    FILE *file = fopen(arquivo, "a");
    if (file) {
        fprintf(file, "%s\n", entrada);
        fclose(file);
        printf("%s adicionado a lista %s\n", entrada, arquivo);
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
    char ultima_acao[256]     = "Nenhuma acao realizada ainda.";
    char log_resultados[1024] = "Nenhum log registrado ainda.";

    while (1) {
        // Limpa a tela antes de exibir o menu
        system("clear");

        const char *ascii[] = {
            "                      -___________-          ",
            "                     (/     _     \\)         ",
            "                     /_____(O)_____\          --DAAALEK BE BALLIN",
            "                     // / / | \\ \\ \\\\         ",
            "                    =================        ",
            "                    // / | | | | \\ \\\\        ",
            "                   ===================       ",
            "                  //// || || || || \\\\\\\\      ",
            "                  |||| || || || || ||||      ",
            "                 /---___-----------,---\\     ",
            "                 |  /   \\         -o-  |     ",
            "                 /  \\___/          '   \\     ",
            "                 +---------------------+     ",
            "                /_   __    ___    __   _\\    ",
            "               (__) (__)  (___)  (__) (__)   ",
            "               |_    __    ___    __    _|   ",
            "              (__)  (__)  (___)  (__)  (__)  ",
            "              /_    ___    ___    ___    _\\  ",
            "             (__)  (___)  (___)  (___)  (__) ",
            "             |_     ___    ___    ___     _| ",
            "            (__)   (___)  (___)  (___)   (__)",
            "            /_______________________________\\"
        };
        const char *menu_linhas[] = {
            "#------------------------------------#",
            "           Antivirus Options:",
            "#------------------------------------#",
            "1  - Monitorar processos ativos",
            "2  - Monitoramento de conexoes",
            "3  - Monitorar tabela ARP",
            "4  - Verificar malware",
            "5  - Adicionar a whitelist",
            "6  - Adicionar a blacklist",
            "7  - Bloquear IPs da blacklist",
            "8  - Desbloquear IPs da blacklist",
            "9 - Sair",
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
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                monitorar_processos();
                strcpy(ultima_acao, "Monitoramento de processos realizado.");
                break;
            case 2:
                monitorar_conexoes();
                strcpy(ultima_acao, "Monitoramento de conexoes realizado.");
                break;
            case 3:
                monitorar_arp();
                strcpy(ultima_acao, "Verificacao da tabela ARP realizada.");
                break;
            case 4:
                verificar_malware();
                strcpy(ultima_acao, "Verificacao de malware realizada.");
                break;
            case 5:
                printf("Digite o IP para a whitelist: ");
                scanf("%s", entrada);
                atualizar_lista("whitelist.txt", entrada);
                strcpy(ultima_acao, "IP adicionado a whitelist.");
                break;
            case 6:
                printf("Digite o IP para a blacklist: ");
                scanf("%s", entrada);
                atualizar_lista("blacklist.txt", entrada);
                strcpy(ultima_acao, "IP adicionado a blacklist.");
                break;
            case 7:
                bloquear_ips();
                strcpy(ultima_acao, "Bloqueio de IPs realizado.");
                break;
            case 8:
                desbloquear_ips();
                strcpy(ultima_acao, "Desbloqueio de IPs realizado.");
                break;
            case 9:
                exit(0);
            default:
                printf("Opcao invalida.\n");
                strcpy(ultima_acao, "Opcao invalida selecionada.");
                break;
        }

        // Pausa antes de atualizar o menu
        printf("\nPressione Enter para continuar...");
        getchar(); // Captura o '\n' deixado pelo scanf
        getchar(); // Aguarda o usuário pressionar Enter
    }
}

int main() {
    menu();
    return 0;
}