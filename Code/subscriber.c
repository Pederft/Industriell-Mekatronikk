#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include </opt/homebrew/Cellar/mosquitto/2.0.15/include/mosquitto.h>

#define BROKER_ADDRESS "localhost" // Change this to the address of your MQTT broker
#define BROKER_PORT 1883
#define TOPIC "hello" // Change this to the topic you want to subscribe to

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    // This function is called when a message is received

    printf("Received message on topic %s: %s\n", message->topic, (char*)message->payload);
}

int main(int argc, char* argv[])
{
    struct mosquitto *mosq = NULL;
    int rc;

    // Initialize mosquitto library
    mosquitto_lib_init();

    // Create mosquitto client
    mosq = mosquitto_new(NULL, true, NULL);

    // Connect to the broker
    rc = mosquitto_connect(mosq, BROKER_ADDRESS, BROKER_PORT, 60);

    if(rc != MOSQ_ERR_SUCCESS)
    {
        printf("Failed to connect to broker, return code %d\n", rc);
        mosquitto_lib_cleanup();
        exit(-1);
    }

    // Subscribe to the topic
    rc = mosquitto_subscribe(mosq, NULL, TOPIC, 0);

    if(rc != MOSQ_ERR_SUCCESS)
    {
        printf("Failed to subscribe to topic, return code %d\n", rc);
        mosquitto_disconnect(mosq);
        mosquitto_lib_cleanup();
        exit(-1);
    }

    // Set callback for incoming messages
    mosquitto_message_callback_set(mosq, on_message);

    // Loop and handle incoming messages
    while(1)
    {
        rc = mosquitto_loop(mosq, -1, 1);

        if(rc != MOSQ_ERR_SUCCESS)
        {
            printf("Error in mosquitto_loop, return code %d\n", rc);
            mosquitto_disconnect(mosq);
            mosquitto_lib_cleanup();
            exit(-1);
        }
    }

    // Disconnect from the broker and cleanup
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

