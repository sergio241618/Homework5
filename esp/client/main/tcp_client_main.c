#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "protocol_examples_common.h"
#include "caesar.h"

#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "TCP_CLIENT";

// --- LÓGICA DEL CLIENTE TCP ---
void tcp_client_task(void *pvParameters)
{
    uint8_t tx_buffer[128];
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    
    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);

        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            close(sock);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "Successfully connected");

        // Prepara y cifra el mensaje
        const char* plaintext_payload = "Sparta123!";
        uint8_t shift = 7;
        size_t encrypted_len = caesar_encrypt_bytes(plaintext_payload, shift, tx_buffer, sizeof(tx_buffer));

        // Envía el mensaje cifrado
        err = send(sock, tx_buffer, encrypted_len, 0);
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        } else {
            ESP_LOGI(TAG, "Message sent, %zu bytes written", encrypted_len);
            // Espera la respuesta del servidor (eco)
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
            } else {
                rx_buffer[len] = 0; // Null-terminate
                ESP_LOGI(TAG, "Received %d bytes", len);
            }
        }
        
        ESP_LOGI(TAG, "Shutting down socket and restarting...");
        shutdown(sock, 0);
        close(sock);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

// --- FUNCIÓN PRINCIPAL ---
void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}