from encodings import utf_8
import requests
import json
import paho.mqtt.client as mqtt

broker = "192.168.0.74"
country = ""
stat = 0
weatherstatus = []
templist = []
tempstatus = ""
items = {"Rain": {"umbrella"}, "Clear": {"sunglasses", "suncream"}, "Drizzle" : {"umbrella"}, "chilly" : {"coat"}, "cold" : {"gloves", "beanie","padded jacket"}}

def warmness(tmp):
    if tmp < 20 and tmp > 0:
        return "chilly"
    elif tmp < 0:
        return "cold"
    else: 
        return "Warm"

def checklist():
    final = ""
    weatherset = set(weatherstatus)
    tempstatus = warmness(max(templist))
    final = final + "Today's weather is " + tempstatus + "\n"
    itemset = {"phone", "wallet", "earphones", "carkey"}

    for i in list(weatherset):
        if i in items:
            itemset.update(items.get(i))

    if tempstatus in items:
        itemset.update(items.get(tempstatus))
    final = final + "have you made sure you have the following?" + "\n" + str(itemset)
    return final

def forecast():
    url = "https://community-open-weather-map.p.rapidapi.com/forecast"
    final = ""
    #country = input('Enter your city and country (city, country code): ')
    #country = "seoul, kr"
    querystring = {"q":"{}".format(country),"units":"metric"}

    headers = {
        "X-RapidAPI-Host": "community-open-weather-map.p.rapidapi.com",
        "X-RapidAPI-Key": "0eb34b55bbmsh99c248b171a1739p10c688jsn0098f36e3135"
    }

    response = requests.request("GET", url, headers=headers, params=querystring)
    data = json.loads(response.text)
    for i in range(5):
        final = final + data['list'][i]['dt_txt'] + " " + data['list'][i]['weather'][0]['main'] + "\n"
        weatherstatus.append(data['list'][i]['weather'][0]['main'])
        templist.append(data['list'][i]['main']['temp_min'])
        templist.append(data['list'][i]['main']['temp_max'])
        
    
    final = final + "The highest temperature today is: " + str(max(templist)) + " degrees celsius" + "\n" + "The lowest temperature today is : " + str(min(templist)) + " degrees celsius"
    return final

######################client stuff######################

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe([("CRhouse/CURTAIN", 0), ("CRhouse/LIGHT", 0), ("COUNTRY", 0)])
    client.publish("CRhouse/COUNTRY", "R")

def on_message(client, userdata, message):
    global stat
    global result
    print("messaged received from topic", message.topic+": "+str(message.payload.decode("utf_8")))


    if(message.topic =="COUNTRY"):
        if(str(message.payload.decode("utf_8") == "R")):
            global country
            country = message.payload.decode("utf_8")
            print("location changed to", country)
    
    if(message.topic == "CRhouse/CURTAIN"):
        if(str(message.payload.decode("utf_8")) == "CURTAIN ON"):
            result = forecast()
            stat = 1
            print("messaged received from topic", message.topic+": "+str(message.payload.decode("utf_8")))
        else:
            stat = 0
            print("nothing was run")

    elif(message.topic == "CRhouse/LIGHT"):
        if(str(message.payload.decode("utf_8") == "LED OFF")):
            forecast()
            result = checklist()
            stat = 1
            print("messaged received from topic", message.topic+": "+str(message.payload.decode("utf_8")))
    

    if(stat == 1):
        client.publish("CRhouse/weather", result)
        print("weather/checklist published")
    else:
        print("nothing was published")
    


def client():
    client = mqtt.Client()
    client.on_message = on_message
    client.on_connect = on_connect
    client.connect(broker, 1883, 60)
    client.loop_forever()
    
    return 0

client()
#forecast()
#checklist()
