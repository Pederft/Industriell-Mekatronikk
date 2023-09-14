#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include </opt/homebrew/Cellar/mosquitto/2.0.15/include/mosquitto.h>



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


/* Callback called when the client knows to the best of its abilities that a
 * PUBLISH has been successfully sent. For QoS 0 this means the message has
 * been completely written to the operating system. For QoS 1 this means we
 * have received a PUBACK from the broker. For QoS 2 this means we have
 * received a PUBCOMP from the broker. */
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Message with mid %d has been published.\n", mid);
}

int get_temperature(void)
{
	sleep(1); /* Prevent a storm of messages - this pretend sensor works at 1Hz */
	return random()%100;
}

/* This function pretends to read some data from a sensor and publish it.*/
void publish_sensor_data(struct mosquitto *mosq)
{
    char payload[20];
    int temp;
    int rc;

    /* Get our pretend data */
    temp = get_temperature();
    /* Print it to a string for easy human reading - payload format is highly
     * application dependent. */
    snprintf(payload, sizeof(payload), "%d", temp);

    /* Publish the message */
    rc = mosquitto_publish(mosq, NULL, "hello", strlen(payload), payload, 0, false);
    if(rc != MOSQ_ERR_SUCCESS){
        fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
    }
}


int main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc;

	/* Required before calling other mosquitto functions */
	mosquitto_lib_init();

    mosquitto_message_callback_set(mosq, on_message);


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
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_publish_callback_set(mosq, on_publish);

	/* Connect to 192.168.1.5 on port 1885, with a keepalive of 60 seconds.
	 * This call makes the socket connection only, it does not complete the MQTT
	 * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
	 * mosquitto_loop_forever() for processing net traffic. */
	rc = mosquitto_connect(mosq, "192.168.1.5", 1885, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	/* Run the network loop in a background thread, this call returns quickly. */
	rc = mosquitto_loop_start(mosq);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return 1;
	}

	while (1) {
		publish_sensor_data(mosq);
	}


	mosquitto_lib_cleanup();

	return 0;

}
