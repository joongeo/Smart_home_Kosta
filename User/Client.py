import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe([("CRhouse/#", 0), ("TIME", 0)])

def on_message(client, userdata, msg):
    global stat
    global result
    #global open1, open2, close1, close2
    print("messaged received from topic:", msg.topic+ "\n" + str(msg.payload.decode("utf_8")) + "\n \n")
    if(str(msg.payload.decode("utf_8")) == "T"):
        stat = 1
        result = input("What time do you want the curtains to open? (hour, min): ")
        result = result + ", " + input("What time do you want the curtains to close? (hour, min): ")
    elif(str(msg.payload.decode("utf_8")) == "R"):
        stat = 2
        result = input('Enter your city and country (city, country code): ')
    else:
        stat = 0

    if(stat == 1):
        client.publish("TIME", result)
    if(stat == 2):
        client.publish("COUNTRY", result)
        print("Location set to", result)
    else:
        client.publish("CRhouse/CURTAIN", "stat is not working")
        print("stat is not working")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.0.74", 1883, 60)
client.loop_forever()