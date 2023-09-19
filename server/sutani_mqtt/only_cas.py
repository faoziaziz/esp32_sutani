from cassandra.cluster import Cluster

# Provide the contact points of the Cassandra nodes
contact_points = [ 'localhost']

# Create a Cluster instance
cluster = Cluster(contact_points)

# Connect to the cluster
session = cluster.connect()

row = session.execute("SELECT release_version FROM system.local").one()
if row:
    print(row[0])
