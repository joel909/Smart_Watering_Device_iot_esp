# Smart_Watering_Device_iot_esp

Hello,
This is the code for a automatic plant watering device, The device can automatically water the plants everyday at a fixed time and it turns off automatically after a set duration of time. The automatic turn on time can be set from a mobile application(code on github), even the automatic turn off duration can be set from the application.This auto turn offf time is set with regards to the number of plants to water ,if its more then the duration can be set higher to water more plants.

# Getting started 
To set up the device and make it functional we need to set up a few services on the cloud 

* Firebase Realtime DataBase
* Mqtt Server on HiveMq (You can chose your own service for this)

In addition to this you will also need to set your utc off set according to you time Zone this can be directly edited to the code and you do not need to set up any external service for this

# Setting Up Firebase Realtime Database
1)  https://console.firebase.google.com/ go to this link and set up a fireBase account
2)  Create a new firebase Project
3)  Go to firebase Realtime Database
<img width="352" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/bd7394f8-6a77-4925-ab63-7f9437fa8ebb">

The tab marked in red is the realtime database

4) Click it and then on the realitme database screen you should see "Create Database" click it and then select your databse location(Select one that is near to your place)
5) Then you will be propted to set your security rules here you need to select "START IN TEST MODE" DO NOT DO PRODUCTION and then click enable.
<img width="462" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/c3fe2ec5-e6a0-442f-a86f-4301b68045c4">

You should see something like this i have hidden the parts in white as they are not ment to be shared.

6) From here select the 3 dots on the right and then you should see something like this
<img width="752" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/833b932f-5dc7-49ce-a62a-8348a6d8be87">
   
from here select the import json option and then (https://github.com/joel909/Smart_Watering_Device_iot_esp/blob/main/template_for_watering_device_databse_data.json) download this(available in the same repo) and uplaod this to the realtime database
   
8) Database is now sucessfully set up now you need the firebase credentials to be able to read and write to the database
   7.1) first you will need somehting called database secret this can be obtained by going to your project settings and then service accounts from there click "Database Secrets" and then chose "add secret" and then one will be generated you can then copy the presented secret(DO NOT SHARE THIS WITH ANYONE) and put this as the FIREBASE_AUTH value(line 18) put this value instead of the text
   
   <img width="786" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/97ed1bdf-7f08-4d2a-abfb-12db730248a7">

   7.2) then you will need the URL of the database this can be found by going to the the realtime database and then on the top there should be a link with https followed by the location of the database this link is the location/HOST of you database copy tha link.The red rectangle in the image is where the URL of the database is

   <img width="744" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/42bc6652-8521-4209-bf90-ed8ebe6e9144">

   7.2.1) We do not need the "https://" part of the location so you should remove this part. then you can put this as the FIREBASE_HOST value(line 17)
   
YOU ARE DONE WITH SETTING YOUR FIREBASE DATABASE 

#Setting up MQTT Server(ON HiveMq)
GO to the hiveMQ cloud website(https://console.hivemq.cloud/)
1) Sign in to the HiveMQ cloud and create a SERVERLESS Cluster for MQTT

   <img width="697" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/683590de-8703-4f0c-83ce-072629f75b57">

   This serverless is the only free option so please choose this

2)next we will need the connection information like the (cluster URL,Port)
   2.1) To get the cluster URI/URL and the port number  go to the cluster you created and then go to overview page and then there you can see your cluster URL along with your port number copy the Cluster URL and put this in the mqtt server variable in line 23 and then you can copy the port number and replace it in line 117 I have put a default port number(883) there if your port number is different you can replace 8883 with your port number.

   <img width="670" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/e6b19c03-ee8f-4646-ba92-8c54f6022fd7">

   2.2) Setting up username and password to connect to the server. TO do this you can go to the access management tab and then create a username and password now you can replace your username and password in line 123 in the appropriate places 

   <img width="602" alt="image" src="https://github.com/joel909/Smart_Watering_Device_iot_esp/assets/89447078/6187fd09-14c1-49f2-9f99-c4d388b25a27">

YOU ARE DONE WITH SETTING UP YOUR MQTT SERVER 

Now one last step is to set your utc off set according yo your time zone. the NTP always return GMT(GreenWich Merridian Time) if you dont know what it is all i can say is you should have studied in class. So check the difference between yor current time and the current GMT time and convert the difference from minutes or hours to seconds and then replace this value in line 46 . replace the value of 19800 and put yours



 
