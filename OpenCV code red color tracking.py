import cv2
import numpy as np
import websocket
import json
import time

ESP32_IP = "192.168.4.1"  # just put ur IP address from the output of arduino


# Servo angle limits
BASE_MIN = 60
BASE_MAX = 120
FRAME_WIDTH = 640
FRAME_HEIGHT = 480

# Gripper positions
GRIPPER_OPEN = 90
GRIPPER_CLOSED = 30

# Convert cx to base angle
def map_base_angle(cx):
    return int(BASE_MIN + (cx / FRAME_WIDTH) * (BASE_MAX - BASE_MIN))

# Convert cy to shoulder and elbow angles
def map_joint_angles(cy):
    shoulder_angle = int(80 + (cy / FRAME_HEIGHT) * 40)  # 80 → 120
    elbow_angle = int(100 - (cy / FRAME_HEIGHT) * 40)    # 100 → 60

    # clamp
    shoulder_angle = max(60, min(120, shoulder_angle))
    elbow_angle = max(60, min(120, elbow_angle))

    return shoulder_angle, elbow_angle

# Send servo command to ESP32
def send_to_esp32(ws, base, shoulder, elbow, gripper):
    msg = {
        "Base": base,
        "Shoulder": shoulder,
        "Elbow": elbow,
        "Gripper": gripper
    }

    print("Sending:", msg)

    for key, value in msg.items():
        ws.send(f"{key},{value}")
        time.sleep(0.02)  # allows ESP32 to process

# HSV color ranges
blue_lower = np.array([100, 150, 70])
blue_upper = np.array([140, 255, 255])

red_lower1 = np.array([0, 120, 70])
red_upper1 = np.array([10, 255, 255])
red_lower2 = np.array([170, 120, 70])
red_upper2 = np.array([180, 255, 255])


def detect_color(frame):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    blue_mask = cv2.inRange(hsv, blue_lower, blue_upper)

    red_mask1 = cv2.inRange(hsv, red_lower1, red_upper1)
    red_mask2 = cv2.inRange(hsv, red_lower2, red_upper2)
    red_mask = red_mask1 | red_mask2

    return blue_mask, red_mask


def get_largest_contour_center(mask):
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        largest = max(contours, key=cv2.contourArea)
        if cv2.contourArea(largest) > 1000:
            M = cv2.moments(largest)
            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                return cx, cy
    return None


# Connect to ESP32 WebSocket
ws = websocket.WebSocket()
ws.connect(f"ws://{ESP32_IP}/RobotArmInput")

# Start camera
cap = cv2.VideoCapture(0)
cap.set(3, FRAME_WIDTH)
cap.set(4, FRAME_HEIGHT)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    blue_mask, red_mask = detect_color(frame)
    blue_center = get_largest_contour_center(blue_mask)
    red_center = get_largest_contour_center(red_mask)

    if blue_center:
        cx, cy = blue_center
        cv2.circle(frame, (cx, cy), 10, (255, 0, 0), -1)

        base_angle = map_base_angle(cx)
        shoulder, elbow = map_joint_angles(cy)

        send_to_esp32(ws, base_angle, shoulder, elbow, GRIPPER_CLOSED)

    elif red_center:
        cx, cy = red_center
        cv2.circle(frame, (cx, cy), 10, (0, 0, 255), -1)
  
        base_angle = map_base_angle(cx)
        shoulder, elbow = map_joint_angles(cy)

        send_to_esp32(ws, base_angle, shoulder, elbow, GRIPPER_CLOSED)

    else:
        # idle
        send_to_esp32(ws, 90, 90, 90, GRIPPER_OPEN)

    cv2.imshow("Robot Eye", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
ws.close()
