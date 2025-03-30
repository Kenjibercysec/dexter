#include<stdio.h>
#include<stdlib.h>
//#include<curl/curl.h>
#include<windows.h>


void pressKey(WORD vk) {
    keybd_event(vk, 0, 0, 0);               // Pressiona a tecla
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);  // Solta a tecla
}


void typeText(const char *text) {
    for (int i = 0; i < strlen(text); i++) {
        // Converte o caractere para o código virtual
        WORD vk = VkKeyScan(text[i]);

        // Se a tecla precisar de SHIFT (ex: letras maiúsculas), segura o SHIFT
        if ((vk & 0x0100) != 0) {
            keybd_event(VK_SHIFT, 0, 0, 0);  // Pressiona SHIFT
        }

        // Pressiona e solta a tecla correspondente
        pressKey(vk & 0xFF);

        // Solta o SHIFT, se necessário
        if ((vk & 0x0100) != 0) {
            keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);  // Solta SHIFT
        }

        Sleep(50);  // Pequeno delay para simular digitação humana
    }
}

int main(){
//  Arte principal da ferramenta  
int command; 
printf("* 8 888888888o.      8 8888888888   `8.`8888.      ,8' 8888888 8888888888 8 8888888888   8 888888888o. *\n* 8 8888    `^888.   8 8888          `8.`8888.    ,8'        8 8888       8 8888         8 8888    `88. *\n* 8 8888        `88. 8 8888           `8.`8888.  ,8'         8 8888       8 8888         8 8888     `88 *\n* 8 8888         `88 8 8888            `8.`8888.,8'          8 8888       8 8888         8 8888     ,88 *\n* 8 8888          88 8 888888888888     `8.`88888'           8 8888       8 888888888888 8 8888.   ,88' *\n* 8 8888          88 8 8888             .88.`8888.           8 8888       8 8888         8 888888888P' *\n* 8 8888         ,88 8 8888            .8'`8.`8888.          8 8888       8 8888         8 8888`8b *\n* 8 8888        ,88' 8 8888           .8'  `8.`8888.         8 8888       8 8888         8 8888 `8b.\n* 8 8888    ,o88P'   8 8888          .8'    `8.`8888.        8 8888       8 8888         8 8888   `8b.\n* 8 888888888P'      8 888888888888 .8'      `8.`8888.       8 8888       8 888888888888 8 8888     `88. *\n\n\n");



while(1){
printf("#------------------------------------#\n");
printf("\tOptions:\n");
printf("#------------------------------------#\n\n");
printf("1 - Neofetch comand in W11\n");
scanf("%d", &command);
switch (command)
{
case 1:
 // Abre o Executar (Win + R)
 keybd_event(VK_LWIN, 0, 0, 0);
 pressKey('R');
 keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
 Sleep(500);

 // Digita o comando "cmd" e aperta Enter
 typeText("cmd");
 pressKey(VK_RETURN);

 Sleep(4000);

 typeText("neofetch");
 pressKey(VK_RETURN);
    break;

default:
    break;
}


}
}