# Greeny
Greeny -HW Graduation Proj- is a waste management system, which basically classify the waste thrown by the users into 4 categories (plastic, metal, glass and something else). And for each waste type user throw, a specific number of points will be added to the user account ..

## System Specification
There are 3 microcontrollers control the whole system:
1) Arduio: which is basically responsible on the mechanic movement in the system
2) Raspberry pi 4: which is responisble on classify the rubbish by using the machine learning model with the help of camera module. Moreover it will update the points for the user. Each time the user throw rubbish, based on it's classificatrion a specific number points will be added.
3) ESP32-Wroom: this microcontroller is responisble on Signing in method. The system will provide 3 ways to sign in, either by using QR, Keypad, or if he don't want to benifit from the point system, he could chse the third sign in method (NoUser), which basically will let him get rid of the rubbish.


## Appendix
This link contains the Project Demo: https://drive.google.com/file/d/1kIPslHWc7SabLGzbOJuV0grQZUhGYl9k/view?usp=drive_link

This link for the mobile application APK: https://drive.google.com/file/d/1enpYlnumaLY0box3Mn6WuKz8yfe0itoF/view?usp=drive_link

