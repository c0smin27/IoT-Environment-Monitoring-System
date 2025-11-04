import json
import requests
import ssl
from paho.mqtt import client as mqtt_client

# --- Configurare MQTT ---
broker = '127.0.0.1'
port = 8883
topic_temp = 'cosmin/temperature'
topic_hum = 'cosmin/humidity'
topic_voc = 'cosmin/air_quality'
mqtt_ca = r'C:\mqtt\certs\mosquitto.crt'

# --- Configurare endpoint Django REST ---
url = "https://127.0.0.1:8000/api/data/"

# stocare temporara a datelor
buffer = {}

# functie care trimite datele catre API-ul Django prin POST
def post_to_django(temp, hum, voc):
    payload = {
        'temperature': temp,
        'humidity': hum,
        'voc_index': voc
    }
    try:
        # r = requests.post(url, json=payload, verify=False) <--- TEST
        # certificatul self-signed 'server.crt' folosit pentru validarea identitatii serverului Django in timpul conexiunii TSL
        r = requests.post(url, json=payload, verify=r'D:\Licenta\licenta_django\certificate\server.crt')
        print(f"POST to Django: {payload}, response: {r.status_code} {r.text}")
    except Exception as e:
        print(f"Error POST to Django: {e}")

# functie callback apelata automat la primirea unui mesaj MQTT
def on_message(client, userdata, msg):
    topic = msg.topic
    value = msg.payload.decode()
    print(f"MQTT [{topic}] = {value}")

    # valorile primite sunt salvate in buffer
    if topic == topic_temp:
        buffer['temperature'] = float(value)
    elif topic == topic_hum:
        buffer['humidity'] = float(value)
    elif topic == topic_voc:
        buffer['voc_index'] = int(value)

    # daca toate cele 3 valori sunt complete, se trimite pachetul catre Django
    if all (k in buffer for k in ('temperature', 'humidity', 'voc_index')):
        post_to_django(buffer['temperature'], buffer['humidity'], buffer['voc_index'])
        buffer.clear() # golire buffer

# functie pentru configurare MQTT cu conexiune TLS
def connect_mqtt():
    client_id = 'bridge-client' # numele clientului MQTT
    client = mqtt_client.Client(client_id=client_id, protocol=mqtt_client.MQTTv311)

    # setare conexiune securizata TLS cu certificat
    client.tls_set(
        ca_certs=mqtt_ca,
        certfile=None,
        keyfile=None,
        tls_version=ssl.PROTOCOL_TLSv1_2
    )
    client.connect(broker, port)
    return client

# functie principala pornire client MQTT si ascultare permanenta a topicurilor
def run():
    client = connect_mqtt()
    client.subscribe([(topic_temp, 0), (topic_hum, 0), (topic_voc, 0)])
    client.on_message = on_message
    client.loop_forever() # asculta permanent

if __name__ == '__main__':
    run()
