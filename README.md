# stm32-automacao-simples

Firmware para controlar uma carga (relé, motor, lâmpada) via **comando serial** ou **botão físico**, usando um microcontrolador STM32G071.

---

## O que faz

- Liga/desliga uma saída GPIO ao receber um comando de texto pela porta serial
- Também liga/desliga ao pressionar um botão físico conectado ao pino de controle
- Responde com mensagem de confirmação na serial
- Watchdog interno mantém o sistema funcionando mesmo em caso de travamento

---

## Hardware necessário

| Item | Detalhe |
|------|---------|
| Microcontrolador | STM32G071KBTX (ou outro STM32G0 com pinagem compatível) |
| Gravador | ST-Link V2 (via SWD) |
| Interface serial | Conversor USB-Serial (FTDI, CH340, CP2102 etc.) conectado na USART2 |
| Carga | Relé, LED, motor — o que for controlado pelo pino de saída |
| Botão/chave | Conectado ao pino CONTROL — ativa com borda de descida |

---

## Pinagem (STM32G071KBTX)

| Pino | Função | Direção | Descrição |
|------|--------|---------|-----------|
| PA2 | USART2_TX | Saída | Transmissão serial (conecte ao RX do conversor USB-Serial) |
| PA3 | USART2_RX | Entrada | Recepção serial (conecte ao TX do conversor USB-Serial) |
| PB5 | GPIO_LOAD | Saída | Controla a carga (HIGH = ligado, LOW = desligado) |
| PB7 | CONTROL | Entrada | Botão/chave física — borda de descida faz toggle da carga |
| PB1 | GPIO_3 | Saída | GPIO disponível para expansão |
| PB6 | GPIO_5 | Saída | GPIO disponível para expansão |
| PB8 | GPIO_7 | Saída | GPIO disponível para expansão |
| PA11 | GPIO_2 | Saída | GPIO disponível para expansão |
| PA12 | GPIO_1 | Saída | GPIO disponível para expansão |

---

## Comandos seriais

Configure o terminal com **115200 baud, 8N1, sem controle de fluxo**.

| Comando (PT) | Comando (EN) | Ação | Resposta |
|--------------|--------------|------|----------|
| `liga` | `turn on` | Liga a carga | `LOAD WAS TURNED ON` |
| `desliga` | `turn off` | Desliga a carga | `LOAD WAS TURNED OFF` |
| — | — | Botão físico pressionado | `LOAD WAS TOGGLED` |
| *(inválido)* | *(invalid)* | Comando não reconhecido | `EXPRESSION WAS NOT RECOGNIZED` |

- Os comandos não diferenciam maiúsculas/minúsculas (`LIGA`, `Liga`, `liga` funcionam igual)
- O buffer aceita até 20 caracteres; se estourar sem comando válido, é limpo automaticamente

---

## Como compilar

1. Instale o [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Abra o projeto: **File → Open Projects from File System → selecione a pasta `AUTOMATIZA_SIMPLES`**
3. O arquivo `.ioc` contém toda a configuração do STM32CubeMX (clock, pinos, periféricos)
4. Compile com **Project → Build Project** (Ctrl+B)

---

## Como gravar

Com o ST-Link conectado via SWD (pinos SWDIO, SWDCLK, GND, 3.3V):

1. No STM32CubeIDE: **Run → Debug** (F11) ou **Run → Run** (Ctrl+F11)
2. Ou use o [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) e carregue o `.elf` gerado na pasta `Debug/` ou `Release/`

---

## Estrutura do projeto

```
AUTOMATIZA_SIMPLES/
├── Core/
│   ├── Inc/              # Cabeçalhos HAL gerados pelo CubeMX (main.h, etc.)
│   ├── Src/              # Fontes HAL gerados pelo CubeMX (main.c, etc.)
│   ├── Startup/          # Código de inicialização (startup_stm32g071kbtx.s)
│   ├── userInc/
│   │   └── loadControl.h # Interface pública: inicialização e controle da carga
│   └── userSrc/
│       └── loadControl.c # Lógica de controle: UART interrupt, GPIO, comandos
├── Drivers/              # HAL e CMSIS do STM32G0 (gerado pelo CubeMX)
├── AUTOMATIZA_SIMPLES.ioc  # Configuração do STM32CubeMX
└── STM32G071KBTX_FLASH.ld  # Linker script
```

---

## Detalhes técnicos

- **Clock:** 64 MHz via PLL (HSI 16 MHz × 8 ÷ 2)
- **UART:** interrupção de recepção caractere a caractere (`HAL_UART_Receive_IT`); transmissão também via interrupção (`HAL_UART_Transmit_IT`) — não bloqueia o loop principal
- **Watchdog (IWDG):** timeout aproximado de 500 ms; reiniciado a cada iteração do loop principal
- **Botão:** interrupção EXTI na borda de descida — não requer polling

---

## Licença

Código de usuário (arquivos em `userSrc/` e `userInc/`): **MIT** — uso livre, inclusive comercial.

Os arquivos em `Core/Src/`, `Core/Inc/`, `Drivers/` e `Startup/` são gerados pelo STM32CubeIDE/HAL e licenciados pela STMicroelectronics (ver cabeçalho de cada arquivo).
