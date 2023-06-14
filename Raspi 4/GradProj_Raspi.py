from keras.models import load_model  # TensorFlow is required for Keras to work
import cv2  # Install opencv-python
import numpy as np
import serial
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

import time
import serial

#Setting up the Raspi 

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("./keras_model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# CAMERA can be 0 or 1 based on default camera of your computer
camera = cv2.VideoCapture(0)


# Firebase 
config =  {
  "apiKey": "AIzaSyAewew9U70UABYYDplmzZv6-pMx8e_SS3c",
  "authDomain": "hardware-graduation-project.firebaseapp.com",
  "databaseURL": "https://hardware-graduation-project-default-rtdb.firebaseio.com/",
  "storageBucket": "hardware-graduation-project.app spot.com",
}


# Initialize the Firebase Admin SDK
cred = credentials.Certificate('./cred.json')
firebase_admin.initialize_app(cred)

# Get a reference to the Firestore database
db = firestore.client()


# Serial init

ESP_port = serial.Serial('/dev/ttyUSB0', 9600)
if not ESP_port.isOpen():
    ESP_port.open()

ESP_port.reset_input_buffer()

Arduino_port = serial.Serial('/dev/ttyACM0', 9600)
if not Arduino_port.isOpen():
    Arduino_port.open()

Arduino_port.reset_input_buffer()

id = "" 	# default id is null
Waste_Category = ""
points = 0 


############################# Finishing Setup the Raspi ############################# 

def classify_type():
    elections = [0, 0, 0]
    for i in range(0,5):
        ret, image = camera.read()
        image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)

        # Make the image a numpy array and reshape it to the models input shape.
        image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)
        # Normalize the image array
        image = (image / 127.5) - 1
        # Predicts the model
        prediction = model.predict(image)
        index = np.argmax(prediction)
        #class_name = class_names[index]
        confidence_score = prediction[0][index]
        elections[index] += confidence_score

 
    for i in range (0,3):
        avg_prediction = elections[i] - int(elections[i])
        avg_prediction = avg_prediction * 100
		# by here I will have array with the percentage for each category
        elections[i] = avg_prediction
		# by end of the loop we will have the data stored in array
		
    
    print(elections)
    
    #Category_else = "SomethingElse"		## this will be used in case the item wasn't fall under the categories
    
    max_category_value = max(elections)
    max_category_index = elections.index(max_category_value)
    if max_category_value < 67:
        return class_names[3]
    else:
        return class_names[max_category_index]
    	



def Update_Points():
    points = 0
    print(Waste_Category)
    if(Waste_Category == 'G'):
        points = 10
    elif Waste_Category == 'P':
        points = 25
    elif Waste_Category == 'M':
        points = 15
    elif Waste_Category == 'E':
        points = 5
		
    collection_ref = db.collection("Users")
    doc_id  = id.decode('utf-8').strip()
    
    doc_ref = collection_ref.document(doc_id)
    doc_snapshot = doc_ref.get()
    
    if doc_snapshot.exists:
		# Access the "points" field in the document
	    points = doc_snapshot.get("Points") + points
	    doc_ref.update({'Points' : points})
    else:
        print("Document does not exist")
    
    

def Wait_From_Ard():
	#
    while True:
		#
        command = Arduino_port.readline()
        #print(command)
        if b"Classify" in command:
			#
            Waste_Category = classify_type()[0]
            break
        
           
       

def Send_to_ard():
	Arduino_port.write(Waste_Category.encode())
	print(Waste_Category)

	

while True:
    
    # Read a line of data
    line = ESP_port.readline()
    print(line)
    
    # If the received data was "ID"
    if b"ID" in line:
        id = ESP_port.readline()
        print(id) 
        #Wait_From_Ard()
        Waste_Category = classify_type()[0]
        Send_to_ard()
        Update_Points()
        
		# Read the next line which will contains 4 digit Id and store it in variable
		
	# If the received data was "NoUser"
    elif b"NoUser" in line:
        id = ""
        #Wait_From_Ard()
        Waste_Category = classify_type()[0]
        Send_to_ard()
		# Keep the id variable empty
		
    
    if b"Done" in line:
        print("Hehehehehehe")
		# Break out of the loop
#    print()
        





