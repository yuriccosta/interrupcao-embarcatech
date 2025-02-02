# Projeto de Controle de LEDs com Interrupções - BitDogLab

## Visão Geral
Este projeto utiliza a placa BitDogLab com o microcontrolador RP2040 para controlar uma matriz de LEDs WS2812 e um LED RGB, implementando interrupções para tratar eventos de botões. O sistema permite incrementar/decrementar um número exibido na matriz e inclui debouncing via software.

## Funcionalidades
- **LED RGB**: Pisca o LED vermelho a 5Hz.
- **Botões A e B**: 
  - **Botão A**: Incrementa o número exibido (0-9).
  - **Botão B**: Decrementa o número exibido (0-9).
- **Matriz de LEDs WS2812**: Exibe números de 0 a 9 em formato digital com LEDs vermelhos.
- **Debouncing**: Tratamento de bouncing dos botões via software (200ms de delay).
- **Interrupções**: Uso de rotinas de interrupção para detectar pressionamentos.

## Requisitos
### Hardware
- Placa BitDogLab com RP2040.
- Matriz de LEDs WS2812 (5x5) conectada ao GPIO 7.
- LED RGB (vermelho no GPIO 13).
- Botões A (GPIO 5) e B (GPIO 6).

## Uso
### Funcionamento Básico
- **Inicialização**: A matriz exibe o número **0** e o LED vermelho começa a piscar.  
- **Botão A**:  
  - Pressione → Número incrementa (ex: 0 → 1).  
  - Ao chegar em 9, retorna para 0.  
- **Botão B**:  
  - Pressione → Número decrementa (ex: 1 → 0).  
  - Ao chegar em 0, retorna para 9.  

### Estrutura do Código
- **Interrupções**:  
  Tratadas na função `gpio_irq_handler` (verificação de bouncing e atualização do número).  
- **Padrões dos Números**:  
  Definidos na matriz `padrao_led`, onde cada posição representa um LED aceso (1) ou apagado (0).  
- **Controle da Matriz**:  
  A função `display_num` renderiza o número atual na matriz usando a cor vermelha.  

## Vídeo de Demonstração
Assista à demonstração do projeto em execução:  
[Vídeo Demonstrativo](https://youtu.be/iqrLGlgJekE)  
