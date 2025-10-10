/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <string.h>
#include "caesar.h" // <-- Asegúrate que este archivo existe en la carpeta 'main'
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"

#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "TCP_CLIENT";

void tcp_client_task(void *pvParameters)
{
    // --- USA BUFFERS SEPARADOS PARA MÁS CLARIDAD ---
    uint8_t tx_buffer[128]; // Buffer para enviar datos (transmitir)
    char rx_buffer[128];    // Buffer para recibir datos

    char host_ip[] = HOST_IP_ADDR;
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    while (1) {
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);

        int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue; // Intenta de nuevo
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue; // Cierra el socket e intenta de nuevo
        }
        ESP_LOGI(TAG, "Successfully connected");

        // --- SECCIÓN CORREGIDA ---

        // 1. Prepara el mensaje y el shift
        const char* plaintext_payload = "Sergio Aguirre 12345"; // Tu mensaje
        uint8_t shift = 7; // Elige tu desplazamiento

        // 2. Llama a la función de cifrado y guarda el NÚMERO de bytes escritos
        size_t encrypted_len = caesar_encrypt_bytes(plaintext_payload, shift, tx_buffer, sizeof(tx_buffer));

        // 3. Envía el buffer cifrado, usando la longitud devuelta por la función
        err = send(sock, tx_buffer, encrypted_len, 0);
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            break; // Si falla el envío, rompe el bucle interno
        }
        ESP_LOGI(TAG, "Message sent, %zu bytes written", encrypted_len);

        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
        } else {
            rx_buffer[len] = 0; // Null-terminate
            ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
            ESP_LOGI(TAG, "%s", rx_buffer); // Muestra la respuesta cifrada del servidor
        }

        // --- FIN DE LA SECCIÓN CORREGIDA ---

        ESP_LOGI(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
        vTaskDelay(5000 / portTICK_PERIOD_MS); // Espera 5 segundos antes de volver a conectar
    }
    vTaskDelete(NULL);
}