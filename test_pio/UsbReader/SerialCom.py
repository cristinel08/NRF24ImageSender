import serial
import numpy as np
import time
import cv2

# Replace with your Pico's port and baud rate
port = "COM15"
baudrate = 921600
timeout_value = 1
# Open serial connection
ser = serial.Serial(port, baudrate)
frames = 0
fps = 0
old_time = time.time()
while True:
  # Clear buffer for new image
  size_bytes = ser.read(4)  # Assuming received bytes
  start_read = ser.read(2)
  if start_read == b'\r\n':
    size_string = size_bytes.decode()  # Decodes the bytes into a string
    image_size = int(size_string)

    print("Converted image size:", image_size)  # Output: Converted image size: 3080

    # Read image data based on the received size
    image_data = ser.read(image_size)

    # while len(received_data) < image_size:
    #   chunk = ser.read(min(image_size - len(received_data), 1024))  # Read in chunks (optional)
    #   received_data += chunk

    # Process complete JPEG image data
    if image_data:
      try:
        image = cv2.imdecode(np.frombuffer(image_data, np.uint8), cv2.IMREAD_GRAYSCALE)
        if image is not None:

          frames = frames + 1
          cv2.putText(image, fps.__str__(), (50, 50), cv2.FONT_HERSHEY_PLAIN, 1, (255, 255, 255), 2, cv2.LINE_AA)
          if time.time() - old_time > 1:
            fps = frames
            frames = 0
            old_time = time.time()
          cv2.imshow('Received Image', image)
          cv2.waitKey(1)  # Adjust delay as needed (optional)
      except Exception as e:
        print(f"Error decoding image: {e}")
        # Optionally, save the decompressed image
        # Optional: Add a small delay between frames
        # sleep(0.1)  # Adjust delay as needed (optional)
      else:
        print("No data received")
      # Reset the buffer for the next image
      # You can use libraries like PIL (Pillow) to decode and process the image
    else:
      print("No image data received")
    start_read = b''
    # Process the received image data




# Close serial connection
ser.close()
