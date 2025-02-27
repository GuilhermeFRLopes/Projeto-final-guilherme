#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h" // Biblioteca para controlar o hardware de PWM
#include <math.h>
#include "pico/time.h"

// Dispositivo Assistivo para Deficientes Visuais

// Definições de pinos e constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26 // GPIO para eixo X do joystick
#define JOYSTICK_Y_PIN 27 // GPIO para eixo Y do joystick
#define BOTAO_A 5         // GPIO para botão A

#define LED_R 13 // GPIO para o LED vermelho
#define LED_G 11 // GPIO para o LED verde

#define BUZZER_A 21 // GPIO para o buzzer A
#define BUZZER_B 10 // GPIO para o buzzer B

#define QUANTIDADE_OBJETOS 4 // Quantidade de objetos no ambiente simulado

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define BOTAO_B 6 // GPIO para botão B

// Estrutura para representar um objeto no ambiente simulado
typedef struct objeto
{
  int pos_x; // Posição X do objeto
  int pos_y; // Posição Y do objeto
} objeto;

objeto lista_objetos_proximos[QUANTIDADE_OBJETOS]; // Lista de objetos
int lenght = 0; // Contador de objetos gerados

const uint16_t WRAP_PERIOD = 4000; // Valor máximo do contador PWM
const float PWM_DIVISER = 4.0;     // Divisor do clock para o PWM
ssd1306_t ssd;                     // Estrutura do display OLED

// Função para gerar objetos aleatórios no ambiente simulado
void gerar_objetos(int quantidade, objeto lista_objetos[])
{
  srand(time_us_64()); // Inicializa o gerador de números aleatórios
  for (int i = 0; i < quantidade; i++)
  {
    int pos_x = (rand() % 4095); // Gera uma posição X aleatória entre 0 e 4095
    int pos_y = (rand() % 4095); // Gera uma posição Y aleatória entre 0 e 4095
    objeto ob;
    ob.pos_x = pos_x;
    ob.pos_y = pos_y;
    lenght += 1; // Incrementa o contador de objetos
    lista_objetos[i] = ob; // Armazena o objeto na lista
  }
}

// Função para tratar interrupções dos botões
void gpio_irq_handler(uint gpio, uint32_t events)
{
  // Debouncing para evitar leituras múltiplas de um único pressionamento
  static uint32_t last_time = 0;
  uint32_t current_time = to_ms_since_boot(get_absolute_time());
  if (current_time - last_time < 200)
  {
    return;
  }
  last_time = current_time;

  // Verifica qual botão foi pressionado
  if (gpio == BOTAO_B)
  {
    reset_usb_boot(0, 0); // Entra no modo BOOTSEL
  }
  else if (gpio == BOTAO_A)
  {
    ssd1306_fill(&ssd, 0); // Limpa o display
    lenght = 0; // Reinicia o contador de objetos
    gerar_objetos(4, lista_objetos_proximos); // Gera novos objetos
    ssd1306_send_data(&ssd); // Atualiza o display
  }
}

// Função para configurar o módulo PWM
void setup()
{
  // Configura os pinos do LED e dos buzzers como saídas PWM
  gpio_set_function(LED_R, GPIO_FUNC_PWM);
  gpio_set_function(BUZZER_A, GPIO_FUNC_PWM);
  gpio_set_function(BUZZER_B, GPIO_FUNC_PWM);

  // Obtém os canais PWM correspondentes aos pinos
  uint slice = pwm_gpio_to_slice_num(LED_R);
  uint slice2 = pwm_gpio_to_slice_num(BUZZER_A);
  uint slice3 = pwm_gpio_to_slice_num(BUZZER_B);

  // Configura o divisor de clock do PWM
  pwm_set_clkdiv(slice, PWM_DIVISER);
  pwm_set_clkdiv(slice2, PWM_DIVISER);
  pwm_set_clkdiv(slice3, PWM_DIVISER);

  // Define o valor de wrap (período do PWM)
  pwm_set_wrap(slice, WRAP_PERIOD);
  pwm_set_wrap(slice2, 4095);
  pwm_set_wrap(slice3, 4095);

  // Define o ciclo de trabalho inicial como 0 (desligado)
  pwm_set_gpio_level(LED_R, 0);
  pwm_set_gpio_level(BUZZER_A, 0);
  pwm_set_gpio_level(BUZZER_B, 0);

  // Habilita os canais PWM
  pwm_set_enabled(slice, true);
  pwm_set_enabled(slice2, true);
  pwm_set_enabled(slice3, true);

  // Configura o LED verde como saída
  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);

  // Configura o botão B para entrar no modo BOOTSEL
  gpio_init(BOTAO_B);
  gpio_set_dir(BOTAO_B, GPIO_IN);
  gpio_pull_up(BOTAO_B);

  // Habilita interrupções para os botões A e B
  gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // Configura o botão A como entrada com pull-up
  gpio_init(BOTAO_A);
  gpio_set_dir(BOTAO_A, GPIO_IN);
  gpio_pull_up(BOTAO_A);

  // Inicializa o I2C para comunicação com o display OLED
  i2c_init(I2C_PORT, 400 * 1000);

  // Configura os pinos SDA e SCL para I2C
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA); // Habilita pull-up na linha SDA
  gpio_pull_up(I2C_SCL); // Habilita pull-up na linha SCL

  // Inicializa o display OLED
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display (todos os pixels apagados)
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  // Inicializa o ADC para leitura do joystick
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN); // Configura o pino 26 como entrada analógica
  adc_gpio_init(JOYSTICK_Y_PIN); // Configura o pino 27 como entrada analógica
}

