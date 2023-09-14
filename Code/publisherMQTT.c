#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include </opt/homebrew/Cellar/mosquitto/2.0.15/include/mosquitto.h>
#include <signal.h>
#include <time.h>


void publish_sensor_data(struct mosquitto *mosq)
{
    char payload[] = "Hello World!";
    int rc;

    /* Publish the message */
    rc = mosquitto_publish(mosq, NULL, "hello", strlen(payload), payload, 0, false);
    if(rc != MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
    }
}


/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    if(reason_code == 0){
        printf("Connected to broker.\n");

        /* Subscribe to the topic */
        mosquitto_subscribe(mosq, NULL, "hello", 0);
    } else {
        fprintf(stderr, "Failed to connect: %s\n", mosquitto_connack_string(reason_code));
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    printf("Received message on topic %s: %s\n", msg->topic, (char *)msg->payload);
}

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Message with mid %d has been published.\n", mid);
}



//Main:
int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc;

	/* Required before calling other mosquitto functions */
	mosquitto_lib_init();

	/* Create a new client instance.
	 * id = NULL -> ask the broker to generate a client id for us
	 * clean session = true -> the broker should remove old sessions when we connect
	 * obj = NULL -> we aren't passing any of our private data for callbacks
	 */
	mosq = mosquitto_new(NULL, true, NULL);
	if(mosq == NULL){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	mosquitto_username_pw_set(mosq, "Lars", "123");

	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_publish_callback_set(mosq, on_publish);

	/* Connect to 192.168.1.5 on port 1885, with a keepalive of 60 seconds.
	 * This call makes the socket connection only, it does not complete the MQTT
	 * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
	 * mosquitto_loop_forever() for processing net traffic. */
	rc = mosquitto_connect(mosq, "127.0.0.1", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	/* Loop and publish a message each second */
	while(1) {
	    publish_sensor_data(mosq);
	    sleep(1);
	}

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return 0;
}
