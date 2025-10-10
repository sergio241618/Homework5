idf_component_register(SRCS "tcp_client_main.c" "caesar.c"
                    INCLUDE_DIRS "."
                    REQUIRES "protocol_examples_common"
                    PRIV_REQUIRES nvs_flash esp_netif esp_event
                    )