// Função para controlar a intensidade do LED e dos buzzers com base na proximidade dos objetos
void controle_sensores(int pos_x, int pos_y, objeto lista_objetos[])
{
  int intensidade_maxima = 0; // Intensidade máxima entre todos os objetos

  // Pequena margem para reduzir oscilações
  if (pos_x > 1900 && pos_x < 2050)
    pos_x = 2048;
  if (pos_y > 1900 && pos_y < 2050)
    pos_y = 2048;

  for (int i = 0; i < lenght; i++)
  {
    objeto ob = lista_objetos[i];

    // Calcula a distância entre o joystick e o objeto
    int distancia_x = abs(ob.pos_x - pos_x);
    int distancia_y = abs(ob.pos_y - pos_y);

    // Define a distância máxima para ativação
    int distancia_maxima = 1000;

    // Verifica se há colisão (coordenadas iguais)
    if (pos_x == ob.pos_x && pos_y == ob.pos_y)
    {
      intensidade_maxima = 3000; // Intensidade máxima em caso de colisão
      break;
    }
    // Verifica se está dentro da margem de ativação
    else if (distancia_x <= distancia_maxima && distancia_y <= distancia_maxima)
    {
      // Calcula a distância total
      int distancia_total = sqrt(distancia_x * distancia_x + distancia_y * distancia_y);

      // Mapeia a distância para a intensidade
      int intensidade = 3000 - (distancia_total * 3000 / distancia_maxima);

      // Atualiza a intensidade máxima, se necessário
      if (intensidade > intensidade_maxima)
      {
        intensidade_maxima = intensidade;
      }
    }
  }

  // Aplica a intensidade máxima ao LED e aos buzzers
  pwm_set_gpio_level(LED_R, intensidade_maxima);
  pwm_set_gpio_level(BUZZER_A, intensidade_maxima);
  pwm_set_gpio_level(BUZZER_B, intensidade_maxima);
}

// Função principal
int main()
{
  stdio_init_all(); // Inicializa o sistema padrão de I/O
  setup();          // Configurações iniciais

  // Gera os objetos iniciais
  gerar_objetos(4, lista_objetos_proximos);

  uint16_t pessoa_pos_x;
  uint16_t pessoa_pos_y;

  bool cor = true;
  while (true)
  {
    // Lê as coordenadas do joystick
    adc_select_input(0); // Seleciona o ADC para eixo X
    pessoa_pos_x = adc_read();
    adc_select_input(1); // Seleciona o ADC para eixo Y
    pessoa_pos_y = adc_read();

    // Converte os valores do ADC para coordenadas do display
    int pos_x = 56 - ((pessoa_pos_x * 56) / 4095); // Mapeia X para [0,120]
    int pos_y = (pessoa_pos_y * 120) / 4095;       // Mapeia Y para [0,56]

    // Atualiza o conteúdo do display
    ssd1306_fill(&ssd, !cor); // Limpa o display

    // Desenha o quadrado do joystick
    ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, cor, 1);

    // Desenha os objetos no display
    for (int i = 0; i < lenght; i++)
    {
      int cord_objeto1_x = 56 - ((lista_objetos_proximos[i].pos_x * 56) / 4095);
      int cord_objeto1_y = (lista_objetos_proximos[i].pos_y * 120) / 4095;
      ssd1306_rect(&ssd, cord_objeto1_x, cord_objeto1_y, 8, 8, cor, 1);
    }

    // Controla a intensidade do LED e dos buzzers
    controle_sensores(pessoa_pos_x, pessoa_pos_y, lista_objetos_proximos);

    // Atualiza o display
    ssd1306_send_data(&ssd);
    sleep_ms(100); // Aguarda 100 ms antes de repetir o loop
  }
}