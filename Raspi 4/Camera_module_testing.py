from keras.models import load_model  # TensorFlow is required for Keras to work
import cv2  # Install opencv-python
import numpy as np
import serial
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore


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
  "storageBucket": "hardware-graduation-project.appspot.com",
}




# Initialize the Firebase Admin SDK
cred = credentials.Certificate('./cred.json')
firebase_admin.initialize_app(cred)

# Get a reference to the Firestore database
db = firestore.client()




# Serial init

'''

inputSerial = serial.Serial('/dev/ttyACM0', 9600)

if not inputSerial.isOpen():

    inputSerial.open()

inputSerial.reset_input_buffer()



outputSerial = serial.Serial('/dev/ttyUSB0', 115200)

if not outputSerial.isOpen():

    outputSerial.open()

outputSerial.reset_input_buffer()

'''









def classify_type():
    elections = [0, 0, 0]
    for i in range(0,5):
        ret, image = camera.read()
        image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)
        # Show the image in a window
        #cv2.imshow("Webcam Image", image)

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

    top_index = 0
    print(elections)
     
    for i in range (0,3):
        elections[i] = (elections[i] - int(elections[i])) * 100
    print(elections)
    
    #for i in range(1,3): 
     #   if elections[i] > elections[top_index]:
      #      top_index = i
    
    max_val = max(elections)
    if(max_val < 70):
        return class_names[3]
    else:    
        return class_names[elections.index(max_val)]




print(classify_type())
