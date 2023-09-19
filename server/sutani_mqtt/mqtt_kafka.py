
# /usr/local/kafka/bin
import paho.mqtt.client as mqtt
from pykafka import KafkaClient
import time

mqtt_broker = "tampan.tech"
mqtt_client = mqtt.Client("BridgeMQTT2Kafka")
mqtt_client.username_pw_set(username="sutani_mqtt",password="sutanipass")
mqtt_client.connect(mqtt_broker, 6996)


kafka_client = KafkaClient(hosts="tampan.tech:9092")
kafka_topic = kafka_client.topics['sutani_telemetry']
kafka_producer = kafka_topic.get_sync_producer()

def on_message(client, userdata, message):
    msg_payload = str(message.payload)
    print("Received MQTT message: ", msg_payload)
    kafka_producer.produce(msg_payload.encode('utf-8'))
    print("KAFKA: Just published " + msg_payload + " to topic temperature2")

while True:
    mqtt_client.loop_start()
    mqtt_client.subscribe("temperature2")
    mqtt_client.on_message = on_message
    time.sleep(300)
    mqtt_client.loop_stop()
