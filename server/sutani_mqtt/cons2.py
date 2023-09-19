from kafka import KafkaConsumer
from json import loads
consumer = KafkaConsumer(
    'sutani_telemetry',
     bootstrap_servers=['tampan.tech:9092'],
     auto_offset_reset='latest',
     enable_auto_commit=True,
     group_id='test-consumer-group',
     value_deserializer=lambda x: loads(x.decode('utf-8')))

for message in consumer:
     print(message.value)
