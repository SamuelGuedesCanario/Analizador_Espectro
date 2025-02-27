#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "neopixel.c"

// Definição do canal e pino do microfone no ADC
#define CANAL_MICROFONE 2
#define PINO_MICROFONE (26 + CANAL_MICROFONE)

// Parâmetros do ADC
#define DIVISAO_RELOGIO_ADC 96.f
#define AMOSTRAS 256 // Número de amostras para a FFT (deve ser potência de 2)
#define AJUSTE_ADC(x) (x * 3.3f / (1 << 12u) - 1.65f) // Converte o valor do ADC para Volts

// Definições da matriz de LEDs
#define PINO_LED 7
#define TOTAL_LEDS 25
#define COLUNAS_LEDS 5 // Número de colunas na matriz de LEDs
#define LINHAS_LEDS 5  // Número de linhas na matriz de LEDs

// Faixas de frequência (Hz) para análise
#define FAIXAS_FREQUENCIA 5
const float limites_faixas[FAIXAS_FREQUENCIA + 1] = {100, 500, 1000, 3000, 6000, 10000};

// Canal e configurações do DMA
uint canal_dma;
dma_channel_config config_dma;

// Buffer para armazenar as amostras do ADC
uint16_t buffer_adc[AMOSTRAS];

// Declaração das funções
void coletar_amostras_microfone();
void calcular_fft(float *saida_fft);
void atualizar_matriz_leds(float *saida_fft);
int obter_indice_led(int coluna, int linha);

/**
 * Captura os valores do ADC utilizando DMA.
 */
void coletar_amostras_microfone() {
    adc_fifo_drain(); // Limpa o buffer FIFO do ADC
    adc_run(false); // Desliga o ADC antes de configurar o DMA

    // Configura e ativa o DMA para transferir dados do ADC para o buffer
    dma_channel_configure(canal_dma, &config_dma,
        buffer_adc, // Buffer de destino
        &(adc_hw->fifo), // Fonte dos dados (FIFO do ADC)
        AMOSTRAS, // Número de amostras a coletar
        true // Inicia a transferência
    );

    // Liga o ADC e aguarda a captura completa
    adc_run(true);
    dma_channel_wait_for_finish_blocking(canal_dma);
    adc_run(false); // Desliga o ADC após a captura
}

/**
 * Implementação do algoritmo de FFT (Cooley-Tukey).
 * Converte o sinal capturado no domínio do tempo para o domínio da frequência.
 */
void fft(float *real, float *imaginario, int n) {
    if (n <= 1) return;

    float real_par[n / 2], imag_par[n / 2];
    float real_impar[n / 2], imag_impar[n / 2];

    for (int i = 0; i < n / 2; i++) {
        real_par[i] = real[2 * i];
        imag_par[i] = imaginario[2 * i];
        real_impar[i] = real[2 * i + 1];
        imag_impar[i] = imaginario[2 * i + 1];
    }

    fft(real_par, imag_par, n / 2);
    fft(real_impar, imag_impar, n / 2);

    for (int k = 0; k < n / 2; k++) {
        float angulo = -2.0 * M_PI * k / n;
        float cos_angulo = cos(angulo);
        float sin_angulo = sin(angulo);

        float t_real = cos_angulo * real_impar[k] - sin_angulo * imag_impar[k];
        float t_imag = sin_angulo * real_impar[k] + cos_angulo * imag_impar[k];

        real[k] = real_par[k] + t_real;
        imaginario[k] = imag_par[k] + t_imag;
        real[k + n / 2] = real_par[k] - t_real;
        imaginario[k + n / 2] = imag_par[k] - t_imag;
    }
}

/**
 * Calcula a FFT e obtém a magnitude das frequências.
 */
void calcular_fft(float *saida_fft) {
    float real[AMOSTRAS], imaginario[AMOSTRAS];

    for (int i = 0; i < AMOSTRAS; i++) {
        real[i] = AJUSTE_ADC(buffer_adc[i]);
        imaginario[i] = 0;
    }

    fft(real, imaginario, AMOSTRAS);

    for (int i = 0; i < AMOSTRAS / 2; i++) {
        saida_fft[i] = sqrt(real[i] * real[i] + imaginario[i] * imaginario[i]);
    }
}

/**
 * Atualiza a matriz de LEDs com base nas faixas de frequência analisadas.
 */
void atualizar_matriz_leds(float *saida_fft) {
    float intensidade_faixas[FAIXAS_FREQUENCIA] = {0};

    for (int i = 0; i < AMOSTRAS / 2; i++) {
        float frequencia = (i * 44100.f) / AMOSTRAS;
        for (int faixa = 0; faixa < FAIXAS_FREQUENCIA; faixa++) {
            if (frequencia >= limites_faixas[faixa] && frequencia < limites_faixas[faixa + 1]) {
                intensidade_faixas[faixa] += saida_fft[i];
                break;
            }
        }
    }

    npClear();
    for (int faixa = 0; faixa < FAIXAS_FREQUENCIA; faixa++) {
        float intensidade = intensidade_faixas[faixa] / 20;
        if (intensidade > LINHAS_LEDS) intensidade = LINHAS_LEDS;

        for (int linha = 0; linha < LINHAS_LEDS; linha++) {
            int indice_led = obter_indice_led(faixa, linha);
            uint8_t brilho = 0;

            if (intensidade > linha) {
                brilho = (uint8_t)((intensidade - linha) * 50);
                if (brilho > 80) brilho = 80;
            }

            npSetLED(indice_led, 0, 0, brilho);
        }
    }
    npWrite();
}

/**
 * Retorna o índice correspondente ao LED na matriz.
 */
int obter_indice_led(int coluna, int linha) {
    if (linha % 2 == 1) {
        coluna = COLUNAS_LEDS - 1 - coluna;
    }
    return linha * COLUNAS_LEDS + coluna;
}

int main() {
    stdio_init_all();
    sleep_ms(5000);

    npInit(PINO_LED, TOTAL_LEDS);
    adc_gpio_init(PINO_MICROFONE);
    adc_init();
    adc_select_input(CANAL_MICROFONE);

    adc_fifo_setup(true, true, 1, false, false);
    adc_set_clkdiv(DIVISAO_RELOGIO_ADC);

    canal_dma = dma_claim_unused_channel(true);
    config_dma = dma_channel_get_default_config(canal_dma);
    channel_config_set_transfer_data_size(&config_dma, DMA_SIZE_16);
    channel_config_set_read_increment(&config_dma, false);
    channel_config_set_write_increment(&config_dma, true);
    channel_config_set_dreq(&config_dma, DREQ_ADC);

    float saida_fft[AMOSTRAS / 2];

    while (true) {
        coletar_amostras_microfone();
        calcular_fft(saida_fft);
        atualizar_matriz_leds(saida_fft);
        sleep_ms(50);
    }
}
