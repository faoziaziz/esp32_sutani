#import KafkaConsumer from Kafka library
from kafka import KafkaConsumer
from cassandra.cluster import Cluster

# Import sys module
import sys
import time


contact_points=['localhost']

cluster = Cluster(contact_points)
session = cluster.connect()

# Define server with port
bootstrap_servers = ['tampan.tech:9092']

# Define topic name from where the message will recieve
topicName = 'sutani_telemetry'

# Initialize consumer variable
consumer = KafkaConsumer (topicName, group_id ='group1',
        bootstrap_servers = bootstrap_servers)

row = session.execute("SELECT release_version FROM system.local").one()



if row:
    print(row[0])

session.set_keyspace('sut_telemetry')
session.execute('USE sut_telemetry')
stmt = session.prepare("INSERT INTO kai_telemetry (kai_id, kai_data) values (?, ?) IF NOT EXISTS");
# Read and print message from consumer

while True:
    for msg in consumer:
        print("Topic Name=%s,Message=%s"%(msg.topic,msg.value))
        results = session.execute(stmt, [str(round(time.time()*1000)),msg.value.decode('utf-8')])

# Terminate the script
sys.exit()
