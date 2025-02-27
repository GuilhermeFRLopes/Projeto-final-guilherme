# Dispositivo Assistivo para Deficientes Visuais

## Descrição
Este projeto consiste em um **dispositivo assistivo** para auxiliar **deficientes visuais**, utilizando um **joystick analógico, sensores de proximidade e feedback tátil/auditivo** (buzzer e LED). Quando o joystick se aproxima de um objeto, o sistema emite sinais sonoros e luminosos para alertar o usuário.

## Funcionalidades
- **Leitura do joystick analógico** para determinar a posição do usuário.
- **Geração de objetos aleatórios** no ambiente simulado.
- **Cálculo da proximidade** entre o joystick e os objetos.
- **Feedback tátil e auditivo**:
  - O buzzer e o LED aumentam de intensidade conforme a aproximação dos objetos.
- **Exibição no display OLED** da posição do joystick e dos objetos.
- **Reinicialização dos objetos** ao pressionar um botão.

## Componentes Utilizados
### Hardware
- **Placa Raspberry Pi Pico** (BitDogLab)
- **Joystick analógico** (Eixos X e Y via ADC)
- **Buzzer (PWM)**
- **LED (PWM)**
- **Display OLED SSD1306** (Comunicação I2C)
- **Botões de controle** (GPIO Digital)

### Software
- **Linguagem:** C/C++
- **Bibliotecas:**
  - `pico/stdlib.h` (funções básicas do Raspberry Pi Pico)
  - `hardware/adc.h` (leitura do joystick)
  - `hardware/pwm.h` (controle do LED e buzzer)
  - `hardware/i2c.h` (comunicação com o display OLED)
  - `ssd1306.h` e `font.h` (manipulação do display OLED)
  - `math.h` (cálculo de distância)

## Configuração dos Pinos

| Componente       | GPIO  |
|-----------------|------|
| Joystick X      | 26   |
| Joystick Y      | 27   |
| Botão A        | 5    |
| Botão B        | 6    |
| LED Vermelho    | 13   |
| LED Verde       | 11   |
| Buzzer A        | 21   |
| Buzzer B        | 10   |
| Display OLED SDA | 14   |
| Display OLED SCL | 15   |

## Instalação e Execução
### 1. Compilação do Código
1. Instale o SDK do **Raspberry Pi Pico**.
2. Compile o código utilizando o `CMake`:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```

### 2. Carregamento no Raspberry Pi Pico
1. Conecte o Pico ao computador segurando **BOOTSEL**.
2. Copie o arquivo `.uf2` gerado para a unidade que aparece no explorador de arquivos.

### 3. Uso
- Mova o **joystick** para navegar no ambiente.
- O **buzzer e o LED** aumentarão de intensidade conforme a aproximação dos objetos.
- Pressione o **botão A** para **gerar novos objetos aleatórios**.
- Pressione o **botão B** para entrar no **modo BOOTSEL**.

## Funcionamento do Código
1. **Inicialização**
   - Configura PWM, ADC e GPIOs.
   - Inicializa o **display OLED**.
   - Gera os **objetos aleatórios**.
2. **Loop principal**
   - Lê os valores do joystick.
   - Atualiza o display OLED.
   - Calcula a proximidade com os objetos.
   - Ajusta a intensidade do LED e do buzzer.
   - Aguarda 100ms antes da próxima leitura.

## Melhorias Futuras
- **Adicionar sensores ultrassônicos** para medição real da distância.
- **Criar um modo de calibração** para ajustar a sensibilidade.
- **Conectar a um aplicativo via Bluetooth** para feedback remoto.

## Link do video
(https://youtu.be/5fK91k4SfLc)
##  Equipe de Desenvolvimento
| Nome | GitHub |
|------|--------|
| Guilherme Lopes | [@GuilhermeFRLopes](https://github.com/GuilhermeFRLopes) |

