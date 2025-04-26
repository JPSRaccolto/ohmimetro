
 #include <stdio.h>
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/adc.h"
 #include "hardware/i2c.h"
 #include "ssd1306.h"
 #include "font.h"
 #include <math.h>

 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define endereco 0x3C
 #define ADC_PIN 28 // GPIO para o voltímetro
 #define Botao_A 5  // GPIO para botão A
 
 int R_conhecido = 10000;   // Resistor de 10k ohm
 float R_x = 0.0;           // Resistor desconhecido
 float ADC_VREF = 3.31;     // Tensão de referência do ADC
 int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
 ssd1306_t ssd;                
 // Trecho para modo BOOTSEL com botão B
 #include "pico/bootrom.h"
 #define botaoB 6
 void gpio_irq_handler(uint gpio, uint32_t events)
 {
   reset_usb_boot(0, 0);
 }
 //vetor com valores que definem os valores da faixa E24
 const int resistores_E24[] = {
    510, 560, 620, 680, 750, 820, 910,
    
    1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000,
    2200, 2400, 2700, 3000, 3300, 3600, 3900, 4300,
    4700, 5100, 5600, 6200, 6800, 7500, 8200, 9100,

    10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000,
    22000, 24000, 27000, 30000, 33000, 36000, 39000, 43000,
    47000, 51000, 56000, 62000, 68000, 75000, 82000, 91000,

    100000 
};
void faixa_1_2(int mp, float R_x){
 char real[10];
 //determina a faixa 1
 //trasformei o valor inteiro mp em string e capturei a primeira e segunda posição do vetor para
 //descrobrir qual a faixa
 sprintf(real, "%d", mp); 
 
 if (R_x > 500 && R_x <= 100000) {
    printf("Faixa 1:");
    switch(real[0]) {  
        case '0': 
            printf(" Preto\n");
            ssd1306_draw_string(&ssd, "Preto", 59, 6); 
            break;
        case '1':
            printf(" Marrom\n");  
            ssd1306_draw_string(&ssd, "Marrom", 59, 6); 
            break;
        case '2':
            printf(" Vermelho\n");
            ssd1306_draw_string(&ssd, "Vermelho", 59, 6); 
            break;
        case '3':
            printf(" Laranja\n");
            ssd1306_draw_string(&ssd, "Laranja", 59, 6); 
            break;
        case '4':
            printf(" Amarelo\n");
            ssd1306_draw_string(&ssd, "Amarelo", 59, 6);
            break;
        case '5':
            printf(" Verde\n");
            ssd1306_draw_string(&ssd, "Verde", 59, 6);
            break;
        case '6':
            printf(" Azul\n");
            ssd1306_draw_string(&ssd, "Azul", 59, 6);
            break;
        case '7':
            printf(" Violeta\n");
            ssd1306_draw_string(&ssd, "Violeta", 59, 6);
            break;
        case '8':
            printf(" Cinza\n");
            ssd1306_draw_string(&ssd, "Cinza", 59, 6);
            break;
        case '9':
            printf(" Branco\n");
            ssd1306_draw_string(&ssd, "Branco", 59, 6);
            break;
        default:
            printf(" Inválido\n");
            ssd1306_draw_string(&ssd, "Invalido", 59, 6);
            break;
        }
    //determina a faixa 2
    printf("Faixa 2:");
    switch(real[1]) {  
        case '0': 
            printf(" Preto\n");
            ssd1306_draw_string(&ssd, "Preto", 59, 16); 
            break;
        case '1':
            printf(" Marrom\n");  
            ssd1306_draw_string(&ssd, "Marrom", 59, 16); 
            break;
        case '2':
            printf(" Vermelho\n");
            ssd1306_draw_string(&ssd, "Vermelho", 59, 16); 
            break;
        case '3':
            printf(" Laranja\n");
            ssd1306_draw_string(&ssd, "Laranja", 59, 16); 
            break;
        case '4':
            printf(" Amarelo\n");
            ssd1306_draw_string(&ssd, "Amarelo", 59, 16);
            break;
        case '5':
            printf(" Verde\n");
            ssd1306_draw_string(&ssd, "Verde", 59, 16);
            break;
        case '6':
            printf(" Azul\n");
            ssd1306_draw_string(&ssd, "Azul", 59, 16);
            break;
        case '7':
            printf(" Violeta\n");
            ssd1306_draw_string(&ssd, "Violeta", 59, 16);
            break;
        case '8':
            printf(" Cinza\n");
            ssd1306_draw_string(&ssd, "Cinza", 59, 16);
            break;
        case '9':
            printf(" Branco\n");
            ssd1306_draw_string(&ssd, "Branco", 59, 16);
            break;
        default:
            printf(" Inválido\n");
            ssd1306_draw_string(&ssd, "Invalido", 59, 16);
            break;
        }
    }
    if(R_x < 500 || R_x > 100000){
        printf("Faixa 1°: Inválido\n");
        printf("Faixa 2°: Inválido\n");
        ssd1306_draw_string(&ssd, "Invalido", 59, 6);
        ssd1306_draw_string(&ssd, "Invalido", 59, 16);
    }
}
//função que determina a faixa 3, enquanto o valor do resistor for maior que 100 ele vai dividindo por 10
// e adicionando +1 no somador.
void faixa_3(int mp, float R_x) {
 int potencia = 0;
 
 while(mp >= 100){
    mp /= 10;
    potencia++;
 }
 if(R_x < 100000){
    switch (potencia) {
        case 0: 
            printf("Faixa 3: Preto (×1Ω)\n");
            ssd1306_draw_string(&ssd, "Preto", 59, 28);
            break;
        case 1: 
            printf("Faixa 3: Marrom (×10Ω)\n");
            ssd1306_draw_string(&ssd, "Marrom", 59, 28);
            break;
        case 2: 
            printf("Faixa 3: Vermelho (×100Ω)\n");
            ssd1306_draw_string(&ssd, "Vermelho", 59, 28);
            break;
        case 3:
            printf("Faixa 3°: Laranja (×1kΩ)\n");
            ssd1306_draw_string(&ssd, "Laranja", 59, 28);
            break;
        case 4: 
            printf("Faixa 3°: Amarelo (×10kΩ)\n");
            ssd1306_draw_string(&ssd, " Amarelo", 59, 28);
            break;
        case 5: 
            printf("Faixa 3°: Verde (×100kΩ)\n");
            ssd1306_draw_string(&ssd, "Verde", 59, 28);
            break;
        default:
            printf("Faixa 3°: Inválido\n");
            ssd1306_draw_string(&ssd, "Invalido", 59, 28);
            break;
        }
    }
    if(R_x > 100000){
        printf("Faixa 3°: Inválido\n");
        ssd1306_draw_string(&ssd, "Invalido", 59, 28);

    }
}
 int main()
 {
   // Para ser utilizado o modo BOOTSEL com botão B
   gpio_init(botaoB);
   gpio_set_dir(botaoB, GPIO_IN);
   gpio_pull_up(botaoB);
   gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
   // Aqui termina o trecho para modo BOOTSEL com botão B
   stdio_init_all();
   gpio_init(Botao_A);
   gpio_set_dir(Botao_A, GPIO_IN);
   gpio_pull_up(Botao_A);
 
   // I2C Initialisation. Using it at 400Khz.
   i2c_init(I2C_PORT, 400 * 1000);
 
   gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
   gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
   gpio_pull_up(I2C_SDA);                                        // Pull up the data line
   gpio_pull_up(I2C_SCL);                                        // Pull up the clock line                                // Inicializa a estrutura do display
   ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
   ssd1306_config(&ssd);                                         // Configura o display
   ssd1306_send_data(&ssd);                                      // Envia os dados para o display
 
   // Limpa o display. O display inicia com todos os pixels apagados.
   ssd1306_fill(&ssd, false);
   ssd1306_send_data(&ssd);
 
   adc_init();
   adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica
 
   float tensao;
   char str_x[5]; // Buffer para armazenar a string
   char str_y[5]; // Buffer para armazenar a string
 
   bool cor = true;
   while (true)
   {
     adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica
 
     float soma = 0.0f;
     for (int i = 0; i < 1000; i++)
     {
       soma += adc_read();
       sleep_ms(1);
     }
     float media = soma / 1000.0f;
 
        // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);
        float menor = fabs(R_x - resistores_E24[0]);
        int mp = resistores_E24[0];  // Inicializa com o primeiro resistor
        
        for(int i = 1; i < 56; i++) {  // Começa de 1, pois o 0 já foi considerado
            float diferenca = fabs(R_x - resistores_E24[i]);
            if(diferenca < menor) {
                menor = diferenca;      // Atualiza a menor diferença
                mp = resistores_E24[i]; // Atualiza o resistor mais próximo
            }
        }


     if(R_x > 100000){
        R_x = 999999;
        media = 4095;
        sprintf(str_x, "%1.0f", media); // Converte o inteiro em string
        sprintf(str_y, "%1.0f", R_x);   // Converte o float em string
        printf("Valor real do resistor: Inválido\n");
        printf("\n=================================\n");
     }
     else if(R_x < 500){
        R_x = 0;
        media = 0;
        sprintf(str_x, "%1.0f", media); // Converte o inteiro em string
        sprintf(str_y, "%1.0f", R_x);   // Converte o float em string
        printf("Valor real do resistor: Inválido\n");
        printf("\n=================================\n");
     }
     else{
        printf("Valor real do resistor: %dΩ\n", mp);
        printf("\n=================================\n");
        sprintf(str_x, "%1.0f", media); // Converte o inteiro em string
        sprintf(str_y, "%1.0f", R_x);   // Converte o float em string
     }
     //  Atualiza o conteúdo do display com animações
     ssd1306_fill(&ssd, !cor);                          // Limpa o display
     ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
     ssd1306_line(&ssd, 3, 25, 123, 25, cor);           // Desenha uma linha
     ssd1306_line(&ssd, 3, 37, 123, 37, cor);           // Desenha uma linha
     ssd1306_line(&ssd, 3, 14, 123, 14, cor);           // Desenha uma linha
     ssd1306_draw_string(&ssd, "Cor 1:", 8, 6); // Desenha uma string
     ssd1306_draw_string(&ssd, "Cor 2:", 8, 16);  // Desenha uma string
     ssd1306_draw_string(&ssd, "Cor 3:", 8, 28);  // Desenha uma string
     ssd1306_draw_string(&ssd, "ADC", 13, 41);          // Desenha uma string
     ssd1306_draw_string(&ssd, "Resisten.", 50, 41);    // Desenha uma string
     ssd1306_line(&ssd, 44, 37, 44, 60, cor);           // Desenha uma linha vertical
     ssd1306_draw_string(&ssd, str_x, 8, 52);           // Desenha uma string
     ssd1306_draw_string(&ssd, str_y, 59, 52);          // Desenha uma string
     faixa_1_2(mp, R_x);              // escreve faixas 1 e 2
     faixa_3(mp, R_x);                // escreve faixa 3
     ssd1306_send_data(&ssd);                           // Atualiza o display
     sleep_ms(700);
   }
 }

