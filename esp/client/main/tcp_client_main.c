/*
 * Este es un EJEMPLO de cómo debería lucir tu archivo principal.
 * Asegúrate de tener estas inclusiones y esta estructura en app_main.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h" // For Wi-Fi connection

// --- Prototipo de tu función de tarea ---
// Coloca esto al inicio del archivo, antes de app_main
void tcp_client_task(void *pvParameters);


// --- Función Principal ---
void app_main(void)
{
    // --- Inicialización del Sistema (requerido para Wi-Fi) ---
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect()); // Conecta al Wi-Fi configurado en menuconfig

    // --- Creación de la Tarea ---
    // Aquí es donde le decimos a FreeRTOS que ejecute tu función tcp_client_task
    xTaskCreate(tcp_client_task, // La función que se ejecutará
                "tcp_client",    // Un nombre para la tarea (útil para depuración)
                4096,            // Tamaño de la pila (stack size) para la tarea
                NULL,            // Parámetros para la tarea (no necesitamos ninguno)
                5,               // Prioridad de la tarea
                NULL);           // Handle de la tarea (no lo necesitamos)
}