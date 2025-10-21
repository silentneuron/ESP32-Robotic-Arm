import cv2
import numpy as np
import websocket
import json

ESP32_IP = "192.168.4.1"  # Change if different as per the terminal output from arduino code said earlier

# Put the Servo angle limits
BASE_MIN = 60
BASE_MAX = 120
FRAME_WIDTH = 640  # Width of camera frame

# Fixed angles for pickup change it as per environment
SHOULDER_ANGLE = 100
ELBOW_ANGLE = 80
GRIPPER_OPEN = 90
GRIPPER_CLOSED = 30

# Convert cx (0 to FRAME_WIDTH) to base angle (BASE_MIN to BASE_MAX)
def map_base_angle(cx):
    return int(BASE_MIN + (cx / FRAME_WIDTH) * (BASE_MAX - BASE_MIN))

# Send servo command to ESP32
def send_to_esp32(ws, base, shoulder, elbow, gripper):
    msg = {
        "Base": base,
        "Shoulder": shoulder,
        "Elbow": elbow,
        "Gripper": gripper
    }
    ws.send(json.dumps(msg))

# HSV color ranges
blue_lower = np.array([100, 150, 70])
blue_upper = np.array([140, 255, 255])
red_lower1 = np.array([0, 120, 70])
red_upper1 = np.array([10, 255, 255])
red_lower2 = np.array([170, 120, 70])
red_upper2 = np.array([180, 255, 255])

def detect_color(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Blue mask
    blue_mask = cv2.inRange(hsv, blue_lower, blue_upper)
    # Red mask (combine two ranges)
    red_mask1 = cv2.inRange(hsv, red_lower1, red_upper1)
    red_mask2 = cv2.inRange(hsv, red_lower2, red_upper2)
    red_mask = red_mask1 | red_mask2

    return blue_mask, red_mask

def get_largest_contour_center(mask):
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest = max(contours, key=cv2.contourArea)
        if cv2.contourArea(largest) > 1000:  # avoid bg noise
            M = cv2.moments(largest)
            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                return cx, cy
    return None

# Connect to ESP32 WebSocket
ws = websocket.WebSocket()
ws.connect(f"ws://{ESP32_IP}/RobotArmInput")

# Camera start
cap = cv2.VideoCapture(0)
cap.set(3, FRAME_WIDTH)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    blue_mask, red_mask = detect_color(frame)
   # to only get the center
    blue_center = get_largest_contour_center(blue_mask)
    red_center = get_largest_contour_center(red_mask)
    
    # the conditions change as per required
    
    if blue_center:
        cx, cy = blue_center
        cv2.circle(frame, (cx, cy), 10, (255, 0, 0), -1)
        base_angle = map_base_angle(cx)
        print(f"Blue at {cx} → base angle: {base_angle}")
        send_to_esp32(ws, base_angle, SHOULDER_ANGLE, ELBOW_ANGLE, GRIPPER_CLOSED)

    elif red_center:
        cx, cy = red_center
        cv2.circle(frame, (cx, cy), 10, (0, 0, 255), -1)
        base_angle = map_base_angle(cx)
        print(f"Red at {cx} → base angle: {base_angle}")
        send_to_esp32(ws, base_angle, SHOULDER_ANGLE, ELBOW_ANGLE, GRIPPER_CLOSED)

    else:
        # No object found stays idle
        send_to_esp32(ws, 90, SHOULDER_ANGLE, ELBOW_ANGLE, GRIPPER_OPEN)

    cv2.imshow("Robot Eye", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
ws.close()


