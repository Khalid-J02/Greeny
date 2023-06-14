import time
import serial

# Open the serial port
ESP_port = serial.Serial('/dev/ttyUSB0', 9600)



# Keep waiting for data to read from the port
while True:
    
    # Send the character 'S'
    ESP_port.write('S'.encode())

    # Wait 2 seconds
    time.sleep(2)

    # Read a line of data
    line = ESP_port.readline()
    print(line) ;
    # If the received data was "ID"
    if line == "ID":
        # Read the next line which will contains 4 digit Id and store it in variable
        id = ESP_port.readline()
        print(id)

    # If the received data was "NoUser"
    elif line == "NoUser":
        # Keep the id variable empty
        id = ""

    # If the received data was "Done"
    #elif line == "Done":
        # Send the character 'D' to the port "tty0ACM0"
        #Arduino_port = serial.Serial('/dev/ttyACM0', 9600)
        #Arduino_port.write('D'.encode())
        # Close the port
        ESP_port.close()

        # Break out of the loop
        break

# Close the port
port.close()
