  # Analisador de Frequência com Matriz de LEDs
  VIDEO DO FUNCIONAMENTO: https://youtu.be/aFTzxG1ZHOc


## Descrição
Este projeto implementa um analisador de frequência que capta sinais sonoros, realiza uma análise espectral utilizando a Transformada Rápida de Fourier (FFT) e exibe a intensidade das faixas de frequência em uma matriz de LEDs WS2812. O sistema utiliza um microfone, um microcontrolador com conversor analógico-digital (ADC) e Direct Memory Access (DMA) para processar os dados de forma eficiente.

## Funcionalidades
- Captura de som ambiente por meio de um microfone.
- Digitalização do sinal utilizando ADC.
- Uso da biblioteca **KISS FFT** para extração das componentes de frequência.
- Mapeamento das frequências analisadas em uma matriz de LEDs 5x5.
- Representação visual das faixas de frequência com base na intensidade do som.

## Componentes Utilizados
- Microcontrolador (exemplo: **RP2040** ou similar)
- Microfone eletreto com circuito amplificador
- Matriz de LEDs **WS2812** (5x5)
- Biblioteca **KISS FFT** para processamento do sinal
- Fonte de alimentação compatível (5V para LEDs e 3.3V para o microcontrolador)

## Diagrama do Sistema
```
Microfone -> ADC -> DMA -> FFT -> Mapeamento de Intensidade -> Matriz de LEDs
```

## Instalação e Configuração
1. Clone este repositório:
   ```bash
   git clone https://github.com/seu-usuario/analisador-frequencia-leds.git
   ```
2. Instale a biblioteca **KISS FFT** (se necessário):
   - Inclua os arquivos da biblioteca no diretório do projeto.
   - Configure os caminhos no **CMakeLists.txt** ou no **Makefile**, conforme o ambiente de desenvolvimento.
3. Compile e carregue o código para o microcontrolador.

## Uso
- Ao ligar o sistema, a matriz de LEDs exibirá os padrões de intensidade correspondentes às frequências captadas.
- Sons graves ativam LEDs nas fileiras inferiores, enquanto sons agudos ativam LEDs superiores.
- Teste com diferentes tipos de som (música, voz, ruídos) para visualizar o comportamento do espectro.
- https://youtu.be/aFTzxG1ZHOc VIDEO DO FUNCIONAMENTO
## Estrutura do Código
```
/
|-- src/
|   |-- main.c       # Código principal
|   |-- fft.c        # Processamento FFT
|   |-- leds.c       # Controle da matriz de LEDs
|   |-- adc_dma.c    # Configuração do ADC e DMA
|-- include/
|   |-- fft.h        # Definição das funções FFT
|   |-- leds.h       # Definição das funções para LEDs
|-- README.md        # Documentação do projeto
```

## Dependências
- **Biblioteca KISS FFT** para processamento de sinais
- **Biblioteca NeoPixel** para controle dos LEDs WS2812 (se necessário)

## Licença
Este projeto está licenciado sob a [MIT License](LICENSE).

## Autor
- **Seu Nome** - Desenvolvedor

## Contribuições
Se desejar contribuir, faça um fork do repositório e envie um pull request com melhorias ou correções.

## Referências
- [KISS FFT - GitHub](https://github.com/mborgerding/kissfft)
- [NeoPixel WS2812 - Documentação Oficial](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf)

